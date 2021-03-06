﻿#include "mainwindow.h"
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
#include <QDesktopServices>
#include <QFileDialog>
#include <cmath>

void MainWindow::closeEvent(QCloseEvent * e)
{
    on_actionClear_triggered();
    QMainWindow::closeEvent(e);
}

inline void MainWindow::delay(float ms)
{
    QTime t;
    t.start();
    while(t.elapsed() < ms && !isStopDemoRealTime)
        QCoreApplication::processEvents();
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    isTrianglated(false),
    isSelectMannually(true),
    isShowCircle(false),
    isShowFlip(false),
    isShowFlippedEdge(false),
    isShowSplitTriangle(false),
    isRandomized(false),
    isDemoRealTimeFinished(true),
    isStopDemoRealTime(false),
    isContinuousDrawing(false),
    delay_mseconds(2000)
{
    ui->setupUi(this);
    delaunay = new Delaunay;
    delaunay_inc = new DelaunayIncremental;
    ui->viewer->setParam(delaunay_inc, this->width(), this->height());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::resizeEvent(QResizeEvent *)
{
    ui->viewer->setParam(delaunay_inc, this->width(), this->height());
}

void MainWindow::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    QPen pen;

    pen.setColor(Qt::black);
    painter.setPen(pen);
    painter.drawLine(QPoint(this->width() / 2 - 2, 96), QPoint(this->width() / 2, this->height()));

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

    if (isShowFlip)
    {
        // two triangles
        pen.setWidth(3);
        pen.setColor(QColor(255, 0, 128));
        //pen.setBrush(QBrush::);
        painter.setPen(pen);
        painter.drawPolygon(&flip_rec_4_points[0], 4);

        // flipping edge
        if(isShowFlippedEdge)
        {
            pen.setStyle(Qt::DotLine);
            pen.setColor(QColor(0, 128, 0));
            pen.setWidth(6);
            painter.setPen(pen);
            painter.drawLine(flip_rec_4_points[0], flip_rec_4_points[2]);
            pen.setStyle(Qt::SolidLine);
        }
        else
        {
            pen.setStyle(Qt::DotLine);
            pen.setColor(Qt::red);
            pen.setWidth(6);
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
    if (isContinuousDrawing)
    {
        mouseReleaseEvent(event);
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent * event)
{
    if(isSelectMannually)
    {
        QString pos = QString("%1, %2").arg(event->pos().x()).arg(event->pos().y());
        QToolTip::showText(event->globalPos(), pos, this);
    }
    if (isContinuousDrawing)
    {
        mouseReleaseEvent(event);
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
    QString pos = QString("%1,%2").arg(event->pos().x()).arg(event->pos().y());
    if(isSelectMannually)
    {
        if (event->pos().x() > this->width() / 2 ||
                event->pos().y() < 100 ||
                points_set.contains(pos))
        {
            return;
        }

        points.append(event->pos());
        points_set.insert(pos);
        update();

        if (ui->actionReal_Time->isChecked())
        {
            isDemoRealTimeFinished = false;
            demoRealTime(event->pos());
            isDemoRealTimeFinished = true;
            if (isStopDemoRealTime)
            {
                on_actionClear_triggered();
            }
        }
    }
}

void MainWindow::demoRealTime(QPoint& p)
{
    //ui->actionClear->setDisabled(true);
    isSelectMannually = false;

    delaunay_inc->performIncremental(Point(p.x(), p.y(), 0));

    isShowSplitTriangle = true;
    showFlips();
    ui->viewer->showMesh2D();
    ui->viewer->showMesh3D();
    if (!ui->actionReal_Time->isChecked())
    {
        delay(delay_mseconds);
    }
    showResult2D();
    isShowSplitTriangle = false;

    isSelectMannually = true;
    //ui->actionClear->setEnabled(true);
}

void MainWindow::initFlipDemoParams(std::array<Point, 4>& flip)
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

void MainWindow::slotRefreshGui()
{
    QCoreApplication::processEvents();
}

void MainWindow::showFlips()
{
    flipped_edges.clear();
    // show all flips during a new point added

    isShowFlip = true;
    for (auto& flip : delaunay_inc->flip_records)
    {
        // 2d: show two triangles which will be flipped with in-circle test
        initFlipDemoParams(flip);
        ui->viewer->initFlipDemoParams(flip);
        isShowCircle = true;
        update();
        // 3d
        ui->viewer->clearMesh3D();
        ui->viewer->showBeforeFlip3D();

        delay(delay_mseconds);

        // 2d: show flipped edge
        isShowFlippedEdge = true;
        update();
        ui->viewer->showAfterFlip3D();

        delay(delay_mseconds);

        // 3d
        // make flipped edges persistent during flips
        flipped_edges << flip_rec_4_points[0] << flip_rec_4_points[2];

        isShowCircle = false;
        isShowFlippedEdge = false;
        update();
    }
    isShowFlip = false;
    ui->viewer->clearAfterFlip3D();
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
    if (!isDemoRealTimeFinished)
    {
        isStopDemoRealTime = true;
        return;
    }
    if (isStopDemoRealTime)
    {
        isStopDemoRealTime = false;
    }

    points.clear();
    points_set.clear();
    triangles.clear();
    flipped_edges.clear();
    flip_rec_4_points.clear();

    isTrianglated = false;
    isSelectMannually = true;
    isShowCircle = false;
    isShowFlip = false;
    isShowFlippedEdge = false;
    //delay_mseconds = 700;

    delaunay_inc->reset();
    ui->viewer->setParam(delaunay_inc, this->width(), this->height());
    ui->viewer->clearAfterFlip3D();
//    ui->actionReal_Time->setEnabled(true);
//    ui->actionReal_Time->setChecked(true);

    update();
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
        std::uniform_int_distribution<int> randx(1, this->width() / 2 - 2);
        std::uniform_int_distribution<int> randy(100, this->height() - 2); // 100 in order to avoid paint on toolbar
        points.append(QPoint(randx(gen), randy(gen)));
    }

    qDebug()<<"random time: "<<t.elapsed() / 1000.0;
    update();
    isRandomized = true;
}

void MainWindow::on_actionPerform_triggered()
{
    if (ui->actionReal_Time->isChecked() && !isRandomized)
    {
        QToolTip::showText(QPoint(this->x() + 20, this->y() + 120), "Please click Random Generation or disable Real Time Mode first", this);
        return;
    }

    isRandomized = false;

    if (points.size() < 3)
    {
        return;
    }

    if (delay_mseconds != 0)
    {
        isDemoRealTimeFinished = false;
        for(auto& p : points)
        {
            demoRealTime(p);
        }
        isDemoRealTimeFinished = true;
        if (isStopDemoRealTime)
        {
            on_actionClear_triggered();
        }
        return;
    }

    isSelectMannually = false;
    triangles.clear();

    QObject::connect(delaunay, SIGNAL(signalRefreshGui()), this, SLOT(slotRefreshGui()));

    PointVec mesh_points;
    for(auto& p : points)
    {
        mesh_points.push_back(Point(p.x(), p.y(), 0));
    }

    QTime t;
    t.start();

    delaunay->perform(mesh_points);

    qDebug()<<"delaunay time: "<<t.elapsed() / 1000.0;

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
    ui->viewer->setParam(delaunay, this->width(), this->height());
    ui->viewer->showMesh2D();
    ui->viewer->showMesh3D();
}


void MainWindow::on_actionShow_Axis_triggered()
{
    ui->viewer->toggleAxisIsDrawn();
}

void MainWindow::on_actionShow_Paraboloid_triggered()
{
    ui->viewer->isKillMesh2D = !ui->viewer->isKillMesh2D;
    ui->viewer->toggleShowParaboloid();
}

void MainWindow::on_actionGithub_triggered()
{
    QDesktopServices::openUrl(QUrl("https://github.com/qiaone/delaunay", QUrl::TolerantMode));
}

void MainWindow::on_actionTake_Snapshot_triggered()
{
    //ui->viewer->openSnapshotFormatDialog();
    ui->viewer->saveSnapshot(false);
}

void MainWindow::on_actionCircle_triggered()
{
    delaunay->funType = CIRCLE;
}

void MainWindow::on_actionEllipse_triggered()
{
    delaunay->funType = ELLIPSE;
}

void MainWindow::on_actionNorm2_triggered()
{
	delaunay->funType = NORM2;
}

void MainWindow::on_actionReal_Time_toggled(bool isRealTime_)
{
    isRandomized = !isRealTime_;
    if (points.size() > 0)
    {
        on_actionClear_triggered();
    }
}

void MainWindow::on_actionOpen_triggered()
{
    on_actionReal_Time_toggled(false);
    QFileDialog * dialog = new QFileDialog(this);
    dialog->setNameFilter(tr("data files (*.txt)"));
    dialog->setDirectory(QDir::currentPath());
    dialog->exec();
    QStringList filename_list = dialog->selectedFiles();
    QString datafilename = filename_list.first();

    QFile datafile(datafilename);
    if (!datafile.open(QIODevice::ReadOnly))
        return;

    QTextStream in_stream(&datafile);

    QVector<QPoint> in_points;

    while (!in_stream.atEnd())
    {
        float x, y;
        in_stream >> x >> y;
        in_points << QPoint(x, y);
        //qDebug() << x << ", " << y;
    }

    if (in_points.last().x() == 0 && in_points.last().y() == 0)
    {
        in_points.pop_back();
    }

    points = in_points;
    update();
    //on_actionPerform_triggered();
}

void MainWindow::on_actionSaveTriangles_triggered()
{
    QFileDialog * dialog = new QFileDialog(this);
    dialog->setNameFilter(tr("data files (*.txt)"));
    dialog->setDirectory(QDir::currentPath());
    dialog->exec();
    QStringList filename_list = dialog->selectedFiles();
    QString datafilename = filename_list.first();

    QFile datafile(datafilename);
    if (!datafile.open(QIODevice::WriteOnly))
        return;

    QTextStream out_stream(&datafile);

    for (int i = 0; i < triangles.size(); i += 3)
    {
        out_stream << triangles[i].x() << " " << triangles[i].y() << " ";
        out_stream << triangles[i + 1].x() << " " << triangles[i + 1].y() << " ";
        out_stream << triangles[i + 2].x() << " " << triangles[i + 2].y() << endl;
    }
}

void MainWindow::on_actionSave_Points_triggered()
{
    QFileDialog * dialog = new QFileDialog(this);
    dialog->setNameFilter(tr("data files (*.txt)"));
    dialog->setDirectory(QDir::currentPath());
    dialog->exec();
    QStringList filename_list = dialog->selectedFiles();
    QString datafilename = filename_list.first();

    QFile datafile(datafilename);
    if (!datafile.open(QIODevice::WriteOnly))
        return;

    QTextStream out_stream(&datafile);

    for(auto& p : points)
    {
        out_stream << p.x() << " " << p.y() << endl;
    }
}

void MainWindow::on_actionContinuous_Drawing_toggled(bool isContinuous)
{
    isContinuousDrawing = isContinuous;
}
