#include "showwindow.h"
#include "ui_showwindow.h"

ShowWindow::ShowWindow(QWidget *parent, QString title, QString content) :
    QWidget(parent),
    ui(new Ui::ShowWindow)
{
    ui->setupUi(this);
    this->ui->fileBrowser->setText(content);
}

ShowWindow::~ShowWindow()
{
    delete ui;
}
