#pragma once

#include "downloader.h"
#include <QNetworkReply>

class QProcess;
class QTemporaryFile;

class UmuDownloader : public Downloader
{
    Q_OBJECT

public:
    explicit UmuDownloader(QObject *parent = nullptr);

    void setInstallPath(const QString &path);

    Q_INVOKABLE void downloadLatest();

Q_SIGNALS:
    void finished(const QString &umuBinPath);
    void error(const QString &message);

private:
    void onReleaseFetched(QNetworkReply *reply);
    void startTarExtraction(QTemporaryFile *archiveFile);

    QString m_installPath;
    QProcess *m_tarProc = nullptr;
};
