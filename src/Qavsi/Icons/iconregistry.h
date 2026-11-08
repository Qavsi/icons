// Copyright (c) 2026 Enes Alp
// SPDX-License-Identifier: MIT
#pragma once
#include <QObject>
#include <QSet>
#include <QUrl>
#include <QVariantMap>
#include <QtQml/qqmlregistration.h>
#include "QavsiIconsExport.h"
class QQmlEngine;
class QJSEngine;
namespace Qavsi::Icons {
class QAVSIICONSCORE_EXPORT IconRegistry : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON
    Q_PROPERTY(QVariantMap packages READ packages NOTIFY packagesChanged FINAL)
public:
    static IconRegistry *forEngine(QQmlEngine *engine);
    static IconRegistry *create(QQmlEngine *engine, QJSEngine *) { return forEngine(engine); }
    QVariantMap packages() const { return m_packages; }
    Q_INVOKABLE bool registerPackage(const QString &name, const QUrl &renderer);
    Q_INVOKABLE void warnOnce(const QString &name, const QString &reason);
signals:
    void packagesChanged();
private:
    explicit IconRegistry(QObject *parent) : QObject(parent) {}
    QVariantMap m_packages;
    QSet<QPair<QString, QString>> m_warned;
};
}
