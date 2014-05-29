#include "dviewerwindow.h"
#include "ui_dviewerwindow.h"

DViewerWindow::DViewerWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DViewerWindow)
{
    ui->setupUi(this);
}

DViewerWindow::~DViewerWindow()
{
    delete ui;
}
