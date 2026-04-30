#pragma once

#include <QJsonObject>
#include <QObject>
#include <QString>
#include <QUuid>
#include <QVariantMap>

class AppEntry
{
    Q_GADGET
public:
    enum RuntimeType {
        Proton,
        Wine,
        Native
    };

    QString id;
    QString name;
    QString exePath;
    RuntimeType runtimeType = Proton;

    QString protonPath;
    QString protonPrefix;

    QString wineBinary;
    QString winePrefix;

    QString iconPath;
    QString gridPath;
    QString heroPath;
    QString logoPath;
    int steamGridDbId = 0;
    QString launchOptions;
    bool enableLogging = false;

    QJsonObject toJson() const;
    QVariantMap toVariantMap() const;
    static AppEntry fromJson(const QJsonObject &obj);
};
