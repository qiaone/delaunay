#ifndef DVIEWERWINDOW_H
#define DVIEWERWINDOW_H

#include <QWidget>

namespace Ui {
class DViewerWindow;
}

class DViewerWindow : public QWidget
{
    Q_OBJECT

public:
    explicit DViewerWindow(QWidget *parent = 0);
    ~DViewerWindow();

private:
    Ui::DViewerWindow *ui;
};

#endif // DVIEWERWINDOW_H
