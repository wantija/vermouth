#include "downloader.h"

Downloader::Downloader(QObject *parent)
    : QObject(parent)
{
    m_statusClearTimer.setSingleShot(true);
    m_statusClearTimer.setInterval(6000);
    connect(&m_statusClearTimer, &QTimer::timeout, this, [this]() {
        if (!m_busy)
            setStatusText(QString());
    });
}

bool Downloader::busy() const
{
    return m_busy;
}

QString Downloader::statusText() const
{
    return m_statusText;
}

double Downloader::progress() const
{
    return m_progress;
}

void Downloader::setBusy(bool busy)
{
    if (m_busy != busy) {
        m_busy = busy;
        Q_EMIT busyChanged();
        if (!busy)
            m_statusClearTimer.start();
        else
            m_statusClearTimer.stop();
    }
}

void Downloader::setStatusText(const QString &text)
{
    if (m_statusText != text) {
        m_statusText = text;
        Q_EMIT statusTextChanged();
    }
}

void Downloader::setProgress(double progress)
{
    if (m_progress != progress) {
        m_progress = progress;
        Q_EMIT progressChanged();
    }
}
