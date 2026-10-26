// Copyright (c) 2026 Enes Alp
// SPDX-License-Identifier: MIT
#include <QQmlEngineExtensionPlugin>
#include <QQmlEngine>
#include "iconregistry.h"

void qml_register_types_Qavsi_Icons_Material();
class MaterialPlugin : public QQmlEngineExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QQmlEngineExtensionInterface_iid)
public:
    MaterialPlugin(QObject *parent = nullptr) : QQmlEngineExtensionPlugin(parent)
    {
        volatile auto registration = &qml_register_types_Qavsi_Icons_Material;
        Q_UNUSED(registration)
    }
    void initializeEngine(QQmlEngine *engine, const char *) override
    {
        Qavsi::Icons::IconRegistry::forEngine(engine)->registerPackage(QStringLiteral("material"),
            QUrl(QStringLiteral("qrc:/qt/qml/Qavsi/Icons/Material/MaterialGlyph.qml")));
    }
};
#include "materialplugin.moc"
