#ifndef ELEVATORSYSTEM_H
#define ELEVATORSYSTEM_H

#include <QObject>
#include <QVector>
#include "floor.h"
#include "elevator.h"

class elevatorSystem : public QObject
{
    Q_OBJECT
public:
    explicit elevatorSystem(int numfloors, int elevators, QWidget* mainCentral, QObject *parent = nullptr);
    void setPowerOut(bool signal);
    void powerOutResponse();
    bool checkSafetyScenerios();


private:
    int numFloors;
    int numElevators;
    int maxPassengers = 5;

    Floor* safetyFloor;
    QVector<Floor*> floors;
    QVector<Elevator*> elevators;
    bool fireAlarm = false;
    bool powerOut = false;
    bool helpAlarm = false;

public slots:

    void elevatorRequested();
    void helpRequested();
    void closingDoors(int elevatorNum);
    void openingDoors(int elevatorNum);
    void fireAlarmReponse();
    void addDestination(Floor* floor);
    void processRequests(int elevatorNum);
    void PassengersEntered(int elevatorNum);
    void PassengersLeft(int elevatorNum);
    void defaultFloors();

signals:

};

#endif // ELEVATORSYSTEM_H
