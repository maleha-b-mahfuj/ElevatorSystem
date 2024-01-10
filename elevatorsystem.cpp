#include "elevatorsystem.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QList>
#include <QWidget>
#include <QDebug>
#include <QScrollArea>
#include <QtMath>
#include <cmath>


elevatorSystem::elevatorSystem(int numfloors, int numElevators, QWidget* mainCentral, QObject *parent)
    : QObject{parent}, numFloors(numfloors), numElevators(numElevators)
{
    QVBoxLayout *mainLay = new QVBoxLayout(mainCentral);

    QHBoxLayout *main = new QHBoxLayout;
    QVBoxLayout *vertical = new QVBoxLayout;
    QVBoxLayout *side = new QVBoxLayout;
    side->setContentsMargins(0,30,0,0);
    side->setAlignment(Qt::AlignTop);
    QHBoxLayout *horizontal = new QHBoxLayout;

    for (int i=1; i <= numFloors; i++){
        Floor *floor = new Floor(i, this);
        vertical->addLayout(floor->bulidFrame());
        floors.append(floor);
        connect(floor, SIGNAL(requestElevator()), this, SLOT(elevatorRequested()));
        connect(floor, &Floor::closeDoorsSignal, this, &elevatorSystem::closingDoors);
        connect(floor, &Floor::doorsOpen, this, &elevatorSystem::openingDoors);
        connect(floor, &Floor::doorClosed, this, &elevatorSystem::processRequests);
        connect(floor, &Floor::PassengersEnter, this, &elevatorSystem::PassengersEntered);
        connect(floor, &Floor::PassengersLeave, this, &elevatorSystem::PassengersLeft);
    }
    QPushButton* powerButton = new QPushButton("Power Outage");
    powerButton->setFixedSize(100,30);
    powerButton->setStyleSheet("background-color: #D6B05E");

    connect(powerButton, &QPushButton::clicked, this, [this](){
        powerOut = true;
        checkSafetyScenerios();
    });

    QPushButton* fireAlarmButton = new QPushButton("Fire Alarm");
    fireAlarmButton->setFixedSize(100,30);
    fireAlarmButton->setStyleSheet("background-color: #C44317");
    connect(fireAlarmButton, &QPushButton::clicked, this, [this](){
        fireAlarm = true;
        checkSafetyScenerios();
    });

    side->addWidget(fireAlarmButton);
    side->addWidget(powerButton);


    for (int i=1; i <= numElevators; i++){
        Elevator *elevator = new Elevator(i, this);
        QGroupBox* box = elevator->bulidFrame(floors);
        elevator->addDoors(floors);
        horizontal->addWidget(box);
        elevators.append(elevator);
        connect(elevator, &Elevator::destinationRequest, this, &elevatorSystem::addDestination);
        connect(elevator, &Elevator::requestHelp, this, &elevatorSystem::helpRequested);
        connect(elevator, &Elevator::fireSignal, this, &elevatorSystem::fireAlarmReponse);
    }

    defaultFloors();
    safetyFloor = floors[qFloor(numFloors/4)];

    main->addLayout(vertical);
    main->addLayout(side);
    mainLay->addLayout(main);
    mainLay->addLayout(horizontal);
}

void elevatorSystem::defaultFloors(){
    int interval = std::ceil(static_cast<double>(numFloors) / numElevators);
    int index = 0;
    if (numElevators >= numFloors) {
        for (int i=0; i < numElevators; i++){
            elevators[i]->setDefaultFloor(floors[i%numFloors]);
            elevators[i]->setCurrentFloor(floors[i%numFloors]);
            qDebug() << "Elevator "<< QString::number(i) << "is at default Floor " << QString::number((i%numFloors));
        }
        return;
    }

    for (int i=0; i < numElevators; ++i){
        elevators[i]->setCurrentFloor(floors[index]);
        elevators[i]->setDefaultFloor(floors[index]);
        qDebug() << "Elevator "<< QString::number(i) << "is at default Floor " << QString::number(index);
        index = std::min(index + interval, numFloors-1);

    }
}

void elevatorSystem::processRequests(int elevatorNum){
    Elevator* elevator = elevators[elevatorNum-1];
    if (elevator->getPassengersNum() > maxPassengers){
        elevator->safetyScenerioResponse("Too many people on-board. Some must leave.");
    } else {
        elevator->processRequests();
    }
}

void elevatorSystem::addDestination(Floor* floor){
    Elevator* elevator = qobject_cast<Elevator*>(sender());
    if (fireAlarm || powerOut || (elevator->getElevatorIssue())) {
        qDebug() << "Elevator requests cannot be handled at the moment";
        return;
    } else {
        QVector<Floor*> destFloors = elevator->getDestFloors();

        destFloors.append(floor);
        elevator->setDestFloors(destFloors);
        elevator->moveTo(elevator->getDestFloors()[0]);
    }
}

void elevatorSystem::helpRequested(){
    QTimer* timer = new QTimer;
    Elevator* elevator = qobject_cast<Elevator*>(sender());
    Floor* floor = elevator->getCurrentFloor();
    QVector<Floor*> floors;
    elevator->setDestFloors(floors);

    connect(timer, &QTimer::timeout, [elevator](){
        if (elevator->getResponded()) {
            elevator->safetyScenerioResponse("You responded! Help is coming");
        } else {
            elevator->safetyScenerioResponse("Calling 911");}
    });

    elevator->safetyScenerioResponse("You requested help. Please respond quickly");
    elevator->showRespondButton();
    floor->hideDoors(elevator->getNumber());
    timer->start(5000);
}

void elevatorSystem::fireAlarmReponse(){
    Elevator* elevator = qobject_cast<Elevator*>(sender());
    elevator->safetyScenerioResponse("Fire! Moving to safe floor");
    elevator->moveToSafe(safetyFloor);
}

void elevatorSystem::PassengersEntered(int elevatorNum){
    Elevator* elevator = elevators[elevatorNum-1];
    elevator->setPassengersNum(elevator->getPassengersNum()+1);
    qDebug() << "Number of Passengers: " << QString::number(elevator->getPassengersNum());

    Floor* floor = elevators[elevatorNum-1]->getCurrentFloor();
    floor->PassengerUpdate(elevatorNum, elevators[elevatorNum-1]->getPassengersNum());

    if (elevator->getPassengersNum() > 0) {
        elevator->getPanel()->show();
    }

}
void elevatorSystem::openingDoors(int elevatorNum){
    Floor* floor = elevators[elevatorNum-1]->getCurrentFloor();
    floor->openDoors(elevatorNum);
    floor->PassengerUpdate(elevatorNum, elevators[elevatorNum-1]->getPassengersNum());

    QVector<Floor*> destFloors = elevators[elevatorNum-1]->getDestFloors();
    destFloors.append(floor);
    elevators[elevatorNum-1]->setDestFloors(destFloors);

}

void elevatorSystem::PassengersLeft(int elevatorNum){
    Elevator* elevator = elevators[elevatorNum-1];
    elevator->setPassengersNum(elevator->getPassengersNum()-1);
    qDebug() << "Number of Passengers: " << QString::number(elevator->getPassengersNum());

    if (elevator->getPassengersNum() == 0) {
        elevator->getPanel()->hide();
    }
    Floor* floor = elevators[elevatorNum-1]->getCurrentFloor();
    floor->PassengerUpdate(elevatorNum, elevators[elevatorNum-1]->getPassengersNum());
}

bool elevatorSystem::checkSafetyScenerios(){
    if (fireAlarm) {
        for (auto elevator: elevators){
            if (!elevator->isTheElevatorFree()) {
                elevator->safetyScenerioResponse("Fire! Moving to safe floor");
                elevator->moveToSafe(safetyFloor);
            } else {
                elevator->setCurrentFloor(safetyFloor);
            }
        }
        qDebug() << "Fire alarm has been set. Elevators are not active";
        return true;

    } else if (powerOut) {
        for (auto elevator: elevators){
            if (!elevator->isTheElevatorFree()) {
                elevator->safetyScenerioResponse("Power Outage. Moving to safe floor");
                elevator->moveToSafe(safetyFloor);
            } else {
                elevator->setCurrentFloor(safetyFloor);
            }
        }
        qDebug() << "There is a power outage; Elevators are not active";
        return true;
    }
    return false;
}

void elevatorSystem::closingDoors(int elevatorNum){
    bool lightSensor = elevators[elevatorNum-1]->getLightSensor();

    Floor* floor = qobject_cast<Floor*>(sender());

    if (!lightSensor) {
        floor->closeDoors(elevatorNum);
    }
}

void elevatorSystem::elevatorRequested(){
    if (fireAlarm || powerOut) {
        qDebug() << "Elevator requests cannot be handled at the moment";
        return;
    }

    Floor* floor = qobject_cast<Floor*>(sender());
    Elevator* nearest = nullptr;
    Elevator* secondNearest = nullptr;

    int minDistance = numFloors;
    int secDistance = numFloors;

    for (auto elevator: elevators){
        if (!elevator->getElevatorIssue()) {
            if (elevator->isTheElevatorFree()){
                int distance = floor->checkDistance(elevator->getCurrentFloor());
                if (minDistance >= distance){
                    minDistance = distance;
                    nearest = elevator;
                }
            } else {
                int distance = floor->checkDistance(elevator->getCurrentFloor());
                if (secDistance >= distance){
                    secDistance = distance;
                    secondNearest = elevator;
                }
            }
        }
    }
    if (nearest != nullptr) {
        QVector<Floor*> destFloors = nearest->getDestFloors();
        destFloors.append(floor);
        nearest->setDestFloors(destFloors);
        nearest->setElevatorFree(false);
        nearest->moveTo(nearest->getDestFloors()[0]);
    } else {
        if (secondNearest != nullptr) {
            //if all the elevators are busy -> the request goes to the one that is closest
            QVector<Floor*> destFloors = secondNearest->getDestFloors();
            destFloors.append(floor);
            secondNearest->setDestFloors(destFloors);
            secondNearest->moveTo(secondNearest->getDestFloors()[0]);
        } else {
            qDebug() << "Elevator requests cannot be handled at the moment";
            return;
        }
    }
}

