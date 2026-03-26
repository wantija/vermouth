#include "appentry.h"

QJsonObject AppEntry::toJson() const {
    QJsonObject obj;
    obj["name"] = name;
    obj["exePath"] = exePath;
    obj["runtimeType"] = (runtimeType == Proton) ? "proton" : "wine";
    obj["protonPath"] = protonPath;
    obj["protonPrefix"] = protonPrefix;
    obj["wineBinary"] = wineBinary;
    obj["winePrefix"] = winePrefix;
    obj["iconPath"] = iconPath;
    obj["launchOptions"] = launchOptions;
    obj["enableLogging"] = enableLogging;
    return obj;
}

AppEntry AppEntry::fromJson(const QJsonObject &obj) {
    AppEntry e;
    e.name = obj["name"].toString();
    e.exePath = obj["exePath"].toString();
    e.runtimeType = (obj["runtimeType"].toString() == "proton") ? Proton : Wine;
    e.protonPath = obj["protonPath"].toString();
    e.protonPrefix = obj["protonPrefix"].toString();
    e.wineBinary = obj["wineBinary"].toString();
    e.winePrefix = obj["winePrefix"].toString();
    e.iconPath = obj["iconPath"].toString();
    e.launchOptions = obj["launchOptions"].toString();
    e.enableLogging = obj["enableLogging"].toBool(false);
    return e;
}
