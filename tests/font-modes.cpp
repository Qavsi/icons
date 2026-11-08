#include <QGuiApplication>
#include <QFont>
#include <QQmlComponent>
#include <QQmlEngine>
#include <QQuickWindow>
#include <QTest>
#include <QDebug>
#include <memory>

static int warnings = 0;
static int fallbackWarnings = 0;
static int missingWarnings = 0;
static void handler(QtMsgType type, const QMessageLogContext &, const QString &message)
{
    if (type == QtWarningMsg && message.contains("material:settings")) {
        ++warnings;
        if (message.contains("variable font unavailable")) ++fallbackWarnings;
        if (message.contains("icon unavailable")) ++missingWarnings;
    }
    fprintf(stderr, "%s\n", qPrintable(message));
}
int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);
    qInstallMessageHandler(handler);
    QQmlEngine engine;
    engine.addImportPath(QStringLiteral(QAVSI_ICONS_TEST_IMPORT_PATH));
    QQmlComponent component(&engine);
    component.setData(R"(
import QtQuick
import Qavsi.Icons
import Qavsi.Icons.Material
Window {
    width: 64; height: 64; visible: true
    function warnMissing() {
        IconRegistry.warnOnce("material:settings", "icon unavailable; using source or missing fallback")
        IconRegistry.warnOnce("material:settings", "icon unavailable; using source or missing fallback")
    }
    AppIcon { objectName: "icon"; name: "material:settings"; size: 48; color: "black";
        appearance: QtObject { property var axes: ({ FILL: 0.5 }) } }
    AppIcon { objectName: "other"; name: "material:home"; size: 48; color: "black" }
})", QUrl());
    std::unique_ptr<QObject> window(component.create());
    if (!window) { qWarning().noquote() << component.errorString(); return 1; }
    auto icon = window->findChild<QObject *>("icon");
    auto other = window->findChild<QObject *>("other");
#ifdef MODE_nofonts
    const bool expected = QTest::qWaitFor([&] { return icon->property("missing").toBool() && warnings == 1; }, 2000);
#else
    const bool expected = QTest::qWaitFor([&] { return !icon->property("missing").toBool() && icon->property("status").toInt() == 1; }, 2000);
#endif
    QTest::qWait(100);
    auto text = icon->findChild<QObject *>("qavsi.material.glyph");
    const auto image = qobject_cast<QQuickWindow *>(window.get())->grabWindow();
    if (!expected || image.isNull()) return 2;
#if defined(MODE_subset) || defined(MODE_fallback)
    if (!other->property("missing").toBool()) return 3;
#elif defined(MODE_all)
    if (other->property("missing").toBool()) return 4;
#endif
#ifdef MODE_subset
    if (!text || text->property("font").value<QFont>().variableAxisValue("FILL") != 0.5 || warnings != 0) return 5;
#elif defined(MODE_fallback) || defined(MODE_all)
    if (!text || text->property("font").value<QFont>().variableAxisValue("FILL") != 1 || warnings != 1) return 6;
    icon->setProperty("filled", true);
    QTest::qWait(100);
    if (warnings != 1) return 7;
    QMetaObject::invokeMethod(window.get(), "warnMissing");
    if (fallbackWarnings != 1 || missingWarnings != 1) return 8;
#endif
    return 0;
}
