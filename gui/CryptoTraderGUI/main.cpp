
// main.cpp
#include "mainwindow.h"
#include <QApplication>
#include <QThread>
#include <QTimer>
#include <QDebug>

int main(int argc, char *argv[]) {
    qDebug() << "Entering main()"; // Debug output
    QApplication app(argc, argv);
    qDebug() << "QApplication created";
    QThread::msleep(2000); // 2-second delay
    qDebug() << "After delay";
    MainWindow w;
    w.show();
    return app.exec();
}