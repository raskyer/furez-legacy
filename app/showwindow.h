#ifndef SHOWWINDOW_H
#define SHOWWINDOW_H

#include <QWidget>

namespace Ui {
class ShowWindow;
}

class ShowWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ShowWindow(QWidget *parent = 0, QString = "", QString = "");
    ~ShowWindow();

private:
    Ui::ShowWindow *ui;
};

#endif // SHOWWINDOW_H
