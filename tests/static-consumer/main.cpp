#include <QGuiApplication>
#include <QQmlComponent>
#include <QQmlEngine>
#include <memory>

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);
    QQmlEngine engine;
    QQmlComponent component(&engine);
    component.setData("import QtQml\nimport Qavsi.Icons.Material\nQtObject { property bool ok: MaterialSymbols.ready && MaterialSymbols.contains('settings') }", QUrl());
    std::unique_ptr<QObject> object(component.create());
    if (!object) qWarning().noquote() << component.errorString();
    return object && object->property("ok").toBool() ? 0 : 1;
}
