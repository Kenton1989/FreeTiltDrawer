#include "mainwindow.h"
#include<iostream>
#include <QApplication>
#include<stdio.h>
int main(int argc, char *argv[])
{
    std::cout << "OpenMind 2019\nVersion 0.2 Beta\n.All Rights Reserved (c) 2019-2022\n";

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    Qt3DExtras::Qt3DWindow view;
    w.cam = view.camera();
    //w.renderGround(w.scene);
    // camera
    w.cam->lens()->setPerspectiveProjection(45,16/9,0.1,1000);
    w.cam->setPosition(QVector3D(0, 0, 0));
    w.cam->setViewCenter(QVector3D(0,0,80));

     //manipulator
    Qt3DExtras::QOrbitCameraController* manipulator = new Qt3DExtras::QOrbitCameraController(w.scene);
    manipulator->setLinearSpeed(100);
    manipulator->setLookSpeed(200);
    manipulator->setCamera(w.cam);

    view.setRootEntity(w.scene);
    view.show();

    //debug part ---------------------------
    w.drawLine(ivec(0,0,0),ivec(10,0,0),Qt::green,w.scene);
    w.renderBall(w.scene,ivec(10,0,0),w.blueMat,0.1);
    w.drawLine(ivec(0,0,0),ivec(0,10,0),Qt::green,w.scene);
    w.renderBall(w.scene,ivec(0,10,0),w.greenMat,0.1);
    w.drawLine(ivec(0,0,0),ivec(0,0,10),Qt::green,w.scene);
    w.renderBall(w.scene,ivec(0,0,10),w.redMat,0.1);
//    ivec tot{0,0,0};
//    w.renderNextPoint(tot);
    //w.renderCylinderBetweenPoints(w.scene,QVector3D(0,0,0),QVector3D(2,2,2),w.totMat,0.5);
    return a.exec();
}
