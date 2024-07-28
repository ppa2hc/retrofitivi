#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "digitalauto/digitalauto.hpp"
#include "aiassist/aiassist.hpp"
#include <QQmlContext>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    qmlRegisterType<DigitalAutoAppAsync>("DigitalAutoAppAsync", 1, 0, "DigitalAutoAppAsync");
    qmlRegisterType<AiassistAsync>("AiassistAsync", 1, 0, "AiassistAsync");

    QQmlApplicationEngine engine;
    const QUrl url(u"qrc:/untitled2/Main.qml"_qs);
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
        &app, [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        }, Qt::QueuedConnection);
    engine.load(url);

     // Get the root object (main window) and set it as a context property
    QObject *rootObject = engine.rootObjects().first();
    engine.rootContext()->setContextProperty("mainWindow", rootObject);

    int ret = app.exec();

    return ret;
}
