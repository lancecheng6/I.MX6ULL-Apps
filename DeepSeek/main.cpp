#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QFile>
#include <QTextStream>
#include <QCoreApplication>
#include "DeepSeekclient.h"
#include "systemuicommonapiclient.h"

static QString readApiKey(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return QString();
    QTextStream in(&file);
    while (!in.atEnd()) {
        const QString line = in.readLine().trimmed();
        if (line.startsWith("API_KEY="))
            return line.mid(8).trimmed();
    }
    return QString();
}

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    qputenv("QT_QUICK_BACKEND", "");
    qputenv("QT_IM_MODULE", "qtvirtualkeyboard");
    QGuiApplication app(argc, argv);

    ChatGPTClient *client = new ChatGPTClient(&app);
    const QString apiKey = readApiKey(QCoreApplication::applicationDirPath() + "/.env");
    client->setApiKey(apiKey);

    qmlRegisterType<SystemUICommonApiClient>("com.alientek.qmlcomponents", 1, 0, "SystemUICommonApiClient");

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("appCurrtentDir", QCoreApplication::applicationDirPath());
    engine.rootContext()->setContextProperty("chatClient", client);
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreated,
        &app,
        [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
