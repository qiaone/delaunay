#include "dviewerwindow.h"
#include "ui_dviewerwindow.h"
#include <QDebug>

//DViewerWindow::DViewerWindow(std::unique_ptr<Delaunay> delaunay, int mainwindow_width, int mainwindow_height, QWidget *parent) :
//    QWidget(parent),
//    ui(new Ui::DViewerWindow)
//{
//    ui->setupUi(this);
//    ui->viewer->setParam(std::move(delaunay), mainwindow_width, mainwindow_height);
//}


DViewerWindow::DViewerWindow(Delaunay* delaunay, int mainwindow_width, int mainwindow_height, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DViewerWindow)
{
    ui->setupUi(this);
    ui->viewer->setParam(delaunay, mainwindow_width, mainwindow_height);
    QObject::connect(delaunay, SIGNAL(signalBeforeFlip(HHandle,VHandle,VHandle)),
                     ui->viewer, SLOT(slotBeforeFlip(HHandle,VHandle,VHandle)));
    QObject::connect(delaunay, SIGNAL(signalAfterFlip()),
                     ui->viewer, SLOT(slotAfterFlip()));
}

DViewerWindow::~DViewerWindow()
{
    delete ui;
}

void DViewerWindow::on_pushButtonNext_clicked()
{

}
