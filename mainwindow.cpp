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
#include <cmath>

const float INF = 1.0e5f;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    isTrianglated(false),
    isSelectMannually(true),
    isShowCircle(false),
    isShowBeforeFlip(false),
    isShowAfterFlip(false),
    viewer(nullptr)
{
    ui->setupUi(this);
    delaunay = new Delaunay;
    delaunay_inc = new DelaunayIncremental;
}

MainWindow::~MainWindow()
{
    //delete delaunay;
    delete ui;
}

void MainWindow::showCircle2D()
{
    double x1 = flipping_triangles[0].x();
    double y1 = flipping_triangles[0].y();
    double x2 = flipping_triangles[1].x();
    double y2 = flipping_triangles[1].y();
    double x3 = flipping_triangles[3].x();
    double y3 = flipping_triangles[3].y();

    in_circle_point.setX(flipping_triangles[2].x());
    in_circle_point.setY(flipping_triangles[2].y());

    double x0 = ((y3 - y1) * (y2 * y2 - y1 * y1) + (y3 - y1) * (x2 * x2 - x1 * x1) -
                 (y1 - y2) * (y1 * y1 - y3 * y3) - (y1 - y2) * (x1 * x1 - x3 * x3)) /
                (2 * (y1 - y2) * (x3 - x1) - 2 * (y3 - y1) * (x1 - x2));
    double y0 = (y3 * y3 - y1 * y1 - 2 * x0 * (x3 - x1) - x1 * x1 + x3 * x3) / (2 * (y3 - y1));
    double r = sqrt((y0 - y2) * (y0 - y2) + (x0 - x2) * (x0 - x2));

    circle_center.setX(x0);
    circle_center.setY(y0);
    circle_radius = r;

    isShowCircle = true;
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

    if (isTrianglated)
    {
        pen.setColor(QColor(0, 196, 0));
        pen.setWidth(2);
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

    if (isShowBeforeFlip)
    {
        // two triangles
        pen.setWidth(4);
        pen.setColor(QColor(187, 0, 187));
        //pen.setBrush(QBrush::);
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
        isSelectMannually = false;
        if (event->pos().y() < 100)
        {
            return;
        }
        points.append(event->pos());
        update();

        auto p = event->pos();
        delaunay_inc->performIncremental(Point(p.x(), p.y(), 0));
        showFlips2D();
        showResult2D();
        isSelectMannually = true;
    }
}

void MainWindow::showFlips2D( )
{
    // show all flips during a new point added
    for (auto& flip : delaunay_inc->flip_records)
    {
        flipping_triangles.clear();
        flipping_triangles
            << QPoint(flip[0][0],flip[0][1])
            << QPoint(flip[1][0],flip[1][1])
            << QPoint(flip[2][0],flip[2][1])
            << QPoint(flip[3][0],flip[3][1]);
        showCircle2D();
        isShowCircle = true;
        isShowBeforeFlip = true;
        update();

        QTime t;
        t.start();
        while(t.elapsed()<1000)
            QCoreApplication::processEvents();

        isShowAfterFlip = true;
        update();

        t.start();
        while(t.elapsed()<1000)
            QCoreApplication::processEvents();

        isShowAfterFlip = false;
    }
    isShowCircle = false;
    isShowBeforeFlip = false;
    update();
}

void MainWindow::showResult2D()
{
    triangles.clear();
    // display 2d result
    for (auto& fh : delaunay_inc->mesh.faces())
    {
         // do not show triangle with inf point
         bool hasInfinitePoint = false;
         for(auto& vh : delaunay_inc->mesh.fv_range(fh))
         {
             Point p = delaunay_inc->mesh.point(vh);
             Point a(-INF, -INF, 0);
             Point b(INF, -INF, 0);
             Point c(0, INF, 0);
             if (p == a || p == b || p == c)
             {
                 hasInfinitePoint = true;
                 break;
             }
         }
 
         if (!hasInfinitePoint)
        {
            for(auto& vh : delaunay_inc->mesh.fv_range(fh))
            {
                Point p = delaunay_inc->mesh.point(vh);
                triangles.push_back(QPoint(p[0], p[1]));
            }
        }
    }

    isTrianglated = true;
    update();
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

void MainWindow::on_actionPerform_triggered()
{
    if (points.size() < 3)
    {
        return;
    }

    isSelectMannually = false;
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
    if(ui->action3D_Viewer->isChecked())
    {
        if (viewer == nullptr)
        {
            DViewerWindow* dvwin = new DViewerWindow(delaunay, this->width(), this->height());
            dvwin->setWindowTitle("Delaunay Triangulation Viewer");
            dvwin->show();
        }
    }
}

void MainWindow::on_actionStepByStep_triggered()
{
    if (points.size() < 3)
    {
        return;
    }

    isSelectMannually = false;
    triangles.clear();

    delaunay->setDemoMode();
    PointVec mesh_points;
    for(auto& p : points)
    {
        mesh_points.push_back(Point(p.x(), p.y(), 0));
    }

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
    if(ui->action3D_Viewer->isChecked())
    {
        if (viewer == nullptr)
        {
            DViewerWindow* dvwin = new DViewerWindow(delaunay, this->width(), this->height());
            dvwin->setWindowTitle("Delaunay Triangulation Viewer");
            dvwin->show();
        }
    }

    delaunay->perform(mesh_points);
}

void MainWindow::on_actionPerformIncremental_triggered()
{

}
