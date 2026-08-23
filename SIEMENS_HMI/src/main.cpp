#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include "systemuicommonapiclient.h"
#include "plcservice.h"

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    qputenv("QT_QUICK_BACKEND", "");
    qputenv("QT_IM_MODULE", "qtvirtualkeyboard");

    QGuiApplication app(argc, argv);
    app.setApplicationName("HeatingSCADA");

    qmlRegisterType<SystemUICommonApiClient>(
        "com.alientek.qmlcomponents", 1, 0, "SystemUICommonApiClient");

    PlcService plcService;

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("plc", &plcService);
    engine.rootContext()->setContextProperty(
        "appCurrtentDir", QCoreApplication::applicationDirPath());

    engine.load(QUrl(QStringLiteral("qrc:/Main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
