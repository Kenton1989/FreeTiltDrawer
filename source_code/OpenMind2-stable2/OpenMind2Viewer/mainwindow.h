#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<Qt3DCore>
#include<Qt3DExtras>
#include<Qt3DInput>
#include<Qt3DRender>
#include<iostream>
#include<vector>
#include<QTimer>
#include<QColor>
#include<sstream>
//#include <boost/interprocess/shared_memory_object.hpp>
//#include <boost/interprocess/mapped_region.hpp>
//#include<boost/interprocess/managed_shared_memory.hpp>
#include<zmq.hpp>
#include<stdio.h>
using namespace std;
const int POINTS_SAMPLE_RATE = 50;
struct point3f{
    float x;
    float y;
    float z;
};

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE
typedef QVector3D ivec;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    //3D Render Functions &  Objects
    Qt3DCore::QEntity* scene;
    Qt3DRender::QCamera* cam;
    Qt3DCore::QEntity* cursorEntity;
    Qt3DExtras::QSphereMesh* cursor;
    Qt3DCore::QTransform* cursorTransformer;
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void renderGround(Qt3DCore::QEntity* ent);
    void renderCylinderBetweenPoints(Qt3DCore::QEntity* ent,ivec p1,ivec p2,Qt3DExtras::QPhongMaterial* matCylinder,float r=0.5);
    void renderBall(Qt3DCore::QEntity* root, ivec p,float r=0.5);
    void renderBall(Qt3DCore::QEntity* root, ivec p,Qt3DExtras::QPhongMaterial* mat,float r=0.5);
    void drawLine(const QVector3D& start, const QVector3D& end, const QColor& color, Qt3DCore::QEntity *_rootEntity);
    const point3f getPosition();
    void moveCursor(ivec target);
    //materials
    Qt3DExtras::QPhongMaterial* redMat;
    Qt3DExtras::QPhongMaterial* greenMat;
    Qt3DExtras::QPhongMaterial* blueMat;
    Qt3DExtras::QPhongMaterial* purpleMat;
    Qt3DExtras::QPhongMaterial* cyanMat;
    Qt3DExtras::QPhongMaterial* greyMat;
    //Program Logics
    ivec prv{-1,-2,-3};
    Qt3DExtras::QPhongMaterial* totMat;
    void renderNextPoint(ivec point);
    void refreshLocation();

private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();
    void on_lineEdit_returnPressed();
    void on_pushButton_4_clicked();
    void on_pushButton_5_clicked();
    void onPointGettyTimeoff(){
        refreshLocation();
    }
private:
    const char* filter = "10001 ";
    QTimer* pointGetter;
    bool penDown;
    QVector3D lastPaintPos;
    QVector3D pos;
    QVector3D prevPos;//for updator determine lost connection
    Ui::MainWindow *ui;
    //zmq
    zmq::context_t* context;
    zmq::socket_t* subscriber;
};
#endif
