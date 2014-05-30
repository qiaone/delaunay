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


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    isTrianglated(false),
    isMouseDraw(true),
    viewer(nullptr)
{
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

    if (isTrianglated)
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
    isTrianglated = true;
    update();

    if (ui->action3D_Viewer->isChecked())
    {
        if (viewer == nullptr)
        {
            DViewerWindow* dvwin = new DViewerWindow(std::move(delaunay), this->width(), this->height());
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
