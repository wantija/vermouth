#include "desktopfilewriter.h"
#include <QDir>
#include <QFile>
#include <QRegularExpression>
#include <QTextStream>
#include <QCoreApplication>
#include <QStandardPaths>

DesktopFileWriter::DesktopFileWriter(QObject *parent)
    : QObject(parent)
{}

QString DesktopFileWriter::safeName(const QString &name) const {
    QString safe = name;
    safe.replace(QRegularExpression("[^a-zA-Z0-9_-]"), "_");
    return safe;
}

bool DesktopFileWriter::writeDesktopFile(const QString &filePath, const QVariantMap &app) {
    QString name = app["name"].toString();
    QString vermouthBin = QCoreApplication::applicationFilePath();
    QString runtimeType = app["runtimeType"].toString();

    QString exec;
    if (runtimeType == "proton") {
        exec = QString("%1 --launch-proton \"%2\" --proton \"%3\" --prefix \"%4\"")
            .arg(vermouthBin, app["exePath"].toString(),
                 app["protonPath"].toString(), app["protonPrefix"].toString());
    } else {
        exec = QString("%1 --launch-wine \"%2\" --wine \"%3\" --prefix \"%4\"")
            .arg(vermouthBin, app["exePath"].toString(),
                 app["wineBinary"].toString(), app["winePrefix"].toString());
    }

    QFile f(filePath);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    QTextStream out(&f);
    out << "[Desktop Entry]\n";
    out << "Type=Application\n";
    out << "Name=" << name << "\n";
    out << "Exec=" << exec << "\n";
    out << "Terminal=false\n";
    out << "Categories=Game;\n";
    if (!app["iconPath"].toString().isEmpty())
        out << "Icon=" << app["iconPath"].toString() << "\n";
    out << "Comment=Launched via Vermouth\n";

    f.close();
    // Make executable
    f.setPermissions(f.permissions() | QFile::ExeUser);
    return true;
}

bool DesktopFileWriter::createStartMenuEntry(const QVariantMap &app) {
    QString dir = QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation);
    QDir().mkpath(dir);
    QString filePath = dir + "/vermouth-" + safeName(app["name"].toString()) + ".desktop";
    return writeDesktopFile(filePath, app);
}

bool DesktopFileWriter::createDesktopShortcut(const QVariantMap &app) {
    QString dir = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    if (dir.isEmpty())
        dir = QDir::homePath() + "/Desktop";
    QDir().mkpath(dir);
    QString filePath = dir + "/vermouth-" + safeName(app["name"].toString()) + ".desktop";
    return writeDesktopFile(filePath, app);
}

bool DesktopFileWriter::removeStartMenuEntry(const QString &name) {
    QString dir = QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation);
    return QFile::remove(dir + "/vermouth-" + safeName(name) + ".desktop");
}
