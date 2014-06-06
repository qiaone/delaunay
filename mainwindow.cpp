﻿#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dviewerwindow.h"
#include "randompointsdialog.h"
#include <random>
#include <QPainter>
#include <QPen>
#include <QToolTip>
#include <QMouseEvent>
#include <QDebug>
#include <QTime>
#include <cmath>
//#include <QtAlgorithms>
//#include <qmath.h>
//#include <QSet>
//#include <limits>

const float INF = 1.0e5f;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    isTrianglated(false),
    isSelectMannually(true),
    isShowNewPoint(false),
    isShowCircle(false),
    isShowSplitTriangle(false),
    isShowBeforeFlip(false),
    isShowAfterFlip(false),
    viewer(nullptr)
{
    ui->setupUi(this);
    delaunay = std::make_shared<Delaunay>();
//    QObject::connect(delaunay.get(), SIGNAL(signalNewPoint(VHandle)),
//                     this, SLOT(slotNewPoint(VHandle)));
//    QObject::connect(delaunay.get(), SIGNAL(signalBeforeSplit(FHandle)),
//                     this, SLOT(slotBeforeSplit(FHandle)));
    QObject::connect(delaunay.get(), SIGNAL(signalBeforeFlip(HHandle,VHandle,VHandle)),
                     this, SLOT(slotBeforeFlip(HHandle,VHandle,VHandle)));
    QObject::connect(delaunay.get(), SIGNAL(signalAfterFlip()),
                     this, SLOT(slotAfterFlip()));
    isFirstTime = true;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::slotBeforeSplit(FHandle fh)
{
    qDebug() << "slotSplittingTriagnle";
    //for(auto& vh : delaunay->mesh.fv_range(fh))
    //{
    //    //auto triangle_point = delaunay->mesh.point(vh);
    //    Point p = delaunay->mesh.point(vh);
    //    Point a(-INF, -INF, 0);
    //    Point b(INF, -INF, 0);
    //    Point c(0, INF, 0);
    //    if (p == a || p == b || p == c)
    //    {
    //        return;
    //    }
    //    splitting_triangle.append(QPoint(p[0], p[1]));
    //    isShowSplittingTriangle = true;
    //}
    splitting_triangle.clear();
    bool isInfinite = false;
//    for(auto& vh : delaunay->mesh.fv_range(fh))
//    {
//        Point p = delaunay->mesh.point(vh);
//        Point a(-INF, -INF, 0);
//        Point b(INF, -INF, 0);
//        Point c(0, INF, 0);
//        if (p == a || p == b || p == c)
//        {
//            isInfinite = true;
//            break;
//        }
//    }
//    if (!isInfinite)
    {
        for(auto& vh : delaunay->mesh.fv_range(fh))
        {
            Point p = delaunay->mesh.point(vh);
            splitting_triangle.append(QPoint(p[0], p[1]));
            qDebug() << QPoint(p[0], p[1]);
        }
        isShowSplitTriangle = true;
    }
    update();
}

void MainWindow::slotAfterSplit(FHandle fh)
{

}

void MainWindow::slotBeforeFlip(HHandle hh, VHandle vh, VHandle vh_oppo)
{
    isShowAfterFlip = false;
    qDebug() << "slotBeforeFlip";
    double x1 = delaunay->mesh.point(vh)[0];
    double y1 = delaunay->mesh.point(vh)[1];
    double x2 = delaunay->mesh.point(delaunay->mesh.from_vertex_handle(hh))[0];
    double y2 = delaunay->mesh.point(delaunay->mesh.from_vertex_handle(hh))[1];
    double x3 = delaunay->mesh.point(delaunay->mesh.to_vertex_handle(hh))[0];
    double y3 = delaunay->mesh.point(delaunay->mesh.to_vertex_handle(hh))[1];
    double x4 = delaunay->mesh.point(vh_oppo)[0];
    double y4 = delaunay->mesh.point(vh_oppo)[1];

    flipping_triangles.clear();
    flipping_triangles << QPoint(x1, y1) << QPoint(x2, y2) << QPoint(x4, y4) << QPoint(x3, y3);

    in_circle_point.setX(x4);
    in_circle_point.setY(y4);

    double x0 = ((y3 - y1) * (y2 * y2 - y1 * y1) + (y3 - y1) * (x2 * x2 - x1 * x1) -
                 (y1 - y2) * (y1 * y1 - y3 * y3) - (y1 - y2) * (x1 * x1 - x3 * x3)) /
                (2 * (y1 - y2) * (x3 - x1) - 2 * (y3 - y1) * (x1 - x2));
    double y0 = (y3 * y3 - y1 * y1 - 2 * x0 * (x3 - x1) - x1 * x1 + x3 * x3) / (2 * (y3 - y1));
    double r = sqrt((y0 - y2) * (y0 - y2) + (x0 - x2) * (x0 - x2));

    circle_center.setX(x0);
    circle_center.setY(y0);
    circle_radius = r;

    isShowCircle = true;
    isShowBeforeFlip = true;

    update();
}

void MainWindow::slotAfterFlip()
{
    isShowAfterFlip = true;
    update();
}

void MainWindow::slotTest()
{
    qDebug() << "slotTest: " << QThread::currentThreadId;
//    delaunay_thread->sleep(5);
}

void MainWindow::slotNewPoint(VHandle vh)
{
    auto p = delaunay->mesh.point(vh);
    new_point = QPoint(p[0], p[1]);
    isShowNewPoint = true;
}


void MainWindow::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    QPen pen;

    if (isSelectMannually)
    {
        pen.setColor(Qt::red);
    }
    else
    {
        pen.setColor(Qt::gray);
    }

    // should be configurable
    pen.setWidth(6);
    pen.setCapStyle(Qt::RoundCap);
    painter.setPen(pen);
    for(auto p : points)
        painter.drawPoint(p);


//    pen.setColor(Qt::green);
//    pen.setWidth(4);
//    pen.setCapStyle(Qt::RoundCap);
//    painter.setPen(pen);

//    QVector<QPoint> vp;
//    vp << QPoint(-100000,-100000);
//    vp << QPoint(310,214);
//    vp << QPoint(0,100000);
////    QBrush brush(Qt::green);
////    painter.setBrush(brush);
//    painter.drawPolygon(&vp[0], 3);


    if (isShowNewPoint)
    {
        pen.setColor(Qt::red);
        painter.setPen(pen);
        painter.drawPoint(new_point);
    }


    if (isShowBeforeFlip)
    {
        // two triangles
        pen.setWidth(2);
        pen.setColor(QColor(151, 193, 62));
        painter.setPen(pen);
        painter.drawPolygon(&flipping_triangles[0], 4);

        // flipping edge
        pen.setColor(QColor(224, 31, 69));
        if(isShowAfterFlip)
        {
            painter.setPen(pen);
            painter.drawLine(flipping_triangles[0], flipping_triangles[2]);
        }
        else
        {
            pen.setStyle(Qt::DotLine);
            painter.setPen(pen);
            painter.drawLine(flipping_triangles[1], flipping_triangles[3]);
            pen.setStyle(Qt::SolidLine);
        }

        // in circle point
        pen.setColor(Qt::red);
        pen.setWidth(8);
        painter.setPen(pen);
        painter.drawPoint(in_circle_point);
    }

    if (isShowCircle)
    {
        pen.setWidth(2);
        pen.setColor(QColor(128, 128, 255));
        painter.setPen(pen);
        painter.drawEllipse(circle_center, circle_radius, circle_radius);
    }

    if (isShowSplitTriangle)
    {
        pen.setColor(Qt::green);
        pen.setWidth(4);
        painter.setPen(pen);
//        QBrush brush(Qt::green);
//        painter.setBrush(brush);
        if (!splitting_triangle.empty())
        {
            painter.drawPolygon(&splitting_triangle[0], 3);
        }
    }

    if (isTrianglated)
    {
        pen.setColor(Qt::blue);
        pen.setWidth(4);
        pen.setCapStyle(Qt::RoundCap);
        painter.setPen(pen);
        if (!triangles.empty())
        {
            for(int i = 0; i < triangles.size() / 3; i++)
            {
                painter.drawPolygon(&triangles[i * 3], 3);
            }
        }
    }
}

void MainWindow::mousePressEvent(QMouseEvent * event)
{
    if(isSelectMannually)
    {
        QString pos = QString("%1, %2").arg(event->pos().x()).arg(event->pos().y());
        QToolTip::showText(event->globalPos(), pos, this);
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent * event)
{
    if(isSelectMannually)
    {
        QString pos = QString("%1, %2").arg(event->pos().x()).arg(event->pos().y());
        QToolTip::showText(event->globalPos(), pos, this);
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent * event)
{
    if(isSelectMannually)
    {
        if (event->pos().y() < 100)
        {
            return;
        }
        points.append(event->pos());
        update();
    }
}

void MainWindow::on_actionPerform_triggered()
{
    if (points.size() < 3)
    {
        return;
    }

    if (!ui->action2D_Viewer->isChecked())
        this->hide();

    triangles.clear();

    PointVec mesh_points;
    for(auto& p : points)
    {
        mesh_points.push_back(Point(p.x(), p.y(), 0));
    }
    delaunay->perform(mesh_points);

    // display 2d result
    for (auto& fh : delaunay->mesh.faces())
    {
        for(auto& vh : delaunay->mesh.fv_range(fh))
        {
            Point p = delaunay->mesh.point(vh);
            triangles.push_back(QPoint(p[0], p[1]));
        }
    }
    isTrianglated = true;
    update();

    // display 3d result
    if (ui->action3D_Viewer->isChecked())
    {
        if (viewer == nullptr)
        {
            DViewerWindow* dvwin = new DViewerWindow(delaunay, this->width(), this->height());
            dvwin->setWindowTitle("Delaunay Triangulation Viewer");
            dvwin->show();
        }
    }
}

void MainWindow::on_actionClear_triggered()
{
    points.clear();
    triangles.clear();
    isTrianglated = false;
    update();
}

void MainWindow::on_actionSelectManually_triggered()
{
    isSelectMannually = !isSelectMannually;
}

void MainWindow::on_actionRandomGeneration_triggered()
{
    on_actionClear_triggered();
    isSelectMannually = false;

    RandomPointsDialog * dialog = new RandomPointsDialog(this);
    dialog->exec();

    QTime t;
    t.start();

    std::random_device rd;
    std::mt19937 gen;
    gen.seed(rd());

    for(int i = 0; i < dialog->getPointsNumber(); i++)
    {
        std::uniform_int_distribution<int> randx(1, this->width() - 1);
        std::uniform_int_distribution<int> randy(150, this->height() - 1); // 150 in order to avoid paint on toolbar
        points.append(QPoint(randx(gen), randy(gen)));
    }

    qDebug()<<"random time: "<<t.elapsed() / 1000.0;
    update();
}

void MainWindow::on_actionStepByStep_triggered()
{
    isSelectMannually = false;

    if (points.size() < 3)
    {
        return;
    }

//    if (!ui->action2D_Viewer->isChecked())
//        this->hide();

    triangles.clear();

    delaunay->setDemoMode();
    PointVec mesh_points;
    for(auto& p : points)
    {
        mesh_points.push_back(Point(p.x(), p.y(), 0));
    }
    delaunay->perform(mesh_points);

    // display 2d result
    for (auto& fh : delaunay->mesh.faces())
    {
        for(auto& vh : delaunay->mesh.fv_range(fh))
        {
            Point p = delaunay->mesh.point(vh);
            triangles.push_back(QPoint(p[0], p[1]));
        }
    }
    isTrianglated = true;
    update();

//    if (points.size() < 3)
//    {
//        return;
//    }

//    if (!ui->action2D_Viewer->isChecked())
//        this->hide();

//    auto delaunay = std::make_shared<Delaunay>();

//    // display 3d result
//    if (ui->action3D_Viewer->isChecked())
//    {
//        if (viewer == nullptr)
//        {
//            DViewerWindow* dvwin = new DViewerWindow(delaunay, this->width(), this->height());
//            dvwin->setWindowTitle("Delaunay Triangulation Viewer");
//            dvwin->show();
//        }
//    }

//    triangles.clear();

//    PointVec mesh_points;
//    for(auto& p : points)
//    {
//        mesh_points.push_back(Point(p.x(), p.y(), 0));
//    }
//    delaunay->perform(mesh_points);

//    // display 2d result
//    for (auto& fh : delaunay->mesh.faces())
//    {
//        for(auto& vh : delaunay->mesh.fv_range(fh))
//        {
//            Point p = delaunay->mesh.point(vh);
//            triangles.push_back(QPoint(p[0], p[1]));
//        }
//    }
//    isTrianglated = true;
//    update();
}

void MainWindow::on_actionPerformStep_triggered()
{
//    if (points.size() < 3)
//    {
//        return;
//    }

//    triangles.clear();

//    if (isFirstTime)
//    {
//        PointVec mesh_points;
//        for(auto& p : points)
//        {
//            mesh_points.push_back(Point(p.x(), p.y(), 0));
//        }

//        delaunay->init(mesh_points);

//        for (int i = 0; i < 3; i++)
//        {
//            delaunay->performStepByStep();
//        }

//        isFirstTime = false;
//    }
//    else
//    {
//        delaunay->performStepByStep();
//    }

//    // display 2d result
//    for (auto& fh : delaunay->mesh.faces())
//    {
//        bool isInfinite = false;
//        for(auto& vh : delaunay->mesh.fv_range(fh))
//        {
//            Point p = delaunay->mesh.point(vh);
//            Point a(-INF, -INF, 0);
//            Point b(INF, -INF, 0);
//            Point c(0, INF, 0);
//            if (p == a || p == b || p == c)
//            {
//                isInfinite = true;
//                break;
//            }
//        }
//        if (!isInfinite)
//        {
//            for(auto& vh : delaunay->mesh.fv_range(fh))
//            {
//                Point p = delaunay->mesh.point(vh);
//                triangles.push_back(QPoint(p[0], p[1]));
//            }
//        }
//    }
//    isTrianglated = true;
//    update();
//    current_point_num++;
}
