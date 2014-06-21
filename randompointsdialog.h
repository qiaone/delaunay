#ifndef RANDOMPOINTSDIALOG_H
#define RANDOMPOINTSDIALOG_H

#include <QDialog>

namespace Ui {
class RandomPointsDialog;
}

class RandomPointsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RandomPointsDialog(QWidget *parent = 0);
    ~RandomPointsDialog();
    int getPointsNumber();

private slots:
    void on_checkBox_toggled(bool checked);

private:
    Ui::RandomPointsDialog *ui;
};

#endif // RANDOMPOINTSDIALOG_H
