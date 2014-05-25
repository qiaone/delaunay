#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dviewer.h"
#include "delaunay.h"

#include <memory>
#include <QPainter>
#include <QPen>
#include <QToolTip>
#include <QMouseEvent>
#include <QDebug>
//#include <QTime>
//#include <QtAlgorithms>
//#include <qmath.h>
//#include <QSet>
//#include <limits>
//#include <random>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    isDone = false;
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
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

    if (isDone)
    {
        pen.setColor(Qt::blue);
        pen.setWidth(4);
        pen.setCapStyle(Qt::RoundCap);
        painter.setPen(pen);
        for(int i = 0; i < triangles.size() / 3; i++)
        {
            painter.drawPolygon(&triangles[i * 3], 3);
        }
    }
}

void MainWindow::mousePressEvent(QMouseEvent * event)
{
//    if(isMouseDraw)
//    {
        QString pos = QString("%1, %2").arg(event->pos().x()).arg(event->pos().y());
        QToolTip::showText(event->globalPos(), pos, this);
//    }
}

void MainWindow::mouseMoveEvent(QMouseEvent * event)
{
//    if(isMouseDraw)
//    {
        QString pos = QString("%1, %2").arg(event->pos().x()).arg(event->pos().y());
        QToolTip::showText(event->globalPos(), pos, this);
//    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent * event)
{
    if (event->pos().y() < 100)
    {
        return;
    }
//    if(isMouseDraw)
//    {
        points.append(event->pos());
        update();
//    }
}

void MainWindow::on_actionPerform_triggered()
{
    if (points.size() < 3)
    {
        return;
    }
    std::unique_ptr<Delaunay> delaunay(new Delaunay);
    PointVec mesh_points;
    for(auto& p : points)
    {
        mesh_points.push_back(Point(p.x(), p.y(), 0));
    }
    delaunay->perform(mesh_points);

    for (auto& fh : delaunay->mesh.faces())
    {
        for(auto& vh : delaunay->mesh.fv_range(fh))
        {
            Point p = delaunay->mesh.point(vh);
            triangles.push_back(QPoint(p[0], p[1]));
        }
    }
    isDone = true;
    update();
    DViewer::Viewer * viewer = new DViewer::Viewer(std::move(delaunay));
    viewer->setWindowTitle("Delaunay Triangulation Viewer");
    viewer->show();
    //viewer.exec();
}

void MainWindow::on_actionOpen_3D_Viewer_triggered()
{
    on_actionPerform_triggered();
}

void MainWindow::on_actionStep_by_Step_triggered()
{
    on_actionPerform_triggered();
}
