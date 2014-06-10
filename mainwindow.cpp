#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "randompointsdialog.h"
#include <random>
#include <QPainter>
#include <QPen>
#include <QToolTip>
#include <QMouseEvent>
#include <QWheelEvent>
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
    isShowSplitTriangle(false),
    delay_mseconds(700)
{
    ui->setupUi(this);
    delaunay = new Delaunay;
    delaunay_inc = new DelaunayIncremental;
    ui->viewer->setParam(delaunay_inc, this->width(), this->height());
}

MainWindow::~MainWindow()
{
    //delete delaunay;
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    QPen pen;


    // Points
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

    // Triangles
    if (isTrianglated)
    {
        pen.setColor(QColor(63, 72, 204));
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

        // flipped edges
        pen.setColor(QColor(0, 128, 0));
        painter.setPen(pen);
        for(int i = 0; i < flipped_edges.size(); i += 2)
        {
            painter.drawLine(flipped_edges[i], flipped_edges[i + 1]);
        }
    }

    // Split edges
    if (isShowSplitTriangle)
    {
        pen.setColor(QColor(63, 72, 204));
        painter.setPen(pen);
        Point newpoint = delaunay_inc->mesh.point(delaunay_inc->new_vh);
        for(auto& p : delaunay_inc->split_records)
        {
            painter.drawLine(QPoint(newpoint[0], newpoint[1]), QPoint(p[0], p[1]));
        }
    }

    if (isShowBeforeFlip)
    {
        // two triangles
        pen.setWidth(3);
        pen.setColor(QColor(255, 0, 128));
        //pen.setBrush(QBrush::);
        painter.setPen(pen);
        painter.drawPolygon(&flip_rec_4_points[0], 4);

        // flipping edge
        if(isShowAfterFlip)
        {
            pen.setColor(QColor(0, 128, 0));
            painter.setPen(pen);
            painter.drawLine(flip_rec_4_points[0], flip_rec_4_points[2]);
        }
        else
        {
            pen.setStyle(Qt::DotLine);
            painter.setPen(pen);
            painter.drawLine(flip_rec_4_points[1], flip_rec_4_points[3]);
            pen.setStyle(Qt::SolidLine);
        }

        // in circle point
        pen.setColor(Qt::black);
        pen.setWidth(8);
        painter.setPen(pen);
        painter.drawPoint(in_circle_point);
    }

    if (isShowCircle)
    {
        pen.setWidth(2);
        pen.setColor(QColor(255, 128, 39));
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

void MainWindow::wheelEvent(QWheelEvent *event)
{
    if (event->angleDelta().y() > 0)
    {
        delay_mseconds += 100;
    }
    else
    {
        delay_mseconds -= 100;
        if (delay_mseconds < 0)
        {
            delay_mseconds = 0;
        }
    }
    QString delay = QString("Delay between steps: %1 ms").arg(delay_mseconds);
    QToolTip::showText(event->globalPos(), delay, this);
}

void MainWindow::mouseReleaseEvent(QMouseEvent * event)
{
    if(isSelectMannually)
    {
        if (event->pos().x() > 600 || event->pos().y() > 600)
        {
            return;
        }

        isSelectMannually = false;
        points.append(event->pos());
        update();

        auto p = event->pos();
        delaunay_inc->performIncremental(Point(p.x(), p.y(), 0));
        isShowSplitTriangle = true;
        ui->viewer->showResult3D();
        showFlips();
        showResult2D();
        isSelectMannually = true;
        isShowSplitTriangle = false;
    }
}

void MainWindow::initFlip(std::array<Point, 4>& flip)
{
    flip_rec_4_points.clear();
    flip_rec_4_points
        << QPoint(flip[0][0],flip[0][1])
        << QPoint(flip[1][0],flip[1][1])
        << QPoint(flip[2][0],flip[2][1])
        << QPoint(flip[3][0],flip[3][1]);

    double x1 = flip_rec_4_points[0].x();
    double y1 = flip_rec_4_points[0].y();
    double x2 = flip_rec_4_points[1].x();
    double y2 = flip_rec_4_points[1].y();
    double x3 = flip_rec_4_points[3].x();
    double y3 = flip_rec_4_points[3].y();

    in_circle_point.setX(flip_rec_4_points[2].x());
    in_circle_point.setY(flip_rec_4_points[2].y());

    double x0 = ((y3 - y1) * (y2 * y2 - y1 * y1) + (y3 - y1) * (x2 * x2 - x1 * x1) -
                 (y1 - y2) * (y1 * y1 - y3 * y3) - (y1 - y2) * (x1 * x1 - x3 * x3)) /
                (2 * (y1 - y2) * (x3 - x1) - 2 * (y3 - y1) * (x1 - x2));
    double y0 = (y3 * y3 - y1 * y1 - 2 * x0 * (x3 - x1) - x1 * x1 + x3 * x3) / (2 * (y3 - y1));
    double r = sqrt((y0 - y2) * (y0 - y2) + (x0 - x2) * (x0 - x2));

    circle_center.setX(x0);
    circle_center.setY(y0);
    circle_radius = r;
}

inline void delay(float ms)
{
    QTime t;
    t.start();
    while(t.elapsed() < ms)
        QCoreApplication::processEvents();
}

void MainWindow::showFlips()
{
    flipped_edges.clear();
    // show all flips during a new point added

    isShowBeforeFlip = true;
    for (auto& flip : delaunay_inc->flip_records)
    {
        initFlip(flip);
        isShowCircle = true;
        update();

        delay(delay_mseconds);

        isShowAfterFlip = true;
        update();

        delay(delay_mseconds);

        flipped_edges << flip_rec_4_points[0] << flip_rec_4_points[2];
        isShowCircle = false;
        isShowAfterFlip = false;
        update();
    }
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
        if (!delaunay_inc->hasInfinitePoint(fh))
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
    flip_rec_4_points.clear();

    isTrianglated = false;
    isSelectMannually = true;
    isShowCircle = false;
    isShowBeforeFlip = false;
    isShowAfterFlip = false;
    delay_mseconds = 700;

    delaunay_inc->mesh.clear();

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

//    // display 3d result
//    if(ui->action3D_Viewer->isChecked())
//    {
//        if (viewer == nullptr)
//        {
//            DViewerWindow* dvwin = new DViewerWindow(delaunay, this->width(), this->height());
//            dvwin->setWindowTitle("Delaunay Triangulation Viewer");
//            dvwin->show();
//        }
//    }
}

