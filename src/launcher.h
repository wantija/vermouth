#pragma once

#include <QObject>
#include <QProcess>

class Launcher : public QObject {
    Q_OBJECT

public:
    explicit Launcher(QObject *parent = nullptr);

    Q_INVOKABLE void launchProton(const QString &protonPath, const QString &prefix,
                                  const QString &exePath, const QStringList &args = {},
                                  const QString &launchOptions = QString(),
                                  bool enableLogging = false, const QString &logName = QString());
    Q_INVOKABLE void launchWine(const QString &wineBinary, const QString &prefix,
                                const QString &exePath, const QStringList &args = {},
                                const QString &launchOptions = QString(),
                                bool enableLogging = false, const QString &logName = QString());
    Q_INVOKABLE void launchEntry(const QVariantMap &app);
    Q_INVOKABLE void runInPrefix(const QVariantMap &app, const QString &exePath);
    Q_INVOKABLE QString logDir() const;

signals:
    void launched(const QString &name);
    void launchError(const QString &name, const QString &error);

private:
    void setupLogging(QProcess *proc, const QString &name);
    QString m_logDir;
};
