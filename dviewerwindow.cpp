#include "dviewerwindow.h"
#include "ui_dviewerwindow.h"

DViewerWindow::DViewerWindow(std::unique_ptr<Delaunay> delaunay, int mainwindow_width, int mainwindow_height, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DViewerWindow)
{
    ui->setupUi(this);
    ui->viewer->setParam(std::move(delaunay), mainwindow_width, mainwindow_height);
}

DViewerWindow::~DViewerWindow()
{
    delete ui;
}
