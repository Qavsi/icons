// Copyright (c) 2026 Enes Alp
// SPDX-License-Identifier: MIT
#include <QColor>
#include <QFile>
#include <QDir>
#include <QFontDatabase>
#include <QFont>
#include <QGuiApplication>
#include <QIcon>
#include <QImage>
#include <QPainter>
#include <QPointer>
#include <QQmlComponent>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickItem>
#include <QQuickWindow>
#include <QScreen>
#include <QSGRendererInterface>
#include <QRawFont>
#include <QTemporaryDir>
#include <QTest>
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <limits>
#include <memory>

namespace {
int assertions = 0;
int failures = 0;
QStringList warningMessages;
void messages(QtMsgType type, const QMessageLogContext &, const QString &message)
{
    if (type == QtWarningMsg)
        warningMessages.append(message);
    std::fprintf(stderr, "%s\n", message.toUtf8().constData());
}
void expect(bool value, const QString &message)
{
    ++assertions;
    if (!value) {
        ++failures;
        qCritical().noquote() << "FAIL:" << message;
    }
}
std::unique_ptr<QObject> create(QQmlEngine &engine, const QByteArray &source,
                               const QVariantMap &inputs = {},
                               const QUrl &baseUrl = QUrl(QStringLiteral("qrc:/qt/qml/Qavsi/Icons/Tests/Fixture.qml")))
{
    QQmlComponent component(&engine);
    component.setData(source, baseUrl);
    auto object = std::unique_ptr<QObject>(component.createWithInitialProperties(inputs));
    expect(object != nullptr, component.errorString());
    if (auto *window = qobject_cast<QQuickWindow *>(object.get())) {
        const auto screens = QGuiApplication::screens();
        if (!screens.isEmpty()) {
            auto *left = *std::min_element(screens.begin(), screens.end(), [](auto *a, auto *b) {
                return a->geometry().left() < b->geometry().left();
            });
            window->setPosition(left->availableGeometry().topLeft() + QPoint(8, 16));
        }
        window->show();
    }
    return object;
}
void prepare(QQmlEngine &engine)
{
    engine.addImportPath(QString::fromUtf8(QAVSI_ICONS_TEST_IMPORT_PATH));
    auto import = create(engine, "import QtQml\nimport Qavsi.Icons\nQtObject {}");
}
QObject *registry(QQmlEngine &engine)
{
    return engine.singletonInstance<QObject *>("Qavsi.Icons", "IconRegistry");
}
bool registerPackage(QObject *target, const QString &name, const QUrl &source)
{
    bool result = false;
    const bool invoked = QMetaObject::invokeMethod(target, "registerPackage", Q_RETURN_ARG(bool, result),
        Q_ARG(QString, name), Q_ARG(QUrl, source));
    expect(invoked, QStringLiteral("package registration is callable through the public Qt metaobject"));
    return result;
}
QVariantMap packages(QObject *registry)
{
    return registry->property("packages").toMap();
}
QByteArray fixture()
{
    return R"(
        import QtQuick
        import Qavsi.Icons
        Window {
            id: root
            width: 80; height: 80; visible: false; color: "transparent"
            property string iconName: ""
            property url fallbackSource: ""
            property color ink: "#2277cc"
            property bool iconFilled: false
            property real iconSize: 48
            property QtObject iconAppearance: null
            AppIcon {
                objectName: "subject"
                x: 16; y: 16
                size: root.iconSize
                color: root.ink
                name: root.iconName
                source: root.fallbackSource
                filled: root.iconFilled
                appearance: root.iconAppearance
            }
        })";
}
QObject *subject(QObject *root)
{
    return root->findChild<QObject *>(QStringLiteral("subject"));
}
bool status(QObject *icon, int wanted)
{
    return icon && QTest::qWaitFor([=] { return icon->property("status").toInt() == wanted; }, 1500);
}
QImage capture(QObject *root)
{
    auto *window = qobject_cast<QQuickWindow *>(root);
    QImage image;
    expect(window && QTest::qWaitFor([&] {
        window->requestUpdate();
        image = window->grabWindow();
        return !image.isNull();
    }, 1500), QStringLiteral("actual Qt Quick window produces an image"));
    return image;
}
int alphaPixels(const QImage &image)
{
    int count = 0;
    for (int y = 0; y < image.height(); ++y)
        for (int x = 0; x < image.width(); ++x)
            count += qAlpha(image.pixel(x, y)) > 32;
    return count;
}
int differentPixels(const QImage &a, const QImage &b)
{
    if (a.size() != b.size())
        return -1;
    int count = 0;
    for (int y = 0; y < a.height(); ++y)
        for (int x = 0; x < a.width(); ++x)
            count += a.pixel(x, y) != b.pixel(x, y);
    return count;
}
int matchingWarnings(const QString &name)
{
    int count = 0;
    for (const auto &message : warningMessages)
        count += message.contains(name);
    return count;
}
}

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);
    const auto previousHandler = qInstallMessageHandler(messages);
    QTemporaryDir assets;
    expect(assets.isValid(), QStringLiteral("local fallback assets exist"));
    QImage fallback(16, 16, QImage::Format_ARGB32_Premultiplied);
    fallback.fill(Qt::transparent);
    { QPainter paint(&fallback); paint.setPen(Qt::NoPen); paint.setBrush(Qt::white); paint.drawEllipse(1, 1, 14, 14); }
    const auto fallbackPath = assets.filePath(QStringLiteral("fallback.png"));
    expect(fallback.save(fallbackPath), QStringLiteral("fallback image is written locally"));

    const auto initialFamilies = QFontDatabase::families();
    QQmlEngine first;
    prepare(first);
    expect(QFontDatabase::families() == initialFamilies,
           QStringLiteral("Core import does not register a bundled font"));
    auto *firstRegistry = registry(first);
    expect(firstRegistry && packages(firstRegistry).isEmpty(), QStringLiteral("Core starts without font packages"));
    if (!firstRegistry)
        return 2;
    auto empty = create(first, fixture());
    expect(empty && status(subject(empty.get()), 0)
               && alphaPixels(capture(empty.get())) == 0,
           QStringLiteral("empty AppIcon is Null and paints nothing"));
    const QUrl testDelegate(QStringLiteral("qrc:/qt/qml/Qavsi/Icons/Tests/TestGlyph.qml"));
    expect(registerPackage(firstRegistry, QStringLiteral("test"), testDelegate), QStringLiteral("caller package registers"));
    expect(registerPackage(firstRegistry, QStringLiteral("test"), testDelegate), QStringLiteral("same package registration is idempotent"));
    const auto registered = packages(firstRegistry);
    expect(!registerPackage(firstRegistry, QStringLiteral("test"), QUrl(QStringLiteral("qrc:/other.qml")))
               && packages(firstRegistry) == registered,
           QStringLiteral("conflicting registration preserves the existing package"));
    expect(!registerPackage(firstRegistry, QStringLiteral("qt"), testDelegate)
               && !registerPackage(firstRegistry, QStringLiteral("bad:name"), testDelegate)
               && !registerPackage(firstRegistry, QString(), testDelegate),
           QStringLiteral("reserved and malformed package names are rejected"));

    auto square = create(first, fixture(), {{"iconName", "test:square"}});
    if (!square) return 2;
    auto *squareIcon = subject(square.get());
    expect(status(squareIcon, 1), QStringLiteral("registered renderer becomes ready"));
    auto squareImage = capture(square.get());
    auto *squareItem = qobject_cast<QQuickItem *>(squareIcon);
    expect(squareItem && squareItem->width() == 48 && squareItem->height() == 48,
           QStringLiteral("explicit caller icon size is preserved"));
    const auto dpr = qobject_cast<QQuickWindow *>(square.get())->devicePixelRatio();
    expect(squareImage.width() == qRound(80 * dpr) && alphaPixels(squareImage) >= qRound(48 * 48 * dpr * dpr * .9),
           QStringLiteral("software image follows device pixel ratio and exact icon bounds"));
    square->setProperty("ink", QColor(Qt::transparent));
    expect(alphaPixels(capture(square.get())) == 0, QStringLiteral("transparent caller color remains transparent"));
    square->setProperty("ink", QColor(Qt::green));
    const auto green = capture(square.get());
    expect(alphaPixels(green) > 0 && differentPixels(squareImage, green) > 20,
           QStringLiteral("theme color binding updates existing glyph pixels"));

    auto missing = create(first, fixture(), {{"iconName", "absent:unique_missing"}});
    auto repeated = create(first, fixture(), {{"iconName", "absent:unique_missing"}});
    if (!missing || !repeated) return 2;
    expect(status(subject(missing.get()), 3) && subject(missing.get())->property("missing").toBool(),
           QStringLiteral("missing package has explicit Error status"));
    expect(alphaPixels(capture(missing.get())) > 0, QStringLiteral("missing mark renders without a font provider"));
    expect(QTest::qWaitFor([&] { return matchingWarnings(QStringLiteral("absent:unique_missing")) == 1; }, 1000),
           QStringLiteral("the same missing name warns once per engine"));
    missing->setProperty("iconName", QStringLiteral("absent:another_missing"));
    expect(QTest::qWaitFor([&] { return matchingWarnings(QStringLiteral("absent:another_missing")) == 1; }, 1000),
           QStringLiteral("changing to another missing name emits that name's one warning even when Error status stays unchanged"));
    missing->setProperty("iconName", QStringLiteral("absent:unique_missing"));
    QCoreApplication::processEvents();
    expect(matchingWarnings(QStringLiteral("absent:unique_missing")) == 1,
           QStringLiteral("returning to a previously missing name does not warn again"));
    missing->setProperty("fallbackSource", QUrl::fromLocalFile(fallbackPath));
    expect(status(subject(missing.get()), 1), QStringLiteral("missing package can fall back to explicit source"));
    const auto fallbackImage = capture(missing.get());
    expect(alphaPixels(fallbackImage) > 0, QStringLiteral("explicit source fallback draws visible pixels"));
    missing->setProperty("ink", QColor(Qt::transparent));
    expect(alphaPixels(capture(missing.get())) == 0, QStringLiteral("native source fallback preserves transparent tint"));
    auto brokenGlyph = create(first, fixture(), {{"iconName", "test:missing"},
        {"fallbackSource", QUrl::fromLocalFile(fallbackPath)}});
    expect(brokenGlyph && status(subject(brokenGlyph.get()), 1), QStringLiteral("provider glyph failure uses source fallback"));

    const auto relativeDir = assets.filePath(QStringLiteral("relative"));
    expect(QDir().mkpath(relativeDir) && fallback.save(relativeDir + QStringLiteral("/relative.png")),
           QStringLiteral("relative source asset is outside the working directory"));
    auto relativeCode = fixture().replace("source: root.fallbackSource", "source: \"relative.png\"");
    auto relative = create(first, relativeCode, {}, QUrl::fromLocalFile(relativeDir + QStringLiteral("/Fixture.qml")));
    auto *relativeIcon = relative ? subject(relative.get()) : nullptr;
    auto *relativeNative = relativeIcon ? relativeIcon->findChild<QObject *>(QStringLiteral("qavsi.icons.native")) : nullptr;
    const bool relativeReady = status(relativeIcon, 1);
    qInfo() << "Relative source AppIcon:" << (relativeIcon ? relativeIcon->property("source") : QVariant())
            << "native:" << (relativeNative ? relativeNative->property("source") : QVariant())
            << "expected:" << QUrl::fromLocalFile(relativeDir + QStringLiteral("/relative.png"))
            << "ready:" << relativeReady
            << "context:" << (relativeIcon && qmlContext(relativeIcon) ? qmlContext(relativeIcon)->baseUrl() : QUrl())
            << "parentContext:" << (relativeIcon && qmlContext(relativeIcon) && qmlContext(relativeIcon)->parentContext() ? qmlContext(relativeIcon)->parentContext()->baseUrl() : QUrl());
    expect(relative && relativeReady && alphaPixels(capture(relative.get())) > 0,
           QStringLiteral("relative source resolves against the caller component URL, not process cwd or library QML"));

    const auto previousTheme = QIcon::themeName();
    const auto previousSearchPaths = QIcon::themeSearchPaths();
    const auto themeRoot = assets.filePath(QStringLiteral("themes"));
    const auto themePath = themeRoot + QStringLiteral("/qavsi-test");
    expect(QDir().mkpath(themePath + QStringLiteral("/48x48/actions")), QStringLiteral("isolated Qt icon theme exists"));
    QFile descriptor(themePath + QStringLiteral("/index.theme"));
    expect(descriptor.open(QIODevice::WriteOnly), QStringLiteral("test theme descriptor opens"));
    descriptor.write("[Icon Theme]\nName=Qavsi Test\nDirectories=48x48/actions\n[48x48/actions]\nSize=48\nType=Fixed\n");
    descriptor.close();
    expect(fallback.save(themePath + QStringLiteral("/48x48/actions/save.png")), QStringLiteral("test Qt theme icon exists"));
    QIcon::setThemeSearchPaths({themeRoot});
    QIcon::setThemeName(QStringLiteral("qavsi-test"));
    auto native = create(first, fixture(), {{"iconName", "qt:save"}});
    expect(native && status(subject(native.get()), 1) && alphaPixels(capture(native.get())) > 0,
           QStringLiteral("qt: package uses Qt's native icon theme lookup"));
    QIcon::setThemeName(previousTheme);
    QIcon::setThemeSearchPaths(previousSearchPaths);

    auto second = std::make_unique<QQmlEngine>();
    prepare(*second);
    QPointer<QObject> secondRegistry = registry(*second);
    expect(secondRegistry && secondRegistry != firstRegistry && packages(secondRegistry).isEmpty(),
           QStringLiteral("registries and packages belong to individual engines"));

#ifdef QAVSI_ICONS_TEST_MATERIAL
    auto late = create(first, fixture(), {{"iconName", "material:settings"},
        {"fallbackSource", QUrl::fromLocalFile(fallbackPath)}});
    if (!late) return 2;
    auto *lateIcon = subject(late.get());
    expect(status(lateIcon, 1), QStringLiteral("not-yet-imported Material uses source fallback"));
    const auto beforeImport = capture(late.get());
    auto materialImport = create(first, "import QtQml\nimport Qavsi.Icons.Material\nQtObject {}");
    expect(materialImport && packages(firstRegistry).contains(QStringLiteral("material")),
           QStringLiteral("importing Material installs its renderer in the importing engine"));
    expect(!packages(secondRegistry).contains(QStringLiteral("material")),
           QStringLiteral("Material import does not mutate another engine"));
    expect(status(lateIcon, 1) && QTest::qWaitFor([&] {
        return lateIcon->findChild<QObject *>(QStringLiteral("qavsi.material.glyph")) != nullptr;
    }, 1500), QStringLiteral("late provider registration reroutes an existing AppIcon"));
    const auto outline = capture(late.get());
    expect(differentPixels(beforeImport, outline) > 20, QStringLiteral("late registration changes fallback pixels to the requested glyph"));
    auto *text = lateIcon->findChild<QObject *>(QStringLiteral("qavsi.material.glyph"));
    if (text) {
        const auto font = text->property("font").value<QFont>();
        expect(font.variableAxisValue("wght") == 400 && font.variableAxisValue("opsz") == 24
                   && font.variableAxisValue("FILL") == 0 && font.variableAxisValue("GRAD") == 0,
               QStringLiteral("Material defaults are wght400 opsz24 GRAD0 FILL0"));
        const bool software = qobject_cast<QQuickWindow *>(late.get())->rendererInterface()->graphicsApi()
            == QSGRendererInterface::Software;
        expect(text->property("renderType").toInt() == (software ? 0 : 2),
               QStringLiteral("Material uses CurveRendering on RHI and QtRendering on software"));
    }
    late->setProperty("iconFilled", true);
    const auto filled = capture(late.get());
    expect(differentPixels(outline, filled) > 20 && alphaPixels(filled) > alphaPixels(outline),
           QStringLiteral("FILL1 changes the actual settings glyph geometry"));
    if (text)
        expect(text->property("font").value<QFont>().variableAxisValue("FILL") == 1,
               QStringLiteral("filled state reaches the font FILL axis"));
    late->setProperty("iconName", QStringLiteral("material:star"));
    late->setProperty("iconFilled", false);
    const auto starOutline = capture(late.get());
    late->setProperty("iconFilled", true);
    const auto starFilled = capture(late.get());
    expect(differentPixels(starOutline, starFilled) > 20 && alphaPixels(starFilled) > alphaPixels(starOutline),
           QStringLiteral("FILL changes star glyph pixels without choosing another asset"));
    const auto fillWarnings = warningMessages.size();
    auto fillAppearance = create(first, R"(
        import QtQml
        QtObject { property var axes: ({ "FILL": 0.5 }) })");
    if (!fillAppearance) return 2;
    late->setProperty("iconAppearance", QVariant::fromValue(fillAppearance.get()));
    const auto fractional = capture(late.get());
    expect(differentPixels(fractional, starOutline) > 20 && differentPixels(fractional, starFilled) > 20,
           QStringLiteral("public appearance FILL0.5 renders a glyph distinct from both filled endpoints"));
    fillAppearance->setProperty("axes", QVariantMap{{QStringLiteral("FILL"), std::numeric_limits<double>::quiet_NaN()}});
    expect(differentPixels(capture(late.get()), starFilled) == 0,
           QStringLiteral("non-finite appearance FILL falls back to the caller's filled state"));
    square->setProperty("iconAppearance", QVariant::fromValue(fillAppearance.get()));
    expect(differentPixels(capture(square.get()), green) == 0 && warningMessages.size() == fillWarnings,
           QStringLiteral("invalid Material appearance data does not change another provider or emit warnings"));
    fillAppearance->setProperty("axes", QVariantMap{});
    late->setProperty("iconFilled", false);
    expect(differentPixels(capture(late.get()), starOutline) == 0,
           QStringLiteral("absent appearance FILL preserves the caller's unfilled glyph"));
    late->setProperty("iconAppearance", QVariant::fromValue<QObject *>(nullptr));
    late->setProperty("iconFilled", true);
    auto badAppearance = create(first, R"(
        import QtQml
        QtObject {
            property var weight: "not-a-number"
            property real opticalSize: Number.NaN
            property real grade: Number.POSITIVE_INFINITY
            property int renderType: 999
        })");
    if (!badAppearance) return 2;
    late->setProperty("iconAppearance", QVariant::fromValue(badAppearance.get()));
    if (text) {
        const auto font = text->property("font").value<QFont>();
        expect(font.variableAxisValue("wght") == 400 && font.variableAxisValue("opsz") == 24
                   && font.variableAxisValue("GRAD") == 0,
               QStringLiteral("non-numeric and non-finite appearance inputs restore safe font defaults"));
        const bool software = qobject_cast<QQuickWindow *>(late.get())->rendererInterface()->graphicsApi()
            == QSGRendererInterface::Software;
        expect(text->property("renderType").toInt() == (software ? 0 : 2),
               QStringLiteral("invalid render type restores the backend-compatible default"));
    }
    const auto fontWarnings = matchingWarnings(QStringLiteral("pixelSize"));
    late->setProperty("iconSize", 0);
    expect(alphaPixels(capture(late.get())) == 0, QStringLiteral("zero-size Material glyph paints no ink"));
    late->setProperty("iconSize", std::numeric_limits<double>::quiet_NaN());
    expect(alphaPixels(capture(late.get())) == 0
               && matchingWarnings(QStringLiteral("pixelSize")) == fontWarnings,
           QStringLiteral("non-finite Material size paints no ink and does not assign an invalid font pixel size"));
    late->setProperty("iconSize", 48);
    expect(alphaPixels(capture(late.get())) > 0, QStringLiteral("valid size recovers the existing Material glyph"));

    auto exactMissing = create(first, fixture(), {{"iconName", "material:SETTINGS"}});
    expect(exactMissing && status(subject(exactMissing.get()), 3),
           QStringLiteral("Material uses exact catalog names, without aliases or literal misspelled text"));
    auto materialImport2 = create(*second, "import QtQml\nimport Qavsi.Icons.Material\nQtObject {}");
    expect(materialImport2 && packages(secondRegistry).contains(QStringLiteral("material")),
           QStringLiteral("a second engine initializes its own Material package"));
    auto *symbols = first.singletonInstance<QObject *>("Qavsi.Icons.Material", "MaterialSymbols");
    expect(symbols != nullptr, QStringLiteral("Material catalog singleton is available"));
    if (symbols) {
        const auto names = symbols->property("names").toStringList();
        expect(names.size() > 3000, QStringLiteral("provider ships the full catalog rather than demo icons"));
        bool all = true;
        for (const auto &name : names) {
            QString glyph;
            all &= QMetaObject::invokeMethod(symbols, "glyph", Q_RETURN_ARG(QString, glyph), Q_ARG(QString, name)) && !glyph.isEmpty();
        }
        expect(all, QStringLiteral("every catalog name resolves to a codepoint"));
    }
#endif
    second.reset();
    expect(secondRegistry.isNull() && packages(firstRegistry).contains(QStringLiteral("test")),
           QStringLiteral("destroying one engine cleans its registry without invalidating another"));
    const auto allWarnings = warningMessages.join('\n');
    expect(!allWarnings.contains(QStringLiteral("Binding loop")) && !allWarnings.contains(QStringLiteral("TypeError")),
           QStringLiteral("runtime has no binding loops or JavaScript type errors"));
    qInfo() << "Qavsi Icons:" << "assertions" << assertions << "failures" << failures << "DPR" << dpr;
    qInstallMessageHandler(previousHandler);
    return failures ? 1 : 0;
}
