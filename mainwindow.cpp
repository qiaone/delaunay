#include "mainwindow.h"
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
//#include <QtAlgorithms>
//#include <qmath.h>
//#include <QSet>
//#include <limits>

const float INF = 1.0e5f;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    isTrianglated(false),
    isMouseDraw(true),
    isShowSplitTriangle(false),
    current_point_num(0),
    viewer(nullptr)
{
    ui->setupUi(this);
    delaunay = std::make_shared<Delaunay>();
//    delaunay = std::make_shared<DelaunayStepByStep>(true);
//    QObject::connect((QObject*)delaunay.get(), SIGNAL(signalBeforeSplit(FHandle)),
//                     this, SLOT(slotBeforeSplit(FHandle)));
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
    for(auto& vh : delaunay->mesh.fv_range(fh))
    {
        Point p = delaunay->mesh.point(vh);
        Point a(-INF, -INF, 0);
        Point b(INF, -INF, 0);
        Point c(0, INF, 0);
        if (p == a || p == b || p == c)
        {
            isInfinite = true;
            break;
        }
    }
    if (!isInfinite)
    {
        for(auto& vh : delaunay->mesh.fv_range(fh))
        {
            Point p = delaunay->mesh.point(vh);
            splitting_triangle.append(QPoint(p[0], p[1]));
        }
        isShowSplitTriangle = true;
    }
    
    qDebug() << "slotSplittingTriagnle";
    update();
}

void MainWindow::slotAfterSplit(FHandle fh)
{

}

void MainWindow::slotBeforeFlip()
{

}

void MainWindow::slotAfterFlip()
{

}

void MainWindow::slotTest()
{
    qDebug() << "slotTest: " << QThread::currentThreadId;
//    delaunay_thread->sleep(5);
}

void MainWindow::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    QPen pen;

    pen.setColor(Qt::red);

    // should be configurable
    pen.setWidth(6);
    pen.setCapStyle(Qt::RoundCap);
    painter.setPen(pen);
    for(auto p : points)
        painter.drawPoint(p);

    if (isShowSplitTriangle)
    {
        pen.setColor(Qt::green);
        pen.setWidth(4);
        pen.setCapStyle(Qt::RoundCap);
        painter.setPen(pen);
        QBrush brush(Qt::green);
        painter.setBrush(brush);
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
    if(isMouseDraw)
    {
        QString pos = QString("%1, %2").arg(event->pos().x()).arg(event->pos().y());
        QToolTip::showText(event->globalPos(), pos, this);
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent * event)
{
    if(isMouseDraw)
    {
        QString pos = QString("%1, %2").arg(event->pos().x()).arg(event->pos().y());
        QToolTip::showText(event->globalPos(), pos, this);
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent * event)
{
    if(isMouseDraw)
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

    auto delaunay = std::make_shared<Delaunay>();
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
    isMouseDraw = !isMouseDraw;
}

void MainWindow::on_actionRandomGeneration_triggered()
{
    on_actionClear_triggered();
    isMouseDraw = false;

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
