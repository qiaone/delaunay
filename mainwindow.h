#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "delaunay.h"
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

private slots:
    void on_actionPerform_triggered();
    void on_actionClear_triggered();

    void on_actionSelectManually_triggered();

    void on_actionRandomGeneration_triggered();

    void on_actionStepByStep_triggered();

private:
    Ui::MainWindow *ui;
    QVector<QPoint> points;
    QVector<QPoint> triangles;
    bool isTrianglated;
    bool isMouseDraw;
    DViewer* viewer;

    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent * event);
    void mouseMoveEvent(QMouseEvent * event);
    void mouseReleaseEvent(QMouseEvent * event);

};

#endif // MAINWINDOW_H
