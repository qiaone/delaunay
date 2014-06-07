#ifndef DVIEWERWINDOW_H
#define DVIEWERWINDOW_H

#include <QWidget>
#include <memory>
#include "delaunay.h"

namespace Ui {
class DViewerWindow;
}

class DViewerWindow : public QWidget
{
    Q_OBJECT

public:
    explicit DViewerWindow(Delaunay* delaunay, int mainwindow_width, int mainwindow_height, QWidget *parent = 0);
    //explicit DViewerWindow(std::unique_ptr<Delaunay> delaunay, int mainwindow_width, int mainwindow_height, QWidget *parent = 0);
    ~DViewerWindow();

signals:
    void test_send();

private slots:
    void on_pushButtonNext_clicked();

private:
    Ui::DViewerWindow *ui;
};

#endif // DVIEWERWINDOW_H
