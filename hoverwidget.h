#ifndef HOVERWIDGET_H
#define HOVERWIDGET_H

#include <QWidget>
#include <QMouseEvent>
#include <QEvent>
#include <QTimer>
#include <QDebug>

class hoverWidget : public QWidget
{
    Q_OBJECT
public:
    explicit hoverWidget(QWidget *parent = nullptr): QWidget(parent){
        setFixedSize(50,30);
        timer.setInterval(timeInterval);

        connect(&timer, &QTimer::timeout, [this](){
            if (count > maxCount) {
                emit lightTooMany();
            }
            count = 0;
            timer.stop();
        });
    }
    int getCount() {return count;}

    void enterEvent(QEvent *event) {
        Q_UNUSED(event);
        timer.start();
        count++;
        emit lightSensorDisturbed();
    }
    void leaveEvent(QEvent *event) {
        Q_UNUSED(event);
        emit lightSensorNormal();
    }

private:
    int count = 0;
    QTimer timer;
    int timeInterval = 2000;
    int maxCount = 3;

signals:
    void lightSensorDisturbed();
    void lightSensorNormal();
    void lightTooMany();
};

#endif // HOVERWIDGET_H
