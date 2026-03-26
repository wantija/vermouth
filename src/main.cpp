#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QCommandLineParser>
#include <QQuickStyle>
#include <QIcon>
#include "appmodel.h"
#include "protonscanner.h"
#include "launcher.h"
#include "desktopfilewriter.h"
#include "iconextractor.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    if (QQuickStyle::name().isEmpty())
        QQuickStyle::setStyle("Fusion");
    app.setApplicationName(QString(APP_NAME_STRING));
    app.setApplicationVersion(QString(APP_VERSION_STRING));
    app.setWindowIcon(QIcon(":/icons/vermouth.svg"));

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption launchProtonOpt("launch-proton", "Launch exe with Proton", "exe");
    QCommandLineOption launchWineOpt("launch-wine", "Launch exe with Wine", "exe");
    QCommandLineOption protonOpt("proton", "Proton path", "path");
    QCommandLineOption wineOpt("wine", "Wine binary path", "path");
    QCommandLineOption prefixOpt("prefix", "Prefix path", "path");

    parser.addOption(launchProtonOpt);
    parser.addOption(launchWineOpt);
    parser.addOption(protonOpt);
    parser.addOption(wineOpt);
    parser.addOption(prefixOpt);
    parser.process(app);

    Launcher launcher;

    // Direct launch mode - no GUI
    if (parser.isSet(launchProtonOpt)) {
        QVariantMap entry;
        entry["runtimeType"] = "proton";
        entry["protonPath"] = parser.value(protonOpt);
        entry["protonPrefix"] = parser.value(prefixOpt);
        entry["exePath"] = parser.value(launchProtonOpt);
        launcher.launchEntry(entry);
        return 0;
    }
    if (parser.isSet(launchWineOpt)) {
        QVariantMap entry;
        entry["runtimeType"] = "wine";
        entry["wineBinary"] = parser.value(wineOpt);
        entry["winePrefix"] = parser.value(prefixOpt);
        entry["exePath"] = parser.value(launchWineOpt);
        launcher.launchEntry(entry);
        return 0;
    }

    AppModel appModel;
    ProtonScanner protonScanner;
    DesktopFileWriter desktopWriter;
    IconExtractor iconExtractor;

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("appModel", &appModel);
    engine.rootContext()->setContextProperty("protonScanner", &protonScanner);
    engine.rootContext()->setContextProperty("launcher", &launcher);
    engine.rootContext()->setContextProperty("desktopWriter", &desktopWriter);
    engine.rootContext()->setContextProperty("iconExtractor", &iconExtractor);

    engine.loadFromModule("Vermouth", "Main");

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
