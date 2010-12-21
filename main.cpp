/*
 *  ImageUpload
 *
 *  Copyright (C) 2010  Volker Grabsch <vog@m-click.de>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  <http://www.gnu.org/licenses/#GPL>
 */

#include <QtCore/QBuffer>
#include <QtCore/QFile>
#include <QtCore/QMimeData>
#include <QtCore/QStringList>
#include <QtCore/QTimer>
#include <QtGui/QApplication>
#include <QtGui/QClipboard>
#include <QtGui/QMessageBox>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QSslError>

class Upload: public QObject
{
    Q_OBJECT

public:
    Upload(const QUrl &url, const QByteArray &data, QObject *parent=0);

private slots:
    void finished(QNetworkReply *reply);
    void userConfirmationOnSslErrors(QNetworkReply *reply, const QList<QSslError> &errors);
};

Upload::Upload(const QUrl &url, const QByteArray &data, QObject *parent): QObject(parent)
{
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(finished(QNetworkReply*)));
    connect(manager, SIGNAL(sslErrors(QNetworkReply*, QList<QSslError>)), this, SLOT(userConfirmationOnSslErrors(QNetworkReply*, QList<QSslError>)));

    QNetworkRequest request(url);
    manager->post(request, data);
}

void Upload::finished(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        const QString contentType = reply->header(QNetworkRequest::ContentTypeHeader).toString();
        const QByteArray content = reply->readAll();
        QMimeData *mimeData = new QMimeData;
        mimeData->setData(contentType, content);
        QApplication::clipboard()->setMimeData(mimeData);
        // Workaround for bug:
        // http://bugreports.qt.nokia.com/browse/QTBUG-16194
        QTimer::singleShot(30000, QCoreApplication::instance(), SLOT(quit()));
    } else {
        QMessageBox::critical(
            0,
            tr("Upload error"),
            tr("Network error #%1 while uploading to:\n\n%2").arg(reply->error()).arg(reply->url().toString())
        );
        QCoreApplication::exit(1);
    }
}

void Upload::userConfirmationOnSslErrors(QNetworkReply *reply, const QList<QSslError> &errors)
{
    QString errorMessage = tr("SSL error") + "\n\n";
    foreach (const QSslError &error, errors) {
        errorMessage += QString("%1 (%2)\n").arg(error.errorString()).arg(error.error());
    }
    const QMessageBox::StandardButton answer = QMessageBox::Yes;
    // TODO: make this confirmation box work
/*
    const QMessageBox::StandardButton answer = QMessageBox::question(
        0,
        tr("SSL error"),
        errorMessage + "\n" + tr("Do you still want to upload?"),
        QMessageBox::Yes | QMessageBox::No
    );
*/
    if (answer == QMessageBox::Yes) {
        reply->ignoreSslErrors(errors);
    }
}

int main(int argc, char *argv[])
{
    QApplication application(argc, argv);
    QCoreApplication::setApplicationName("ImageUpload");
    QCoreApplication::setApplicationVersion(VERSION);

    const QStringList args = QCoreApplication::instance()->arguments();
    if (args.size() <= 1) {
        QMessageBox::critical(0, QCoreApplication::tr("Missing URL file"), QCoreApplication::tr("Missing URL file"));
        return 1;
    }

    QFile urlFile(args.at(1));
    urlFile.open(QIODevice::ReadOnly);
    const QUrl url = QUrl::fromEncoded(urlFile.readAll(), QUrl::StrictMode);
    if (!url.isValid()) {
        QMessageBox::critical(0, QCoreApplication::tr("Invalid URL in file"), QCoreApplication::tr("Invalid URL in file"));
        return 1;
    }

    QImage image = QApplication::clipboard()->image();
    if (image.isNull()) {
        QMessageBox::critical(0, QCoreApplication::tr("Missing image in clipboard"), QCoreApplication::tr("Missing image in clipboard"));
        return 1;
    }
    QByteArray data;
    QBuffer buffer(&data);
    buffer.open(QIODevice::WriteOnly);
    image.save(&buffer, "PNG");

    Upload upload(url, data);
    return application.exec();
}

#include <main.moc>
