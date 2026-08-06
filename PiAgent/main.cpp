#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QCoreApplication>
#include "PiAgentclient.h"
#include "systemuicommonapiclient.h"

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    qputenv("QT_QUICK_BACKEND", "");
    qputenv("QT_IM_MODULE", "qtvirtualkeyboard");
    QGuiApplication app(argc, argv);

    PiAgentclient *client = new PiAgentclient(&app);

    qmlRegisterType<SystemUICommonApiClient>("com.alientek.qmlcomponents", 1, 0, "SystemUICommonApiClient");

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("appCurrtentDir", QCoreApplication::applicationDirPath());
    engine.rootContext()->setContextProperty("piClient", client);

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
