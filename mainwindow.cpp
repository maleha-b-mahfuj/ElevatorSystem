#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QList>
#include "elevatorsystem.h"
#include <QScrollArea>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QScrollArea* scroll = new QScrollArea;
    scroll->setWidgetResizable(true);
    QWidget* mainCentral = new QWidget;
    const int floorNum = 4;
    const int elevatorNum = 2;
    elevatorSystem* elevatorControl = new elevatorSystem(floorNum, elevatorNum, mainCentral, parent);

    scroll->setWidget(mainCentral);
    setCentralWidget(scroll);

}

MainWindow::~MainWindow()
{
    delete ui;
}


