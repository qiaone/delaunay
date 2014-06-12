#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "delaunay.h"
#include "delaunayincremental.h"
#include <QSet>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionPerform_triggered();
    void on_actionClear_triggered();
    void on_actionRandomGeneration_triggered();
    void on_actionShow_Axis_triggered();
    void on_actionShow_Paraboloid_triggered();
    void on_actionGithub_triggered();
    void on_actionTake_Snapshot_triggered();
    void on_actionCircle_triggered();
    void on_actionEllipse_triggered();
    void on_actionReal_Time_toggled(bool isRealTime_);

public slots:
    void slotRefreshGui();

private:
    Ui::MainWindow *ui;
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent * event);
    void mouseMoveEvent(QMouseEvent * event);
    void mouseReleaseEvent(QMouseEvent * event);
    void resizeEvent(QResizeEvent *);
    void wheelEvent(QWheelEvent * event);
    void showResult2D();
    void showFlips();
    void initFlipDemoParams(std::array<Point, 4>& flip);
    void demoRealTime(QPoint& p);

    Delaunay* delaunay;
    DelaunayIncremental* delaunay_inc;

    bool isPerformClickable;

    // painter related
private:
    QPainter* painter;
    QPen* pen;
    QVector<QPoint> points;
    QSet<QString> points_set;
    QVector<QPoint> triangles;
    QPointF in_circle_point;
    QPointF circle_center;
    double circle_radius;
    QVector<QPoint> flip_rec_4_points;
    QVector<QPoint> flipped_edges;

    bool isTrianglated;
    bool isSelectMannually;
    bool isShowSplitTriangle;
    bool isShowCircle;
    bool isShowFlip;
    bool isShowFlippedEdge;

    int delay_mseconds;
};

#endif // MAINWINDOW_H
