#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "delaunay.h"
#include "delaunayincremental.h"

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
    void on_actionSelectManually_triggered();
    void on_actionRandomGeneration_triggered();

private:
    Ui::MainWindow *ui;
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent * event);
    void mouseMoveEvent(QMouseEvent * event);
    void mouseReleaseEvent(QMouseEvent * event);
    void showResult2D();
    void showCircle2D();
    void showFlips2D();

    Delaunay* delaunay;
    DelaunayIncremental* delaunay_inc;

// painter related
private:
    QPainter* painter;
    QPen* pen;
    QVector<QPoint> points;
    QVector<QPoint> triangles;
    QPoint in_circle_point;
    QPointF circle_center;
    double circle_radius;
    QVector<QPoint> flipping_triangles;

    bool isTrianglated;
    bool isSelectMannually;
    bool isShowSplitTriangle;
    bool isShowCircle;
    bool isShowBeforeFlip;
    bool isShowAfterFlip;
};

#endif // MAINWINDOW_H
