#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "delaunay.h"
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

    void on_actionOpen_3D_Viewer_triggered();

    void on_actionStep_by_Step_triggered();

private:
    Ui::MainWindow *ui;
    QVector<QPoint> points;
    QVector<QPoint> triangles;
    bool isDone;
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent * event);
    void mouseMoveEvent(QMouseEvent * event);
    void mouseReleaseEvent(QMouseEvent * event);
};

#endif // MAINWINDOW_H
