// Copyright (c) 2026 Enes Alp
// SPDX-License-Identifier: MIT
#include "iconregistry.h"
#include <QQmlEngine>
#include <QRegularExpression>
#include <QDebug>
namespace Qavsi::Icons {
IconRegistry *IconRegistry::forEngine(QQmlEngine *engine)
{
    auto *existing = engine->findChild<IconRegistry *>(QString{}, Qt::FindDirectChildrenOnly);
    return existing ? existing : new IconRegistry(engine);
}
bool IconRegistry::registerPackage(const QString &name, const QUrl &renderer)
{
    static const QRegularExpression pattern(QStringLiteral("^[a-z][a-z0-9-]*$"));
    if (!pattern.match(name).hasMatch() || name == QLatin1String("qt") || name == QLatin1String("image")
        || !renderer.isValid() || renderer.isRelative()
        || (renderer.scheme() != QLatin1String("qrc") && !renderer.isLocalFile()))
        return false;
    const auto existing = m_packages.constFind(name);
    if (existing != m_packages.cend())
        return existing->toUrl() == renderer;
    m_packages.insert(name, renderer);
    // Let the importing provider finish before existing icons instantiate its renderer.
    QMetaObject::invokeMethod(this, &IconRegistry::packagesChanged, Qt::QueuedConnection);
    return true;
}
void IconRegistry::warnOnce(const QString &name, const QString &reason)
{
    if (m_warned.contains(name))
        return;
    m_warned.insert(name);
    qWarning().noquote() << "Qavsi.Icons:" << name << reason;
}
}
