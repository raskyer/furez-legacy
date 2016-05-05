#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "editorwindow.h"

#include <QMessageBox>

#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QNetworkRequest>

#include "QFuzzer.h"
#include "Cmd.h"
#include "Parser.h"
#include "QDownloader.h"

#include <QMediaPlayer>
#include <QMediaPlaylist>

#include <QStandardItemModel>
#include <QStandardItem>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->tabWidget->setTabEnabled(1, false);

    bool song;
    song = false;

    if(song) {
        QMediaPlaylist *playlist = new QMediaPlaylist;
        playlist->addMedia(QUrl::fromLocalFile("C:\\Users\\Vincent\\Documents\\Dev\\qt\\air.mp3"));
        playlist->setPlaybackMode( QMediaPlaylist::Loop );

        QMediaPlayer *player = new QMediaPlayer();
        player->setPlaylist(playlist);
        player->setVolume(50);
        player->play();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

QString MainWindow::getUrl()
{
    QString url = this->ui->url->text();
    url = "http://localhost/public/perl.php";

    return url;
}

QString MainWindow::getHeader()
{
    QString header = this->ui->header->text();

    if(header.isEmpty()) {
        header = "Nes";
    }

    return header;
}

QAbstractButton* MainWindow::getExecuter()
{
    QButtonGroup * executeCommand = new QButtonGroup(this);
    executeCommand->addButton(this->ui->shellExec);
    executeCommand->addButton(this->ui->exec);
    executeCommand->addButton(this->ui->passthru);

    QAbstractButton * radio = executeCommand->checkedButton();

    return radio;
}

void MainWindow::on_save_clicked()
{
    url = this->getUrl();
    header = this->getHeader();
    //QAbstractButton * radio = executeCommand->checkedButton();

    if(url.isEmpty()) {
        QMessageBox alert;
        alert.setIcon(alert.Critical);
        alert.setText("You must fill in the url attribute");
        alert.exec();
        return;
    }

    fuzzer = new QFuzzer(0, header, url);
    cmd = new Cmd(fuzzer);
    parser = new Parser();

    std::vector<std::string> response = cmd->listDir("/");

    if(response[0] == "ERROR") {
        return;
    }

    handleTreeView(response);
    this->ui->tabWidget->setTabEnabled(1, true);
    this->ui->settings->setEnabled(false);
    this->ui->tabWidget->setCurrentIndex(1);
}

void MainWindow::on_treeView_clicked(const QModelIndex &index)
{
    QStandardItem *itemPath = rootPathModel->itemFromIndex(index);
    QString type = index.sibling(index.row(), 1).data().toString();

    if(!itemPath->hasChildren()) {
        QStandardItem *sourcePath = itemPath->parent();
        context = "";
        while(sourcePath != 0) {
            context = sourcePath->text() + context;
            sourcePath = sourcePath->parent();
        }

        context = "/" + context + itemPath->text();
        elemName = itemPath->text();

        if(type == "Folder") {
            std::vector<std::string> responseArr = cmd->listDir(context);
            prepareBlock(responseArr, itemPath);
        }
    }

    if(!this->ui->treeView->isExpanded(index)) {
        this->ui->treeView->expand(index);
    } else {
        this->ui->treeView->collapse(index);
    }
}

void MainWindow::on_show_clicked()
{
    QString fileContent;

    fileContent = cmd->getFileContent(context);
}

void MainWindow::on_edit_clicked()
{
    QString response, cacheDir;

    cacheDir = "cache/";

    EditorWindow *edit = new EditorWindow(this, fuzzer, elemName, context, cacheDir + elemName);
    QDownloader *qDownloader = new QDownloader(0, header, url, cacheDir, edit);
    response = qDownloader->setFile(context, elemName);
}

void MainWindow::handleTreeView(std::vector<std::string> response)
{
    rootPathModel = new QStandardItemModel();
    QStandardItem *pathItem = rootPathModel->invisibleRootItem();

    prepareBlock(response, pathItem);

    rootPathModel->setHorizontalHeaderItem(0, new QStandardItem("Name"));
    rootPathModel->setHorizontalHeaderItem(1, new QStandardItem("Type"));

    this->ui->treeView->setModel(rootPathModel);
    this->ui->treeView->setColumnWidth(0, 400);
}

void MainWindow::prepareBlock(std::vector<std::string> response, QStandardItem * pathItem)
{
    QString folderName, fileName;

    std::vector<std::string> folders = parser->parseSpace(response[0]);
    for(auto& s: folders) {
        QList<QStandardItem *> rowItem;

        folderName = s.c_str();
        QStandardItem * item = new QStandardItem(folderName);

        item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled);

        rowItem << item;
        rowItem << new QStandardItem("Folder");

        pathItem->appendRow(rowItem);
    }

    if(1 < response.size()) {
        std::vector<std::string> files = parser->parseSpace(response[1]);
        for(auto& s: files) {
            QList<QStandardItem *> rowItem;

            fileName = s.c_str();
            QStandardItem * item = new QStandardItem(fileName);

            item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled);

            rowItem << item;
            rowItem << new QStandardItem("File");

            pathItem->appendRow(rowItem);
        }
    }
}