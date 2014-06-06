#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "delaunay.h"
//#include "delaunaystepbystep.h"
#include "dviewer.h"
#include <memory>
#include <QThread>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void slotBeforeSplit(FHandle fh);
    void slotAfterSplit(FHandle fh);
    void slotBeforeFlip(HHandle hh, VHandle vh, VHandle vh_oppo);
    void slotAfterFlip();
    void slotTest();
    void slotNewPoint(VHandle vh);

private slots:
    void on_actionPerform_triggered();
    void on_actionClear_triggered();
    void on_actionSelectManually_triggered();
    void on_actionRandomGeneration_triggered();
    void on_actionStepByStep_triggered();
    void on_actionPerformStep_triggered();

private:
    Ui::MainWindow *ui;
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent * event);
    void mouseMoveEvent(QMouseEvent * event);
    void mouseReleaseEvent(QMouseEvent * event);

    DViewer* viewer;
    std::shared_ptr<Delaunay> delaunay;

    bool isFirstTime;

// painter related
private:
    QVector<QPoint> points;
    QVector<QPoint> splitting_triangle;
    QVector<QPoint> triangles;
    QPoint new_point;
    QPointF circle_center;
    double circle_radius;
    QVector<QPoint> flipping_triangles;

    bool isTrianglated;
    bool isSelectMannually;
    bool isShowNewPoint;
    bool isShowSplitTriangle;
    bool isShowCircle;
    bool isShowFlippingTriangles;

};

#endif // MAINWINDOW_H
