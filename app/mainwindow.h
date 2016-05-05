#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QStandardItem>
#include <QMainWindow>

#include <QAbstractButton>

#include "Cmd.h"
#include "QFuzzer.h"
#include "Parser.h"
#include "UiService.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void save_finish(std::vector<std::string>);
    void treeView_finish(std::vector<std::string>);
    void show_finish(QString);

    void progressUpdate(qint64, qint64);

private slots:
    void on_save_clicked();
    void on_treeView_clicked(const QModelIndex &index);
    void on_show_clicked();
    void on_edit_clicked();

private:
    Ui::MainWindow *ui;

    //Object
    Cmd *cmd;
    QFuzzer *fuzzer;
    Parser *parser;
    UiService *uiService;

    //Variables, g for Global
    QString gContext, gFileName, gHeader, gUrl, gRootPath, gCacheDir;
    QStandardItem *currentItem;

    //Getters
    QString getUrl();
    QString getHeader();
    QAbstractButton* getExecuter();
    QString getRootPath();
    QString getCacheDir();

    //Utils
    bool checkGlobals();
};

#endif // MAINWINDOW_H
