#include "elevator.h"
#include <QDebug>
#include <functional>
#include <QScrollArea>
#include <QtCore>
#include <QtMath>
#include <QTimer>
#include "floor.h"

Elevator::Elevator(int num, QObject *parent)
    : QObject{parent}, num(num)
{
    panel = new QWidget;
    display = new QTextEdit;
    display->setFixedSize(110,60);
    display->setAlignment(Qt::AlignCenter);
}


void Elevator::addDoors(QVector<Floor*> floors) {
    for (Floor* floor: floors){
        hoverWidget* lightSensorWid = new hoverWidget;
        //lightSensorWid->setStyleSheet("background-color: #F9D649");
        //comebaxktothisluvs
        connect(lightSensorWid, &hoverWidget::lightSensorDisturbed, [this, floor](){
            qDebug() << "Light interpution in elevator " << QString::number(num);
            lightSensor = true;
            floor->openDoors(num);
        });

        connect(lightSensorWid, &hoverWidget::lightSensorNormal, [this](){
            lightSensor = false;
        });

        connect(lightSensorWid, &hoverWidget::lightTooMany, [this, floor](){
            lightSensor = false;
            safetyScenerioResponse("Too many interruptions.");
            floor->openDoors(num);
        });
        floor->addDoors(num, lightSensorWid);
    }
}


void Elevator::moveTo(Floor* floor){
    currentFloor->hideDoors(num); //hide the doors in the current floor
    currentFloor = floor;

    currentFloor->openDoors(num);
    currentFloor->PassengerUpdate(num, numPassengers);
    QPushButton* button = panel->findChild<QPushButton*>(QString::number(currentFloor->getFloor()));
    button->setStyleSheet("background-color: #F9D649");

    display->clear();
    if (issue) {
        safetyScenerioResponse("Safety Floor. Please disembarak");
    } else {
        display->append("Floor " + QString::number(floor->getFloor()));
    }

}


void Elevator::processRequests() {
    if (!destFloors.isEmpty()) { //reached destination
        destFloors.removeFirst();
    }

    if (!destFloors.empty()) {   //next destination in the list
        moveTo(destFloors[0]);
    } else {

        //no requests and no passesngers -> can move to default floors
        if (numPassengers <= 0) {
            isFree = true;
            currentFloor->hideDoors(num);
            currentFloor = defaultFloor;
            qDebug() << "Elevator "<< QString::number(num) << "is back at default Floor " << QString::number(defaultFloor->getFloor());
        }
    }
}

QVector<Floor*> Elevator::getDestFloors(){
    return destFloors;
}


void Elevator::floorButtonPressed(Floor* floor){
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    button->setStyleSheet("background-color: #A68848");
    emit destinationRequest(floor);
}

bool Elevator::isTheElevatorFree(){
    return isFree;
}

void Elevator::setElevatorFree(bool free){
    isFree = free;
}

Floor* Elevator::getCurrentFloor(){
    return currentFloor;
}

void Elevator::setCurrentFloor(Floor *floor){
    currentFloor = floor;
}

QTextEdit* Elevator::getDisplay(){
    return display;
}


QGroupBox* Elevator::bulidFrame(QVector<Floor*> floors){
    QGroupBox *box = new QGroupBox("Elevator " + QString::number(num));
    box->setStyleSheet("QGroupBox{color: white; background-color: #03383B}");
    QHBoxLayout* boxLayout = new QHBoxLayout(box);

    QGridLayout* gridLayout = new QGridLayout(panel);

    QPushButton* helpButton = new QPushButton("Help");
    helpButton->setObjectName("help");
    helpButton->setStyleSheet("background-color: #827352");
    helpButton->setFixedSize(50,30);
    connect(helpButton, &QPushButton::clicked, this, [this](){
        issue = true;
        emit requestHelp();
    });

    QPushButton* respond = new QPushButton("Respond");
    respond->setStyleSheet("background-color: #827352");
    respond->setObjectName("respond");
    connect(respond, &QPushButton::clicked, this, [this](){
        responded = true;
    });


    QPushButton* fireButton = new QPushButton("Fire");
    fireButton->setObjectName("fire");
    fireButton->setStyleSheet("background-color: #C44317");
    connect(fireButton, &QPushButton::clicked, this, [this](){
        emit fireSignal();
    });

    fireButton->setFixedSize(50,30);

    int i=0, size = floors.size(), grid = qCeil(qSqrt(floors.size()));
    for (int j=0; j < grid; j++){
        for (int k=0; k < grid; k++){
            i++;
            QPushButton* button = new QPushButton(QString::number(i));
            button->setObjectName(QString::number(i));
            button->setStyleSheet("background-color: #F9D649");
            connect(button, &QPushButton::clicked, this, std::bind(&Elevator::floorButtonPressed, this, floors[i-1]));

            button->setFixedSize(30,30);
            gridLayout->addWidget(button, j, k);
            if (i == size){
                break;
            }
        }
        if (i == size){
            gridLayout->addWidget(helpButton, j+1, 0);
            gridLayout->addWidget(fireButton, j+1, 1);
            gridLayout->addWidget(display, j+1, 2);
            gridLayout->addWidget(respond);
            break;
        }
    }

    panel->setLayout(gridLayout);
    boxLayout->addWidget(panel);
    box->setLayout(boxLayout);

    panel->hide();
    respond->hide();
    return box;

}



void Elevator::safetyScenerioResponse(QString content){
    display->clear();
    display->append(content);
    display->setStyleSheet("background-color: #F9D649");

}


bool Elevator::getLightSensor() {
    return lightSensor;
}

void Elevator::moveToSafe(Floor* safetyFLoor){
    issue = true;
    QVector<Floor*> floors;
    floors.append(currentFloor);
    floors.append(safetyFLoor);
    destFloors = floors;
    QGroupBox* door = qobject_cast<QGroupBox*>(currentFloor->getLayout()->itemAt(num)->widget());
    QString styleSheet = door->styleSheet();

    QPushButton* help = panel->findChild<QPushButton*>("help");
    help->setEnabled(false); //not needed

    //door was closed when the fire button was pressed
    if (styleSheet.contains("background-color: #03555A")){
        processRequests();
    }
}

void Elevator::showRespondButton(){
    qDebug() << "Elevator " << QString::number(num) << "is out of service. It is being helped";
    QPushButton* respond = panel->findChild<QPushButton*>("respond");
    QPushButton* fire = panel->findChild<QPushButton*>("fire");
    fire->setEnabled(false);
    respond->show();
}
