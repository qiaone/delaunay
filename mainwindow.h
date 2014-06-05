#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "delaunay.h"
//#include "delaunaystepbystep.h"
#include "dviewer.h"
#include <memory>

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
    void slotBeforeFlip();
    void slotAfterFlip();


private slots:
    void on_actionPerform_triggered();
    void on_actionClear_triggered();
    void on_actionSelectManually_triggered();
    void on_actionRandomGeneration_triggered();
    void on_actionStepByStep_triggered();
    void on_actionPerformStep_triggered();

private:
    Ui::MainWindow *ui;
    QVector<QPoint> points;
    QVector<QPoint> splitting_triangle;
    QVector<QPoint> triangles;
    bool isTrianglated;
    bool isMouseDraw;
    bool isShowSplitTriangle;

    DViewer* viewer;
    std::shared_ptr<Delaunay> delaunay;
//    std::shared_ptr<DelaunayStepByStep> delaunay_step;

    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent * event);
    void mouseMoveEvent(QMouseEvent * event);
    void mouseReleaseEvent(QMouseEvent * event);

    bool isFirstTime;
    int current_point_num;
};

#endif // MAINWINDOW_H
