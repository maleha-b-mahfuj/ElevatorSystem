#ifndef ELEVATOR_H
#define ELEVATOR_H

#include <QWidget>
#include <QPushButton>
#include <QFrame>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QTextEdit>
#include "floor.h"
#include <QVector>
#include "hoverwidget.h"

class Elevator : public QObject
{
    Q_OBJECT

public:
    explicit Elevator(int num, QObject *parent = nullptr);

    QGroupBox* bulidFrame(QVector<Floor*> floors);
    QVector<Floor*> getDestFloors();

    void setDestFloors(QVector<Floor*> floors){
        destFloors = floors;
    }

    void setPassengersNum(int num) {
        numPassengers = num;
    }

    int getPassengersNum() {
        return numPassengers;
    }

    QWidget* getPanel() {return panel;}

    void moveTo(Floor* floor);
    void addDoors(QVector<Floor*> floors);

    Floor* getCurrentFloor();
    int getNumber() {
        return num;
    }

    bool getResponded() {return responded;}

    QTextEdit* getDisplay();

    void setCurrentFloor(Floor* floor);
    void setDefaultFloor(Floor* floor) {
        defaultFloor = floor;
    }
    bool isTheElevatorFree();

    void setElevatorFree(bool free);
    bool getElevatorIssue() {return issue;}

    void setLightSensor(bool light);
    void moveToSafe(Floor* safetyFLoor);
    void safetyScenerioResponse(QString content);
    bool getLightSensor();
    void processRequests();
    void showRespondButton();


public slots:
    void floorButtonPressed(Floor* floor);

private:
    int num;
    bool isFree = true;
    bool lightSensor = false;
    hoverWidget* lightSensorWid;

    Floor* currentFloor;
    Floor* defaultFloor;

    QWidget* panel;
    QTextEdit* display;
    QVector<Floor*> destFloors;
    bool issue = false;
    bool responded = false;

    int numPassengers = 0;

signals:

    void destinationRequest(Floor* floor);
    void requestHelp();
    void fireSignal();
};

#endif // ELEVATOR_H
