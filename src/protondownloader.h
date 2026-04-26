#pragma once

#include "downloader.h"
#include <QNetworkReply>

class QProcess;
class QTemporaryFile;

class ProtonDownloader : public Downloader
{
    Q_OBJECT

public:
    explicit ProtonDownloader(QObject *parent = nullptr);

    void setLocalProtonPath(const QString &path);

    Q_INVOKABLE void downloadLatest();

Q_SIGNALS:
    void finished();
    void error(const QString &message);

private:
    void onReleaseFetched(QNetworkReply *reply);
    void startExtraction(QTemporaryFile *archiveFile);

    QString m_localProtonPath;
    QProcess *m_extractProc = nullptr;
};
