#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<cmath>
namespace core = Qt3DCore;
namespace ext = Qt3DExtras;
using mat = Qt3DExtras::QPhongMaterial;
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    QColor totcolor;
    ui->setupUi(this);
    //init workspace
    scene = new Qt3DCore::QEntity;
    //init palette
    greenMat = new Qt3DExtras::QPhongMaterial;
    greenMat->setAmbient(Qt::green);
    greenMat->setSpecular(Qt::green);
    greenMat->setShininess(0.5);
    redMat = new Qt3DExtras::QPhongMaterial;
    redMat->setAmbient(Qt::red);
    redMat->setSpecular(Qt::red);
    redMat->setShininess(0.5);
    blueMat = new Qt3DExtras::QPhongMaterial;
    blueMat->setAmbient(Qt::blue);
    blueMat->setSpecular(Qt::blue);
    blueMat->setShininess(0.5);
    //custom colors by pityhero
    purpleMat = new Qt3DExtras::QPhongMaterial;
    totcolor.setRgb(110, 77, 140);//purple
    purpleMat->setAmbient(totcolor);
    purpleMat->setSpecular(totcolor);
    purpleMat->setShininess(0.5);

    cyanMat = new Qt3DExtras::QPhongMaterial;
    totcolor.setRgb(71, 105, 122);
    cyanMat->setAmbient(totcolor);
    cyanMat->setSpecular(totcolor);
    cyanMat->setShininess(0.5);

    greyMat = new Qt3DExtras::QPhongMaterial;
    greyMat->setAmbient(Qt::gray);
    greyMat->setSpecular(Qt::gray);
    greyMat->setShininess(0.5);

    //init QTimer
    pointGetter = new QTimer;
    pointGetter->stop();
    pointGetter->setInterval(POINTS_SAMPLE_RATE);
     connect(pointGetter,SIGNAL(timeout()),this,SLOT(onPointGettyTimeoff()));
    pointGetter->start();

    //init cursor
    lastPaintPos = ivec{-1,-2,-3};
    cursorEntity = new Qt3DCore::QEntity(scene);
    cursor = new Qt3DExtras::QSphereMesh;
    cursor->setRadius(0.3);
    cursorTransformer = new Qt3DCore::QTransform;
    cursorTransformer->setTranslation(ivec{1000,1000,1000});//intentionally out-of-sight before detecting human input
    cursorEntity->addComponent(cursor);
    cursorEntity->addComponent(cursorTransformer);
    cursorEntity->addComponent(redMat);

    //debug part
//    int _whatever;
//    if (scanf("%d",&_whatever)==1){
//        printf("testing transform cursor to (0,0,0)\n");
//        cursorTransformer->setTranslation(ivec{0,0,0});
//    }
//    if (scanf("%d",&_whatever)==1){
//        printf("testing transform cursor to (0,1,1)\n");
//        cursorTransformer->setTranslation(ivec{0,1,1});
//    }

    //init program logic
    totMat = purpleMat;
    penDown = true;

    //init zmq
    context = new zmq::context_t(1);
    subscriber = new zmq::socket_t(*context,ZMQ_SUB);
    subscriber->connect("tcp://localhost:5556");
    subscriber->setsockopt(ZMQ_SUBSCRIBE, filter, strlen (filter));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::renderBall(Qt3DCore::QEntity* root,ivec p,float r){
    Qt3DCore::QEntity* ball = new Qt3DCore::QEntity(root);
    Qt3DExtras::QSphereMesh* mesh = new Qt3DExtras::QSphereMesh;
    mesh->setRadius(r);
    Qt3DCore::QTransform* trans = new Qt3DCore::QTransform;
    trans->setTranslation(p);
    ball->addComponent(mesh);
    ball->addComponent(trans);

    Qt3DExtras::QPhongMaterial* greenMat = new Qt3DExtras::QPhongMaterial;
    greenMat->setAmbient(Qt::green);
    greenMat->setSpecular(Qt::green);
    greenMat->setShininess(0.5);
    ball->addComponent(greenMat);
}
void MainWindow::renderGround(Qt3DCore::QEntity* ent){
    Qt3DCore::QEntity* planee = new Qt3DCore::QEntity(ent);
    Qt3DExtras::QPlaneMesh* plane = new Qt3DExtras::QPlaneMesh;
    plane->setWidth(500);
    plane->setHeight(500);
    Qt3DExtras::QPhongMaterial* matPlane = new Qt3DExtras::QPhongMaterial;//material 2
    matPlane->setAmbient(Qt::gray);
    matPlane->setSpecular(Qt::gray);
    matPlane->setShininess(0);
    Qt3DCore::QTransform* transform3 =  new Qt3DCore::QTransform;//location 3
    transform3->setTranslation(QVector3D(0,0,0));
    planee->addComponent(plane);
    planee->addComponent(matPlane);
    planee->addComponent(transform3);
}
void MainWindow::renderCylinderBetweenPoints(Qt3DCore::QEntity* ent,ivec p1,ivec p2,Qt3DExtras::QPhongMaterial* matCylinder,float r){
    renderBall(ent,p1,matCylinder,r);renderBall(ent,p2,matCylinder,r);
    core::QEntity* brush = new core::QEntity(ent);

    ext::QCylinderMesh* mesh = new ext::QCylinderMesh;
    auto length = (p1-p2).length();
    mesh->setLength(length);mesh->setRadius(r);

    core::QTransform* trans = new core::QTransform;
    trans->setTranslation((p1+p2)/2.0);
    //calculate rotations
    ivec delta = p2-p1;

    trans->setRotation(QQuaternion::rotationTo(ivec(0,1,0), delta));

    brush->addComponent(mesh);
    brush->addComponent(trans);
    brush->addComponent(matCylinder);
}
void MainWindow::renderBall(Qt3DCore::QEntity* root, ivec p,Qt3DExtras::QPhongMaterial* mat,float r){
    Qt3DCore::QEntity* ball = new Qt3DCore::QEntity(root);
    Qt3DExtras::QSphereMesh* mesh = new Qt3DExtras::QSphereMesh;
    mesh->setRadius(r);
    Qt3DCore::QTransform* trans = new Qt3DCore::QTransform;
    trans->setTranslation(p);
    ball->addComponent(mesh);
    ball->addComponent(trans);

    ball->addComponent(mat);
}
void MainWindow::drawLine(const QVector3D& start, const QVector3D& end, const QColor& color, Qt3DCore::QEntity *_rootEntity)
{
    auto *geometry = new Qt3DRender::QGeometry(_rootEntity);

    // position vertices (start and end)
    QByteArray bufferBytes;
    bufferBytes.resize(3 * 2 * sizeof(float)); // start.x, start.y, start.end + end.x, end.y, end.z
    float *positions = reinterpret_cast<float*>(bufferBytes.data());
    *positions++ = start.x();
    *positions++ = start.y();
    *positions++ = start.z();
    *positions++ = end.x();
    *positions++ = end.y();
    *positions++ = end.z();
    auto *buf = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::VertexBuffer, geometry);
    buf->setData(bufferBytes);


    auto *positionAttribute = new Qt3DRender::QAttribute(geometry);
    positionAttribute->setName(Qt3DRender::QAttribute::defaultPositionAttributeName());
    positionAttribute->setVertexBaseType(Qt3DRender::QAttribute::Float);
    positionAttribute->setVertexSize(3);
    positionAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    positionAttribute->setBuffer(buf);
    positionAttribute->setByteStride(3 * sizeof(float));
    positionAttribute->setCount(2);
    geometry->addAttribute(positionAttribute); // We add the vertices in the geometry

    // connectivity between vertices
    QByteArray indexBytes;
    indexBytes.resize(2 * sizeof(unsigned int)); // start to end
    unsigned int *indices = reinterpret_cast<unsigned int*>(indexBytes.data());
    *indices++ = 0;
    *indices++ = 1;

    //auto *indexBuffer = new Qt3DRender::QBuffer(geometry);
    auto *indexBuffer = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::IndexBuffer, geometry);
    indexBuffer->setData(indexBytes);

    auto *indexAttribute = new Qt3DRender::QAttribute(geometry);
    indexAttribute->setVertexBaseType(Qt3DRender::QAttribute::UnsignedInt);
    indexAttribute->setAttributeType(Qt3DRender::QAttribute::IndexAttribute);
    indexAttribute->setBuffer(indexBuffer);
    indexAttribute->setCount(2);
    geometry->addAttribute(indexAttribute); // We add the indices linking the points in the geometry

    // mesh
    auto *line = new Qt3DRender::QGeometryRenderer(_rootEntity);
    line->setGeometry(geometry);
    line->setPrimitiveType(Qt3DRender::QGeometryRenderer::Lines);
    auto *material = new Qt3DExtras::QPhongMaterial(_rootEntity);
    material->setAmbient(color);

    // entity
    auto *lineEntity = new Qt3DCore::QEntity(_rootEntity);
    lineEntity->addComponent(line);
    lineEntity->addComponent(material);
}
void MainWindow::renderNextPoint(ivec point){
    if (lastPaintPos==ivec{-1,-2,-3}){
        renderBall(scene,point,totMat);
        lastPaintPos = point;
    }
    else{
        renderCylinderBetweenPoints(scene,lastPaintPos,point,totMat,0.2);
        renderBall(scene,point,0.2);
        lastPaintPos = point;
    }
}
void MainWindow::on_pushButton_clicked()
{
    totMat = cyanMat;
    printf("changed to Cyan.\n");
}
void MainWindow::on_pushButton_2_clicked()
{
    totMat = purpleMat;
    printf("changed to Purple.\n");
}
void MainWindow::on_pushButton_3_clicked()
{
    totMat = greyMat;
    printf("changed to Grey.\n");
}
void MainWindow::on_lineEdit_returnPressed()
{
    QString text = ui->lineEdit->text();
    auto phrases = text.split(" ");
    int cnt=0;ivec tot;
    for (auto i: phrases){
        if (i.toFloat()!=0){
            tot[cnt]=i.toFloat();
            cnt=cnt+1;
        }
    }
    renderNextPoint(tot);
    ui->lineEdit->setText("");
    printf("new debug point selected: %f %f %f.\n",tot[0],tot[1],tot[2]);
}
void MainWindow::on_pushButton_4_clicked()
{
    penDown = !penDown;
    int stat = static_cast<int>(penDown);
    printf("Pen Status changed to %d.\n",stat);
}
void MainWindow::on_pushButton_5_clicked()
{
    cam->lens()->setPerspectiveProjection(45,16/9,0.1,1000);
    cam->setPosition(QVector3D(10,10,10));
    cam->setViewCenter(QVector3D(0,0,0));
    printf("camera restored.(actually not)(FIXME)\n");
}
void MainWindow::refreshLocation(){
    ivec next;
    zmq::message_t update;
    subscriber->recv(&update);
    int zipcode;float x,y,z;
    std::istringstream iss(static_cast<char*>(update.data()));
    iss>>zipcode>>x>>y>>z;
    next[0]=x;next[1]=y;next[2]=z;
    if ((prevPos-next).length()>0.2){
        if (penDown){
            renderNextPoint(next);
            moveCursor(next);
        }else{
            moveCursor(next);
        }
    }
}
void MainWindow::moveCursor(ivec target){
    cursorTransformer->setTranslation(target);
    prevPos = target;//update for detection
}
