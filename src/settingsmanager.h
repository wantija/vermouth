#pragma once

#include <QObject>
#include <QSettings>
#include <QStringList>

class SettingsManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString defaultPrefixDir READ defaultPrefixDir WRITE setDefaultPrefixDir NOTIFY defaultPrefixDirChanged)
    Q_PROPERTY(QString defaultGamePrefix READ defaultGamePrefix WRITE setDefaultGamePrefix NOTIFY defaultGamePrefixChanged)
    Q_PROPERTY(QStringList extraProtonPaths READ extraProtonPaths WRITE setExtraProtonPaths NOTIFY extraProtonPathsChanged)
    Q_PROPERTY(QString defaultRuntimeType READ defaultRuntimeType WRITE setDefaultRuntimeType NOTIFY defaultRuntimeChanged)
    Q_PROPERTY(QString defaultProtonPath READ defaultProtonPath WRITE setDefaultProtonPath NOTIFY defaultRuntimeChanged)
    Q_PROPERTY(QString defaultWineBinary READ defaultWineBinary WRITE setDefaultWineBinary NOTIFY defaultRuntimeChanged)
    Q_PROPERTY(bool drawerPinned READ drawerPinned WRITE setDrawerPinned NOTIFY drawerPinnedChanged)
    Q_PROPERTY(QString umuPath READ umuPath WRITE setUmuPath NOTIFY umuPathChanged)
    Q_PROPERTY(QStringList globalEnvVars READ globalEnvVars WRITE setGlobalEnvVars NOTIFY globalEnvVarsChanged)
    Q_PROPERTY(bool lightsOut READ lightsOut WRITE setLightsOut NOTIFY lightsOutChanged)
    Q_PROPERTY(QString lightsOutColor READ lightsOutColor WRITE setLightsOutColor NOTIFY lightsOutColorChanged)

public:
    explicit SettingsManager(QObject *parent = nullptr);

    QString defaultPrefixDir() const;
    Q_INVOKABLE void setDefaultPrefixDir(const QString &path);

    QString defaultGamePrefix() const;
    Q_INVOKABLE void setDefaultGamePrefix(const QString &path);

    QStringList extraProtonPaths() const;
    void setExtraProtonPaths(const QStringList &paths);

    Q_INVOKABLE void addExtraProtonPath(const QString &path);
    Q_INVOKABLE void removeExtraProtonPath(int index);

    QString defaultRuntimeType() const;
    Q_INVOKABLE void setDefaultRuntimeType(const QString &type);

    QString defaultProtonPath() const;
    Q_INVOKABLE void setDefaultProtonPath(const QString &path);

    QString defaultWineBinary() const;
    Q_INVOKABLE void setDefaultWineBinary(const QString &path);

    bool drawerPinned() const;
    Q_INVOKABLE void setDrawerPinned(bool pinned);

    QString umuPath() const;
    Q_INVOKABLE void setUmuPath(const QString &path);

    QStringList globalEnvVars() const;
    Q_INVOKABLE void setGlobalEnvVars(const QStringList &vars);

    bool lightsOut() const;
    Q_INVOKABLE void setLightsOut(bool enabled);

    QString lightsOutColor() const;
    Q_INVOKABLE void setLightsOutColor(const QString &color);

Q_SIGNALS:
    void defaultPrefixDirChanged();
    void defaultGamePrefixChanged();
    void extraProtonPathsChanged();
    void defaultRuntimeChanged();
    void drawerPinnedChanged();
    void umuPathChanged();
    void globalEnvVarsChanged();
    void lightsOutChanged();
    void lightsOutColorChanged();

private:
    QSettings m_settings;
};
