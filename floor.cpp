#include "floor.h"
#include <QDebug>
#include <qmath.h>
#include <QTimer>
#include "hoverwidget.h"

Floor::Floor(int floorNum, QObject *parent)
    : QObject{parent}, floorNum(floorNum)
{
    layout = new QHBoxLayout;
}


void Floor::hideDoors(int elevatorNum){
    QGroupBox* door = qobject_cast<QGroupBox*>(layout->itemAt(elevatorNum)->widget());
    door->setStyleSheet("QGroupBox{color: white; background-color: #03555A}");
    QWidget *panel= door->findChild<QWidget*>("panel");
    panel->hide();
}

void Floor::PassengerUpdate(int elevatorNum, int passengers){
    QGroupBox* door = qobject_cast<QGroupBox*>(layout->itemAt(elevatorNum)->widget());
    QPushButton *leave = door->findChild<QPushButton*>("leave");
    QPushButton *open = door->findChild<QPushButton*>("open");
    QPushButton *close = door->findChild<QPushButton*>("close");
    QPushButton *enter = door->findChild<QPushButton*>("enter");
    QWidget *sensor= door->findChild<QWidget*>("sensor");
    if (passengers == 0) {
        leave->hide();
        open->hide();
        close->hide();
        sensor->hide();
    } else {
        leave->show();
        open->show();
        close->show();
        sensor->show();
    }
    enter->show();
}

void Floor::openDoors(int elevatorNum){

   if (timers[elevatorNum-1]->isActive()) {
       timers[elevatorNum-1]->stop();
   }

   timers[elevatorNum-1]->start(5000);

   QGroupBox* door = qobject_cast<QGroupBox*>(layout->itemAt(elevatorNum)->widget());

   door->setStyleSheet("QGroupBox{background-color: #0EAAB2}");
   QWidget *panel= door->findChild<QWidget*>("panel");

   QGroupBox* box = qobject_cast<QGroupBox*>(layout->parentWidget());
   QPushButton *up = box->findChild<QPushButton*>("up");
   up->setStyleSheet("background-color: #F9D649");
   QPushButton *down = box->findChild<QPushButton*>("down");
   down->setStyleSheet("background-color: #F9D649");

   panel->show();
}


void Floor::closeDoors(int elevatorNum){
    QGroupBox* door = qobject_cast<QGroupBox*>(layout->itemAt(elevatorNum)->widget());
    door->setStyleSheet("QGroupBox{color: white; background-color: #03555A}");

    //sensor is removed once the door closes
    QWidget *sensor= door->findChild<QWidget*>("sensor");
    sensor->hide();

    //cant enter/leave if the doors are closed
    QPushButton *enter = door->findChild<QPushButton*>("enter");
    enter->hide();
    QPushButton *leave = door->findChild<QPushButton*>("leave");
    leave->hide();

    emit doorClosed(elevatorNum);
}


void Floor::addDoors(int num, hoverWidget* lightSensor){
    QGroupBox *door = new QGroupBox("Elevator Door " + QString::number(num));
    door->setStyleSheet("QGroupBox{color: white; background-color: #03555A}");

    QTimer* timer = new QTimer;
    connect(timer, &QTimer::timeout, [this, timer, num](){
        timer->stop();
        emit closeDoorsSignal(num);
    });

    timers.append(timer);

    QHBoxLayout* mainLayout = new QHBoxLayout(door);

    QWidget *doorPanel = new QWidget;
    doorPanel->setObjectName("panel");
    QHBoxLayout *boxLayout = new QHBoxLayout(doorPanel);

    QPushButton* closeButton = new QPushButton("Close");
    closeButton->setStyleSheet("background-color: #F9D649");
    closeButton->setFixedSize(50,30);
    closeButton->setObjectName("close");

    connect(closeButton, &QPushButton::clicked, this, std::bind(&Floor::closeDoors, this, num));


    QPushButton* openButton = new QPushButton("Open");
    openButton->setStyleSheet("background-color: #F9D649");
    openButton->setObjectName("open");
    openButton->setFixedSize(50,30);

    connect(openButton, &QPushButton::clicked, this, [this, num]() {
        emit doorsOpen(num);
    });

    QPushButton* enterButton = new QPushButton("Enter");
    enterButton->setStyleSheet("background-color: #F9D649");
    enterButton->setObjectName("enter");
    enterButton->setFixedSize(50,30);
    connect(enterButton, &QPushButton::clicked, this, [this, num](){
        emit PassengersEnter(num);
    });

    QPushButton* leaveButton = new QPushButton("Leave");
    leaveButton->setStyleSheet("background-color: #F9D649");
    leaveButton->setObjectName("leave");
    leaveButton->setFixedSize(50,30);
    connect(leaveButton, &QPushButton::clicked, this, [this, num](){
        emit PassengersLeave(num);
    });


    boxLayout->addWidget(openButton);
    boxLayout->addWidget(closeButton);

    boxLayout->addWidget(enterButton);
    boxLayout->addWidget(leaveButton);
    enterButton->hide();
    leaveButton->hide();


    lightSensor->setObjectName("sensor");
    boxLayout->addWidget(lightSensor);


    doorPanel->setLayout(boxLayout);

    door->setLayout(mainLayout);
    mainLayout->addWidget(doorPanel);

    doorPanel->hide();
    layout->addWidget(door);
}


int Floor::checkDistance(Floor *floor){
    int distance = (floor->floorNum) - floorNum;
    return qAbs(distance);
}


QHBoxLayout* Floor::bulidFrame(){
    QHBoxLayout* boxLayout = new QHBoxLayout;
    QGroupBox *box = new QGroupBox("Floor " + QString::number(floorNum));


    boxLayout->addWidget(box);

    upButton = new QPushButton("Up");
    upButton->setObjectName("up");
    upButton->setStyleSheet("background-color: #F9D649");
    upButton->setFixedSize(50,30);
    connect(upButton, &QPushButton::clicked, this, [this](){
        upButton->setStyleSheet("background-color: #A68848");
        emit requestElevator();
    });

    downButton = new QPushButton("Down");
    downButton->setObjectName("down");
    downButton->setFixedSize(50,30);
    downButton->setStyleSheet("background-color: #F9D649");
    connect(downButton, &QPushButton::clicked, this, [this](){
        downButton->setStyleSheet("background-color: #A68848");
        emit requestElevator();
    });

    QVBoxLayout *buttonPanel = new QVBoxLayout;

    buttonPanel->addWidget(upButton);
    buttonPanel->addWidget(downButton);

    layout->addLayout(buttonPanel);

    box->setLayout(layout);
    return boxLayout;
}

int Floor::getFloor(){
    return floorNum;
}


