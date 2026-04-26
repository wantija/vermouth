#include "protondownloader.h"
#include <QDir>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QProcess>
#include <QTemporaryFile>

ProtonDownloader::ProtonDownloader(QObject *parent)
    : Downloader(parent)
{
}

void ProtonDownloader::setLocalProtonPath(const QString &path)
{
    m_localProtonPath = path;
}

void ProtonDownloader::downloadLatest()
{
    if (busy())
        return;

    setBusy(true);
    setStatusText(tr("Checking latest release…"));
    setProgress(0.0);

    QNetworkRequest req(QUrl(QStringLiteral("https://github.com/GloriousEggroll/proton-ge-custom/releases/latest")));
    req.setHeader(QNetworkRequest::UserAgentHeader, QStringLiteral("Vermouth"));
    req.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::ManualRedirectPolicy);
    auto *reply = nam().get(req, QByteArray());
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        onReleaseFetched(reply);
    });
}

void ProtonDownloader::onReleaseFetched(QNetworkReply *reply)
{
    reply->deleteLater();

    if (reply->error() != QNetworkReply::NoError) {
        setStatusText(tr("Failed to fetch release info: %1").arg(reply->errorString()));
        setBusy(false);
        Q_EMIT error(reply->errorString());
        return;
    }

    QUrl redirectUrl = reply->header(QNetworkRequest::LocationHeader).toUrl();
    QString path = redirectUrl.path();
    QString tagName = path.mid(path.lastIndexOf(QLatin1Char('/')) + 1);

    if (tagName.isEmpty()) {
        setStatusText(tr("Could not determine latest version"));
        setBusy(false);
        Q_EMIT error(QStringLiteral("No tag found in redirect URL"));
        return;
    }

    QDir localDir(m_localProtonPath);
    for (const auto &entry : localDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        if (entry.contains(tagName, Qt::CaseInsensitive)) {
            setStatusText(tr("Latest version (%1) is already installed").arg(tagName));
            setBusy(false);
            Q_EMIT finished();
            return;
        }
    }

    QString downloadUrl = QStringLiteral("https://github.com/GloriousEggroll/proton-ge-custom/releases/download/%1/%1.tar.gz").arg(tagName);

    setStatusText(tr("Downloading %1…").arg(tagName));

    auto *tmpFile = new QTemporaryFile(QDir::tempPath() + QStringLiteral("/vermouth-proton-XXXXXX.tar.gz"));
    if (!tmpFile->open()) {
        setStatusText(tr("Failed to create temp file"));
        setBusy(false);
        Q_EMIT error(QStringLiteral("Could not create temporary file"));
        delete tmpFile;
        return;
    }

    QUrl dlUrl(downloadUrl);
    QNetworkRequest dlReq(dlUrl);
    dlReq.setHeader(QNetworkRequest::UserAgentHeader, QStringLiteral("Vermouth"));
    dlReq.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);
    QNetworkReply *dlReply = nam().get(dlReq, QByteArray());
    connect(dlReply, &QNetworkReply::downloadProgress, this, [this](qint64 received, qint64 total) {
        if (total > 0)
            setProgress(static_cast<double>(received) / static_cast<double>(total));
    });
    connect(dlReply, &QNetworkReply::readyRead, this, [dlReply, tmpFile]() {
        tmpFile->write(dlReply->readAll());
    });
    connect(dlReply, &QNetworkReply::finished, this, [this, dlReply, tmpFile]() {
        tmpFile->flush();
        tmpFile->close();
        if (dlReply->error() != QNetworkReply::NoError) {
            setStatusText(tr("Download failed"));
            setBusy(false);
            Q_EMIT error(dlReply->errorString());
            delete tmpFile;
            dlReply->deleteLater();
            return;
        }
        setStatusText(tr("Extracting…"));
        setProgress(1.0);
        dlReply->deleteLater();
        startExtraction(tmpFile);
    });
}

void ProtonDownloader::startExtraction(QTemporaryFile *archiveFile)
{
    delete m_extractProc;
    m_extractProc = new QProcess(this);
    connect(m_extractProc, &QProcess::finished, this, [this, archiveFile](int exitCode) {
        delete archiveFile;
        if (exitCode != 0) {
            setStatusText(tr("Extraction failed"));
            setBusy(false);
            Q_EMIT error(QStringLiteral("tar extraction failed"));
            return;
        }
        setStatusText(tr("Done!"));
        setBusy(false);
        Q_EMIT finished();
    });

    QDir().mkpath(m_localProtonPath);
    m_extractProc->setProgram(QStringLiteral("tar"));
    m_extractProc->setArguments({QStringLiteral("-xzf"), archiveFile->fileName(), QStringLiteral("-C"), m_localProtonPath});
    m_extractProc->start();
}
