#include "launcher.h"
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QRegularExpression>
#include <QStandardPaths>

Launcher::Launcher(QObject *parent)
    : QObject(parent)
{
    m_logDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/logs";
    QDir().mkpath(m_logDir);
}

QString Launcher::logDir() const {
    return m_logDir;
}

static QString shellQuote(const QString &s) {
    QString quoted = s;
    quoted.replace("'", "'\\''");
    return "'" + quoted + "'";
}

void Launcher::launch(const QString &binary, const QStringList &baseArgs,
                      const QString &exePath, const QProcessEnvironment &env,
                      const QString &launchOptions, bool enableLogging,
                      const QString &logName) {
    auto *proc = new QProcess(this);
    connect(proc, &QProcess::finished, proc, &QProcess::deleteLater);

    proc->setProcessEnvironment(env);
    proc->setWorkingDirectory(QFileInfo(exePath).absolutePath());

    if (enableLogging) {
        proc->setProcessChannelMode(QProcess::SeparateChannels);
        setupLogging(proc, logName.isEmpty() ? QFileInfo(exePath).baseName() : logName);
    }

    if (!launchOptions.trimmed().isEmpty()) {
        // Build quoted base command: binary [baseArgs...] exePath
        QString baseCmd = shellQuote(binary);
        for (const auto &a : baseArgs)
            baseCmd += " " + shellQuote(a);
        baseCmd += " " + shellQuote(exePath);

        QString opts = launchOptions.trimmed();
        QString fullCmd = opts.contains("%command%")
            ? QString(opts).replace("%command%", baseCmd)
            : opts + " " + baseCmd;

        proc->start("/bin/sh", {"-c", fullCmd});
    } else {
        QStringList args = baseArgs;
        args << exePath;
        proc->start(binary, args);
    }

    if (!proc->waitForStarted(5000)) {
        emit launchError(exePath, proc->errorString());
        proc->deleteLater();
    } else {
        emit launched(exePath);
    }
}

void Launcher::launchEntry(const QVariantMap &app) {
    QString exePath = app["exePath"].toString();
    QString opts = app["launchOptions"].toString();
    bool logging = app["enableLogging"].toBool();
    QString name = app["name"].toString();

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

    if (app["runtimeType"].toString() == "proton") {
        QString prefix = app["protonPrefix"].toString();
        if (!prefix.isEmpty())
            QDir().mkpath(prefix);
        env.insert("STEAM_COMPAT_CLIENT_INSTALL_PATH", QDir::homePath() + "/.steam/steam");
        env.insert("STEAM_COMPAT_DATA_PATH", prefix);
        launch(app["protonPath"].toString() + "/proton", {"run"},
               exePath, env, opts, logging, name);
    } else {
        QString prefix = app["winePrefix"].toString();
        if (!prefix.isEmpty()) {
            QDir().mkpath(prefix);
            env.insert("WINEPREFIX", prefix);
        }
        launch(app["wineBinary"].toString(), {},
               exePath, env, opts, logging, name);
    }
}

void Launcher::runInPrefix(const QVariantMap &app, const QString &exePath) {
    QVariantMap copy = app;
    copy["exePath"] = exePath;
    launchEntry(copy);
}

void Launcher::setupLogging(QProcess *proc, const QString &name) {
    QString safeName = name;
    safeName.replace(QRegularExpression("[^a-zA-Z0-9_-]"), "_");
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd_HH-mm-ss");
    QString logPath = m_logDir + "/" + safeName + "_" + timestamp + ".log";

    auto *logFile = new QFile(logPath, proc);
    if (!logFile->open(QIODevice::WriteOnly | QIODevice::Text)) {
        delete logFile;
        return;
    }

    logFile->write(QString("=== Vermouth log: %1 ===\n").arg(name).toUtf8());
    logFile->write(QString("=== Started: %1 ===\n\n")
                       .arg(QDateTime::currentDateTime().toString()).toUtf8());

    connect(proc, &QProcess::readyReadStandardOutput, proc, [proc, logFile]() {
        logFile->write(proc->readAllStandardOutput());
        logFile->flush();
    });

    connect(proc, &QProcess::readyReadStandardError, proc, [proc, logFile]() {
        logFile->write("[stderr] ");
        logFile->write(proc->readAllStandardError());
        logFile->flush();
    });

    connect(proc, &QProcess::finished, proc, [logFile](int exitCode) {
        logFile->write(QString("\n=== Exited with code %1 ===\n").arg(exitCode).toUtf8());
        logFile->close();
    });
}
