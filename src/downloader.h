#pragma once

#include <QNetworkAccessManager>
#include <QObject>
#include <QTimer>

class Downloader : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool busy READ busy NOTIFY busyChanged)
    Q_PROPERTY(QString statusText READ statusText NOTIFY statusTextChanged)
    Q_PROPERTY(double progress READ progress NOTIFY progressChanged)

public:
    explicit Downloader(QObject *parent = nullptr);

    bool busy() const;
    QString statusText() const;
    double progress() const;

Q_SIGNALS:
    void busyChanged();
    void statusTextChanged();
    void progressChanged();

protected:
    void setBusy(bool busy);
    void setStatusText(const QString &text);
    void setProgress(double progress);
    QNetworkAccessManager &nam()
    {
        return m_nam;
    }

private:
    QNetworkAccessManager m_nam;
    QTimer m_statusClearTimer;
    bool m_busy = false;
    QString m_statusText;
    double m_progress = 0.0;
};
