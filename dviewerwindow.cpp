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


DViewerWindow::DViewerWindow(std::shared_ptr<Delaunay> delaunay, int mainwindow_width, int mainwindow_height, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DViewerWindow)
{
    ui->setupUi(this);
    ui->viewer->setParam(delaunay->mesh, mainwindow_width, mainwindow_height);
    QObject::connect(delaunay.get(), SIGNAL(signalBeforeFlip(HHandle,VHandle,VHandle)),
                     ui->viewer, SLOT(slotBeforeFlip(HHandle,VHandle,VHandle)));
    QObject::connect(delaunay.get(), SIGNAL(signalAfterFlip()),
                     ui->viewer, SLOT(slotAfterFlip()));
}

void DViewerWindow::test()
{
    qDebug() << "DViewerWindow slot test";
}

DViewerWindow::~DViewerWindow()
{
    delete ui;
}

void DViewerWindow::on_pushButtonNext_clicked()
{

}
