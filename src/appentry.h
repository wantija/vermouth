#pragma once

#include <QObject>
#include <QString>
#include <QJsonObject>

class AppEntry {
    Q_GADGET
public:
    enum RuntimeType { Proton, Wine };

    QString name;
    QString exePath;
    RuntimeType runtimeType = Proton;

    // Proton fields
    QString protonPath;    // e.g. ~/.steam/steam/compatibilitytools.d/GE-Proton9-1
    QString protonPrefix;  // STEAM_COMPAT_DATA_PATH

    // Wine fields
    QString wineBinary;    // path to wine binary
    QString winePrefix;    // WINEPREFIX

    QString iconPath;
    QString launchOptions;
    bool enableLogging = false;

    QJsonObject toJson() const;
    static AppEntry fromJson(const QJsonObject &obj);
};
