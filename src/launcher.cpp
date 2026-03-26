#include "launcher.h"
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QProcessEnvironment>
#include <QRegularExpression>
#include <QStandardPaths>

Launcher::Launcher(QObject *parent)
    : QObject(parent)
{
    m_logDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/logs";
    QDir().mkpath(m_logDir);
}

static QString shellQuote(const QString &s) {
    QString quoted = s;
    quoted.replace("'", "'\\''");
    return "'" + quoted + "'";
}

QString Launcher::logDir() const {
    return m_logDir;
}

void Launcher::setupLogging(QProcess *proc, const QString &name) {
    QString safeName = name;
    safeName.replace(QRegularExpression("[^a-zA-Z0-9_-]"), "_");
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd_HH-mm-ss");
    QString logPath = m_logDir + "/" + safeName + "_" + timestamp + ".log";

    QFile *logFile = new QFile(logPath, proc);
    if (!logFile->open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        delete logFile;
        return;
    }

    // Write header
    logFile->write(QString("=== Vermouth log: %1 ===\n").arg(name).toUtf8());
    logFile->write(QString("=== Started: %1 ===\n\n").arg(QDateTime::currentDateTime().toString()).toUtf8());

    connect(proc, &QProcess::readyReadStandardOutput, proc, [proc, logFile]() {
        QByteArray data = proc->readAllStandardOutput();
        logFile->write(data);
        logFile->flush();
    });

    connect(proc, &QProcess::readyReadStandardError, proc, [proc, logFile]() {
        QByteArray data = proc->readAllStandardError();
        logFile->write("[stderr] ");
        logFile->write(data);
        logFile->flush();
    });

    connect(proc, &QProcess::finished, proc, [logFile](int exitCode) {
        logFile->write(QString("\n=== Exited with code %1 ===\n").arg(exitCode).toUtf8());
        logFile->close();
    });
}

void Launcher::launchProton(const QString &protonPath, const QString &prefix,
                            const QString &exePath, const QStringList &args,
                            const QString &launchOptions,
                            bool enableLogging, const QString &logName) {
    auto *proc = new QProcess(this);
    connect(proc, &QProcess::finished, proc, &QProcess::deleteLater);

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("STEAM_COMPAT_CLIENT_INSTALL_PATH",
               QDir::homePath() + "/.steam/steam");
    // Ensure prefix directory exists -- Proton writes pfx.lock inside it
    if (!prefix.isEmpty())
        QDir().mkpath(prefix);
    env.insert("STEAM_COMPAT_DATA_PATH", prefix);
    proc->setProcessEnvironment(env);

    proc->setWorkingDirectory(QFileInfo(exePath).absolutePath());

    if (enableLogging) {
        proc->setProcessChannelMode(QProcess::SeparateChannels);
        setupLogging(proc, logName.isEmpty() ? QFileInfo(exePath).baseName() : logName);
    }

    QString protonBin = protonPath + "/proton";

    if (!launchOptions.trimmed().isEmpty()) {
        QString baseCmd = shellQuote(protonBin) + " run " + shellQuote(exePath);
        for (const auto &a : args)
            baseCmd += " " + shellQuote(a);

        QString fullCmd;
        QString opts = launchOptions.trimmed();
        if (opts.contains("%command%")) {
            fullCmd = opts;
            fullCmd.replace("%command%", baseCmd);
        } else {
            fullCmd = opts + " " + baseCmd;
        }

        proc->start("/bin/sh", {"-c", fullCmd});
    } else {
        QStringList fullArgs = {"run", exePath};
        fullArgs.append(args);
        proc->start(protonBin, fullArgs);
    }

    if (!proc->waitForStarted(5000)) {
        emit launchError(exePath, proc->errorString());
        proc->deleteLater();
    } else {
        emit launched(exePath);
    }
}

void Launcher::launchWine(const QString &wineBinary, const QString &prefix,
                          const QString &exePath, const QStringList &args,
                          const QString &launchOptions,
                          bool enableLogging, const QString &logName) {
    auto *proc = new QProcess(this);
    connect(proc, &QProcess::finished, proc, &QProcess::deleteLater);

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    if (!prefix.isEmpty()) {
        QDir().mkpath(prefix);
        env.insert("WINEPREFIX", prefix);
    }
    proc->setProcessEnvironment(env);

    proc->setWorkingDirectory(QFileInfo(exePath).absolutePath());

    if (enableLogging) {
        proc->setProcessChannelMode(QProcess::SeparateChannels);
        setupLogging(proc, logName.isEmpty() ? QFileInfo(exePath).baseName() : logName);
    }

    if (!launchOptions.trimmed().isEmpty()) {
        QString baseCmd = shellQuote(wineBinary) + " " + shellQuote(exePath);
        for (const auto &a : args)
            baseCmd += " " + shellQuote(a);

        QString fullCmd;
        QString opts = launchOptions.trimmed();
        if (opts.contains("%command%")) {
            fullCmd = opts;
            fullCmd.replace("%command%", baseCmd);
        } else {
            fullCmd = opts + " " + baseCmd;
        }

        proc->start("/bin/sh", {"-c", fullCmd});
    } else {
        QStringList fullArgs = {exePath};
        fullArgs.append(args);
        proc->start(wineBinary, fullArgs);
    }

    if (!proc->waitForStarted(5000)) {
        emit launchError(exePath, proc->errorString());
        proc->deleteLater();
    } else {
        emit launched(exePath);
    }
}

void Launcher::launchEntry(const QVariantMap &app) {
    QString runtimeType = app["runtimeType"].toString();
    QString opts = app["launchOptions"].toString();
    bool logging = app["enableLogging"].toBool();
    QString name = app["name"].toString();
    if (runtimeType == "proton") {
        launchProton(app["protonPath"].toString(),
                     app["protonPrefix"].toString(),
                     app["exePath"].toString(), {}, opts, logging, name);
    } else {
        launchWine(app["wineBinary"].toString(),
                   app["winePrefix"].toString(),
                   app["exePath"].toString(), {}, opts, logging, name);
    }
}

void Launcher::runInPrefix(const QVariantMap &app, const QString &exePath) {
    QString runtimeType = app["runtimeType"].toString();
    QString opts = app["launchOptions"].toString();
    bool logging = app["enableLogging"].toBool();
    QString name = app["name"].toString();
    if (runtimeType == "proton") {
        launchProton(app["protonPath"].toString(),
                     app["protonPrefix"].toString(),
                     exePath, {}, opts, logging, name);
    } else {
        launchWine(app["wineBinary"].toString(),
                   app["winePrefix"].toString(),
                   exePath, {}, opts, logging, name);
    }
}
