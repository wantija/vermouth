#include "appmodel.h"
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>

AppModel::AppModel(QObject *parent)
    : QAbstractListModel(parent)
{
    load();
}

int AppModel::rowCount(const QModelIndex &) const {
    return m_entries.size();
}

QVariant AppModel::data(const QModelIndex &index, int role) const {
    if (index.row() < 0 || index.row() >= m_entries.size())
        return {};

    const auto &e = m_entries[index.row()];
    switch (role) {
    case NameRole:         return e.name;
    case ExePathRole:      return e.exePath;
    case RuntimeTypeRole:  return (e.runtimeType == AppEntry::Proton) ? "proton" : "wine";
    case ProtonPathRole:   return e.protonPath;
    case ProtonPrefixRole: return e.protonPrefix;
    case WineBinaryRole:   return e.wineBinary;
    case WinePrefixRole:   return e.winePrefix;
    case IconPathRole:     return e.iconPath;
    case LaunchOptionsRole: return e.launchOptions;
    case EnableLoggingRole: return e.enableLogging;
    }
    return {};
}

QHash<int, QByteArray> AppModel::roleNames() const {
    return {
        {NameRole,         "name"},
        {ExePathRole,      "exePath"},
        {RuntimeTypeRole,  "runtimeType"},
        {ProtonPathRole,   "protonPath"},
        {ProtonPrefixRole, "protonPrefix"},
        {WineBinaryRole,   "wineBinary"},
        {WinePrefixRole,   "winePrefix"},
        {IconPathRole,     "iconPath"},
        {LaunchOptionsRole, "launchOptions"},
        {EnableLoggingRole, "enableLogging"},
    };
}

void AppModel::addApp(const QString &name, const QString &exePath,
                      const QString &runtimeType,
                      const QString &protonPath, const QString &protonPrefix,
                      const QString &wineBinary, const QString &winePrefix,
                      const QString &iconPath,
                      const QString &launchOptions,
                      bool enableLogging) {
    AppEntry e;
    e.name = name;
    e.exePath = exePath;
    e.runtimeType = (runtimeType == "proton") ? AppEntry::Proton : AppEntry::Wine;
    e.protonPath = protonPath;
    e.protonPrefix = protonPrefix;
    e.wineBinary = wineBinary;
    e.winePrefix = winePrefix;
    e.iconPath = iconPath;
    e.launchOptions = launchOptions;
    e.enableLogging = enableLogging;

    beginInsertRows(QModelIndex(), m_entries.size(), m_entries.size());
    m_entries.append(e);
    endInsertRows();
    emit countChanged();
    save();
}

void AppModel::removeApp(int index) {
    if (index < 0 || index >= m_entries.size()) return;
    beginRemoveRows(QModelIndex(), index, index);
    m_entries.removeAt(index);
    endRemoveRows();
    emit countChanged();
    save();
}

void AppModel::editApp(int index,
                       const QString &name, const QString &exePath,
                       const QString &runtimeType,
                       const QString &protonPath, const QString &protonPrefix,
                       const QString &wineBinary, const QString &winePrefix,
                       const QString &iconPath,
                       const QString &launchOptions,
                       bool enableLogging) {
    if (index < 0 || index >= m_entries.size()) return;
    auto &e = m_entries[index];
    e.name = name;
    e.exePath = exePath;
    e.runtimeType = (runtimeType == "proton") ? AppEntry::Proton : AppEntry::Wine;
    e.protonPath = protonPath;
    e.protonPrefix = protonPrefix;
    e.wineBinary = wineBinary;
    e.winePrefix = winePrefix;
    e.iconPath = iconPath;
    e.launchOptions = launchOptions;
    e.enableLogging = enableLogging;
    emit dataChanged(createIndex(index, 0), createIndex(index, 0));
    save();
}

QVariantMap AppModel::getApp(int index) const {
    if (index < 0 || index >= m_entries.size()) return {};
    const auto &e = m_entries[index];
    return {
        {"name", e.name},
        {"exePath", e.exePath},
        {"runtimeType", (e.runtimeType == AppEntry::Proton) ? "proton" : "wine"},
        {"protonPath", e.protonPath},
        {"protonPrefix", e.protonPrefix},
        {"wineBinary", e.wineBinary},
        {"winePrefix", e.winePrefix},
        {"iconPath", e.iconPath},
        {"launchOptions", e.launchOptions},
        {"enableLogging", e.enableLogging},
    };
}

QString AppModel::configPath() const {
    QString dir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QDir().mkpath(dir);
    return dir + "/apps.json";
}

void AppModel::load() {
    QFile f(configPath());
    if (!f.open(QIODevice::ReadOnly)) return;
    auto doc = QJsonDocument::fromJson(f.readAll());
    if (!doc.isArray()) return;

    beginResetModel();
    m_entries.clear();
    for (const auto &val : doc.array())
        m_entries.append(AppEntry::fromJson(val.toObject()));
    endResetModel();
    emit countChanged();
}

void AppModel::save() const {
    QJsonArray arr;
    for (const auto &e : m_entries)
        arr.append(e.toJson());

    QFile f(configPath());
    if (!f.open(QIODevice::WriteOnly)) return;
    f.write(QJsonDocument(arr).toJson());
}
