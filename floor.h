#ifndef FLOOR_H
#define FLOOR_H

#include <QWidget>
#include <QPushButton>
#include <QFrame>
#include <QHBoxLayout>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGroupBox>
#include <QVector>
#include "hoverwidget.h"

class Floor : public QObject
{
    Q_OBJECT
public:
    explicit Floor(int num, QObject *parent = nullptr);

    QHBoxLayout* bulidFrame();
    int getFloor();
    QHBoxLayout* getLayout(){return layout;}


    void addDoors(int num, hoverWidget* lightSensorWid);
    void hideDoors(int elevatorNum);
    int checkDistance(Floor* floor);
    void PassengerUpdate(int elevatorNum, int passengers);
    QHBoxLayout* getFloorLayout() {return layout;}


public slots:

    void openDoors(int elevatorNum);
    void closeDoors(int elevatorNum);

private:
    int floorNum;
    QPushButton *upButton;
    QPushButton *downButton;
    QHBoxLayout *layout;
    QVector <QTimer*> timers;

signals:
    void requestElevator();
    void doorClosed(int elevatorNum);
    void doorsOpen(int elevatorNum);
    void closeDoorsSignal(int elevatorNum);
    void PassengersEnter(int ElevatorNum);
    void PassengersLeave(int elevatorNum);

};

#endif // FLOOR_H
