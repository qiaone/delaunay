#include "randompointsdialog.h"
#include "ui_randompointsdialog.h"

RandomPointsDialog::RandomPointsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RandomPointsDialog)
{
    ui->setupUi(this);
}

RandomPointsDialog::~RandomPointsDialog()
{
    delete ui;
}

int RandomPointsDialog::getPointsNumber()
{
    return ui->spinBox->value();
}
