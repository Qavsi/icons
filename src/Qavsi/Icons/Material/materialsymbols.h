// Copyright (c) 2026 Enes Alp
// SPDX-License-Identifier: MIT
#pragma once
#include <QObject>
#include <QHash>
#include <QStringList>
#include <QtQml/qqmlregistration.h>
namespace Qavsi::Icons::Material {
class MaterialSymbols : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON
    Q_PROPERTY(QString family READ family CONSTANT FINAL)
    Q_PROPERTY(QString outlineFamily READ outlineFamily CONSTANT FINAL)
    Q_PROPERTY(QString filledFamily READ filledFamily CONSTANT FINAL)
    Q_PROPERTY(QStringList names READ names CONSTANT FINAL)
    Q_PROPERTY(bool ready READ ready CONSTANT FINAL)
    Q_PROPERTY(int glyphCount READ glyphCount CONSTANT FINAL)
public:
    explicit MaterialSymbols(QObject *parent = nullptr);
    QString family() const { return m_family; }
    QString outlineFamily() const { return m_outlineFamily; }
    QString filledFamily() const { return m_filledFamily; }
    QStringList names() const { return m_names; }
    bool ready() const { return !m_family.isEmpty() && !m_glyphs.isEmpty(); }
    int glyphCount() const { return m_glyphs.size(); }
    Q_INVOKABLE QString glyph(const QString &name) const { return m_glyphs.value(name); }
    Q_INVOKABLE bool contains(const QString &name) const { return m_glyphs.contains(name); }
private:
    QString m_family;
    QString m_outlineFamily;
    QString m_filledFamily;
    QStringList m_names;
    QHash<QString, QString> m_glyphs;
};
}
