#include "iconextractor.h"
#include <QCryptographicHash>
#include <QDir>
#include <QFileInfo>
#include <QProcess>
#include <QStandardPaths>
#include <QRegularExpression>
#include <QTemporaryFile>

IconExtractor::IconExtractor(QObject *parent)
    : QObject(parent)
{}

QString IconExtractor::cacheDir() const {
    QString dir = QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/icons";
    QDir().mkpath(dir);
    return dir;
}

QString IconExtractor::extractIcon(const QString &exePath) {
    if (!QFileInfo::exists(exePath))
        return {};

    // Use hash of exe path as cache key
    QByteArray hash = QCryptographicHash::hash(exePath.toUtf8(), QCryptographicHash::Md5).toHex();
    QString outPath = cacheDir() + "/" + QString::fromLatin1(hash) + ".png";

    if (QFileInfo::exists(outPath))
        return outPath;

    QString result = tryWrestool(exePath, outPath);
    if (!result.isEmpty())
        return result;

    return {};
}

QString IconExtractor::tryWrestool(const QString &exePath, const QString &outPath) {
    // Check if wrestool and icotool are available
    QString wrestool = QStandardPaths::findExecutable("wrestool");
    QString icotool = QStandardPaths::findExecutable("icotool");
    if (wrestool.isEmpty() || icotool.isEmpty())
        return {};

    // Extract .ico from the exe
    QTemporaryFile icoFile;
    icoFile.setFileTemplate(QDir::tempPath() + "/vermouth_XXXXXX.ico");
    if (!icoFile.open())
        return {};
    icoFile.close();

    QProcess wrestoolProc;
    wrestoolProc.start(wrestool, {"--extract", "--type=14", "--output=" + icoFile.fileName(), exePath});
    if (!wrestoolProc.waitForFinished(10000) || wrestoolProc.exitCode() != 0)
        return {};

    if (QFileInfo(icoFile.fileName()).size() == 0)
        return {};

    // Convert .ico to .png, picking the largest resolution
    QProcess icotoolProc;
    icotoolProc.start(icotool, {"--list", icoFile.fileName()});
    if (!icotoolProc.waitForFinished(5000))
        return {};

    // Parse icotool --list output to find the largest icon
    QString listOutput = QString::fromUtf8(icotoolProc.readAllStandardOutput());
    int bestSize = 0;
    int bestIndex = 1;
    int currentIndex = 0;
    for (const auto &line : listOutput.split('\n')) {
        currentIndex++;
        // Lines look like: --icon --index=1 --width=256 --height=256 ...
        QRegularExpression rx("--width=(\\d+)");
        auto match = rx.match(line);
        if (match.hasMatch()) {
            int w = match.captured(1).toInt();
            if (w > bestSize) {
                bestSize = w;
                QRegularExpression idxRx("--index=(\\d+)");
                auto idxMatch = idxRx.match(line);
                if (idxMatch.hasMatch())
                    bestIndex = idxMatch.captured(1).toInt();
            }
        }
    }

    QProcess convertProc;
    convertProc.start(icotool, {"--extract", "--index=" + QString::number(bestIndex),
                                "--output=" + outPath, icoFile.fileName()});
    if (!convertProc.waitForFinished(5000) || convertProc.exitCode() != 0) {
        // Fallback: extract without index selection
        QProcess fallback;
        fallback.start(icotool, {"--extract", "--output=" + outPath, icoFile.fileName()});
        fallback.waitForFinished(5000);
    }

    if (QFileInfo::exists(outPath) && QFileInfo(outPath).size() > 0)
        return outPath;
    return {};
}
