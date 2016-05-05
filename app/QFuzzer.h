#ifndef QFUZZER_H
#define QFUZZER_H

#include <QObject>
#include <QNetworkReply>
#include <QProgressBar>

class QFuzzer : public QObject
{
    Q_OBJECT
public:
    explicit QFuzzer(QObject *parent = 0, QString h = "Nes", QString u = "http://localhost/hack/perl.php");
    void sendRequest(QString);
    QString uploadFile(QString, QString, QString);
    QString getResponse();

signals:
    void finishRequest();

private:
    QString normalizeRequest(QString);
    QByteArray buildUploadString(QString, QString);

    QString header;
    QString url;
    QString response;
    bool error;
    QNetworkAccessManager *gManager;

private slots:
    void handleReturn();
    void handleError(QNetworkReply::NetworkError);
    void httpRequestFinished(QNetworkReply *);
};

#endif // QFUZZER_H
