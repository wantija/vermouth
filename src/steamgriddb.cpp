#include "steamgriddb.h"
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkRequest>
#include <QRegularExpression>

SteamGridDB::SteamGridDB(QObject *parent)
    : QObject(parent)
{
}

bool SteamGridDB::busy() const
{
    return m_busy;
}

QString SteamGridDB::statusText() const
{
    return m_statusText;
}

void SteamGridDB::setBusy(bool busy)
{
    if (m_busy != busy) {
        m_busy = busy;
        Q_EMIT busyChanged();
    }
}

void SteamGridDB::setStatusText(const QString &text)
{
    if (m_statusText != text) {
        m_statusText = text;
        Q_EMIT statusTextChanged();
    }
}

void SteamGridDB::makeRequest(const QUrl &url, const QString &apiKey, const std::function<void(const QJsonArray &)> &callback)
{
    QNetworkRequest req(url);
    req.setHeader(QNetworkRequest::UserAgentHeader, QStringLiteral("Vermouth"));
    req.setRawHeader(QByteArrayLiteral("Authorization"), QByteArrayLiteral("Bearer ") + apiKey.toUtf8());

    auto *reply = m_nam.get(req);
    connect(reply, &QNetworkReply::finished, this, [this, reply, callback]() {
        reply->deleteLater();
        if (reply->error() != QNetworkReply::NoError) {
            Q_EMIT error(reply->errorString());
            setBusy(false);
            return;
        }

        auto doc = QJsonDocument::fromJson(reply->readAll());
        auto obj = doc.object();
        if (!obj.value(QStringLiteral("success")).toBool(false)) {
            auto errors = obj.value(QStringLiteral("errors")).toArray();
            QString errMsg = errors.isEmpty() ? tr("Unknown API error") : errors.first().toString();
            Q_EMIT error(errMsg);
            setBusy(false);
            return;
        }

        callback(obj.value(QStringLiteral("data")).toArray());
    });
}

void SteamGridDB::searchGames(const QString &query, const QString &apiKey)
{
    if (busy() || query.isEmpty() || apiKey.isEmpty())
        return;

    setBusy(true);
    setStatusText(tr("Searching SteamGridDB…"));

    QUrl url(QStringLiteral("https://www.steamgriddb.com/api/v2/search/autocomplete/%1").arg(QString::fromUtf8(QUrl::toPercentEncoding(query))));
    makeRequest(url, apiKey, [this](const QJsonArray &data) {
        QVariantList games;
        for (const auto &val : data) {
            auto obj = val.toObject();
            QVariantMap game;
            game[QStringLiteral("id")] = obj.value(QStringLiteral("id")).toInt();
            game[QStringLiteral("name")] = obj.value(QStringLiteral("name")).toString();
            game[QStringLiteral("verified")] = obj.value(QStringLiteral("verified")).toBool();
            games.append(game);
        }
        setBusy(false);
        Q_EMIT searchFinished(games);
    });
}

void SteamGridDB::fetchGrids(int gameId, const QString &apiKey)
{
    if (busy() || gameId <= 0 || apiKey.isEmpty())
        return;

    setBusy(true);
    setStatusText(tr("Fetching grids…"));

    QUrl url(QStringLiteral("https://www.steamgriddb.com/api/v2/grids/game/%1").arg(gameId));
    makeRequest(url, apiKey, [this](const QJsonArray &data) {
        QVariantList items;
        for (const auto &val : data) {
            auto obj = val.toObject();
            QVariantMap item;
            item[QStringLiteral("id")] = obj.value(QStringLiteral("id")).toInt();
            item[QStringLiteral("url")] = obj.value(QStringLiteral("url")).toString();
            item[QStringLiteral("thumb")] = obj.value(QStringLiteral("thumb")).toString();
            item[QStringLiteral("score")] = obj.value(QStringLiteral("score")).toInt();
            item[QStringLiteral("style")] = obj.value(QStringLiteral("style")).toString();
            auto author = obj.value(QStringLiteral("author")).toObject();
            item[QStringLiteral("author")] = author.value(QStringLiteral("name")).toString();
            items.append(item);
        }
        setBusy(false);
        Q_EMIT gridsFinished(items);
    });
}

void SteamGridDB::fetchHeroes(int gameId, const QString &apiKey)
{
    if (busy() || gameId <= 0 || apiKey.isEmpty())
        return;

    setBusy(true);
    setStatusText(tr("Fetching heroes…"));

    QUrl url(QStringLiteral("https://www.steamgriddb.com/api/v2/heroes/game/%1").arg(gameId));
    makeRequest(url, apiKey, [this](const QJsonArray &data) {
        QVariantList items;
        for (const auto &val : data) {
            auto obj = val.toObject();
            QVariantMap item;
            item[QStringLiteral("id")] = obj.value(QStringLiteral("id")).toInt();
            item[QStringLiteral("url")] = obj.value(QStringLiteral("url")).toString();
            item[QStringLiteral("thumb")] = obj.value(QStringLiteral("thumb")).toString();
            item[QStringLiteral("score")] = obj.value(QStringLiteral("score")).toInt();
            item[QStringLiteral("style")] = obj.value(QStringLiteral("style")).toString();
            auto author = obj.value(QStringLiteral("author")).toObject();
            item[QStringLiteral("author")] = author.value(QStringLiteral("name")).toString();
            items.append(item);
        }
        setBusy(false);
        Q_EMIT heroesFinished(items);
    });
}

void SteamGridDB::fetchIcons(int gameId, const QString &apiKey)
{
    if (busy() || gameId <= 0 || apiKey.isEmpty())
        return;

    setBusy(true);
    setStatusText(tr("Fetching icons…"));

    QUrl url(QStringLiteral("https://www.steamgriddb.com/api/v2/icons/game/%1").arg(gameId));
    makeRequest(url, apiKey, [this](const QJsonArray &data) {
        QVariantList items;
        for (const auto &val : data) {
            auto obj = val.toObject();
            QVariantMap item;
            item[QStringLiteral("id")] = obj.value(QStringLiteral("id")).toInt();
            item[QStringLiteral("url")] = obj.value(QStringLiteral("url")).toString();
            item[QStringLiteral("thumb")] = obj.value(QStringLiteral("thumb")).toString();
            item[QStringLiteral("score")] = obj.value(QStringLiteral("score")).toInt();
            item[QStringLiteral("style")] = obj.value(QStringLiteral("style")).toString();
            auto author = obj.value(QStringLiteral("author")).toObject();
            item[QStringLiteral("author")] = author.value(QStringLiteral("name")).toString();
            items.append(item);
        }
        setBusy(false);
        Q_EMIT iconsFinished(items);
    });
}

void SteamGridDB::fetchLogos(int gameId, const QString &apiKey)
{
    if (busy() || gameId <= 0 || apiKey.isEmpty())
        return;

    setBusy(true);
    setStatusText(tr("Fetching logos…"));

    QUrl url(QStringLiteral("https://www.steamgriddb.com/api/v2/logos/game/%1").arg(gameId));
    makeRequest(url, apiKey, [this](const QJsonArray &data) {
        QVariantList items;
        for (const auto &val : data) {
            auto obj = val.toObject();
            QVariantMap item;
            item[QStringLiteral("id")] = obj.value(QStringLiteral("id")).toInt();
            item[QStringLiteral("url")] = obj.value(QStringLiteral("url")).toString();
            item[QStringLiteral("thumb")] = obj.value(QStringLiteral("thumb")).toString();
            item[QStringLiteral("score")] = obj.value(QStringLiteral("score")).toInt();
            item[QStringLiteral("style")] = obj.value(QStringLiteral("style")).toString();
            auto author = obj.value(QStringLiteral("author")).toObject();
            item[QStringLiteral("author")] = author.value(QStringLiteral("name")).toString();
            items.append(item);
        }
        setBusy(false);
        Q_EMIT logosFinished(items);
    });
}

void SteamGridDB::downloadImage(const QString &url, const QString &savePath)
{
    if (busy() || url.isEmpty())
        return;

    setBusy(true);
    setStatusText(tr("Downloading…"));

    QDir().mkpath(QFileInfo(savePath).path());

    QNetworkRequest req{QUrl(url)};
    req.setHeader(QNetworkRequest::UserAgentHeader, QStringLiteral("Vermouth"));

    auto *reply = m_nam.get(req);
    connect(reply, &QNetworkReply::downloadProgress, this, [this](qint64 received, qint64 total) {
        if (total > 0)
            Q_EMIT downloadProgress(static_cast<double>(received) / static_cast<double>(total));
    });
    connect(reply, &QNetworkReply::finished, this, [this, reply, savePath]() {
        reply->deleteLater();
        if (reply->error() != QNetworkReply::NoError) {
            Q_EMIT error(reply->errorString());
            setBusy(false);
            return;
        }

        QFile file(savePath);
        if (!file.open(QIODevice::WriteOnly)) {
            Q_EMIT error(tr("Failed to write file"));
            setBusy(false);
            return;
        }
        file.write(reply->readAll());
        file.close();

        Q_EMIT downloadFinished(savePath);
        setStatusText(tr("Downloaded!"));
        setBusy(false);
    });
}

// ── Auto-download chain ────────────────────────────────────────────────────

void SteamGridDB::autoDownloadAll(const QString &gameName, const QString &assetsPath, const QString &apiKey)
{
    if (m_autoBusy)
        return;

    m_autoBusy = true;
    Q_EMIT autoDownloadingChanged();

    m_autoApiKey = apiKey;
    m_autoAssetsPath = assetsPath;
    m_autoSafeName = QString(gameName).replace(QRegularExpression(QStringLiteral("[^a-zA-Z0-9_-]")), QStringLiteral("_")).toLower();
    m_autoGameId = 0;
    m_autoIconPath.clear();
    m_autoGridPath.clear();
    m_autoHeroPath.clear();
    m_autoLogoPath.clear();

    Q_EMIT autoDownloadProgress(tr("Searching…"));

    QUrl url(QStringLiteral("https://www.steamgriddb.com/api/v2/search/autocomplete/%1").arg(QString::fromUtf8(QUrl::toPercentEncoding(gameName))));

    autoMakeRequest(url, [this](const QJsonArray &data) {
        if (data.isEmpty()) {
            autoFinish();
            return;
        }
        m_autoGameId = data.first().toObject().value(QStringLiteral("id")).toInt();
        autoFetchIcons();
    });
}

void SteamGridDB::autoMakeRequest(const QUrl &url, std::function<void(const QJsonArray &)> onSuccess)
{
    QNetworkRequest req(url);
    req.setHeader(QNetworkRequest::UserAgentHeader, QStringLiteral("Vermouth"));
    req.setRawHeader(QByteArrayLiteral("Authorization"), QByteArrayLiteral("Bearer ") + m_autoApiKey.toUtf8());

    auto *reply = m_nam.get(req);
    connect(reply, &QNetworkReply::finished, this, [this, reply, onSuccess]() {
        reply->deleteLater();
        if (reply->error() != QNetworkReply::NoError) {
            autoFinish();
            return;
        }

        auto doc = QJsonDocument::fromJson(reply->readAll());
        auto obj = doc.object();
        if (!obj.value(QStringLiteral("success")).toBool(false)) {
            onSuccess(QJsonArray());
            return;
        }

        onSuccess(obj.value(QStringLiteral("data")).toArray());
    });
}

void SteamGridDB::autoDownloadFile(const QString &imgUrl, const QString &suffix, std::function<void(const QString &)> onDone)
{
    QString ext = imgUrl.section(QLatin1Char('.'), -1);
    int qIdx = ext.indexOf(QLatin1Char('?'));
    if (qIdx >= 0)
        ext = ext.left(qIdx);
    if (ext.isEmpty())
        ext = QStringLiteral("png");

    QString savePath = m_autoAssetsPath + QLatin1Char('/') + m_autoSafeName + QLatin1Char('_') + suffix + QLatin1Char('.') + ext;
    QDir().mkpath(m_autoAssetsPath);

    QNetworkRequest req{QUrl(imgUrl)};
    req.setHeader(QNetworkRequest::UserAgentHeader, QStringLiteral("Vermouth"));

    auto *reply = m_nam.get(req);
    connect(reply, &QNetworkReply::finished, this, [reply, savePath, onDone]() {
        reply->deleteLater();
        if (reply->error() != QNetworkReply::NoError) {
            onDone(QString());
            return;
        }

        QFile file(savePath);
        if (!file.open(QIODevice::WriteOnly)) {
            onDone(QString());
            return;
        }
        file.write(reply->readAll());
        file.close();
        onDone(savePath);
    });
}

void SteamGridDB::autoFetchIcons()
{
    Q_EMIT autoDownloadProgress(tr("Downloading icon…"));
    QUrl url(QStringLiteral("https://www.steamgriddb.com/api/v2/icons/game/%1").arg(m_autoGameId));
    autoMakeRequest(url, [this](const QJsonArray &data) {
        if (data.isEmpty()) {
            autoFetchGrids();
            return;
        }
        QString imgUrl = data.first().toObject().value(QStringLiteral("url")).toString();
        autoDownloadFile(imgUrl, QStringLiteral("icon"), [this](const QString &path) {
            m_autoIconPath = path;
            autoFetchGrids();
        });
    });
}

void SteamGridDB::autoFetchGrids()
{
    Q_EMIT autoDownloadProgress(tr("Downloading grid…"));
    QUrl url(QStringLiteral("https://www.steamgriddb.com/api/v2/grids/game/%1").arg(m_autoGameId));
    autoMakeRequest(url, [this](const QJsonArray &data) {
        if (data.isEmpty()) {
            autoFetchHeroes();
            return;
        }
        QString imgUrl = data.first().toObject().value(QStringLiteral("url")).toString();
        autoDownloadFile(imgUrl, QStringLiteral("grid"), [this](const QString &path) {
            m_autoGridPath = path;
            autoFetchHeroes();
        });
    });
}

void SteamGridDB::autoFetchHeroes()
{
    Q_EMIT autoDownloadProgress(tr("Downloading hero…"));
    QUrl url(QStringLiteral("https://www.steamgriddb.com/api/v2/heroes/game/%1").arg(m_autoGameId));
    autoMakeRequest(url, [this](const QJsonArray &data) {
        if (data.isEmpty()) {
            autoFetchLogos();
            return;
        }
        QString imgUrl = data.first().toObject().value(QStringLiteral("url")).toString();
        autoDownloadFile(imgUrl, QStringLiteral("hero"), [this](const QString &path) {
            m_autoHeroPath = path;
            autoFetchLogos();
        });
    });
}

void SteamGridDB::autoFetchLogos()
{
    Q_EMIT autoDownloadProgress(tr("Downloading logo…"));
    QUrl url(QStringLiteral("https://www.steamgriddb.com/api/v2/logos/game/%1").arg(m_autoGameId));
    autoMakeRequest(url, [this](const QJsonArray &data) {
        if (data.isEmpty()) {
            autoFinish();
            return;
        }
        QString imgUrl = data.first().toObject().value(QStringLiteral("url")).toString();
        autoDownloadFile(imgUrl, QStringLiteral("logo"), [this](const QString &path) {
            m_autoLogoPath = path;
            autoFinish();
        });
    });
}

void SteamGridDB::autoFinish()
{
    m_autoBusy = false;
    Q_EMIT autoDownloadingChanged();
    Q_EMIT autoDownloadFinished(m_autoGameId, m_autoIconPath, m_autoGridPath, m_autoHeroPath, m_autoLogoPath);
}
