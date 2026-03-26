#pragma once

#include <QObject>
#include <QString>

class IconExtractor : public QObject {
    Q_OBJECT

public:
    explicit IconExtractor(QObject *parent = nullptr);

    // Extracts the icon from an exe, saves as PNG, returns the path (empty on failure)
    Q_INVOKABLE QString extractIcon(const QString &exePath);

private:
    QString cacheDir() const;
    QString tryWrestool(const QString &exePath, const QString &outPath);
};
