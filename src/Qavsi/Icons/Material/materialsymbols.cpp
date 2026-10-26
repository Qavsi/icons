// Copyright (c) 2026 Enes Alp
// SPDX-License-Identifier: MIT
#include "materialsymbols.h"
#include <QFile>
#include <QFontDatabase>
#include <QDebug>
namespace Qavsi::Icons::Material {
MaterialSymbols::MaterialSymbols(QObject *parent) : QObject(parent)
{
    // Qt owns the immutable font registration; no QObject crosses engine boundaries.
    static const int fontId = QFontDatabase::addApplicationFont(QStringLiteral(":/qt/qml/Qavsi/Icons/Material/fonts/MaterialSymbolsRounded.ttf"));
    const auto families = QFontDatabase::applicationFontFamilies(fontId);
    if (fontId < 0 || families.isEmpty()) {
        qWarning("Qavsi.Icons.Material: bundled font could not be registered");
        return;
    }
    m_family = families.first();
    QFile metadata(QStringLiteral(":/qt/qml/Qavsi/Icons/Material/fonts/MaterialSymbolsRounded.codepoints"));
    if (!metadata.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning("Qavsi.Icons.Material: bundled catalog could not be read");
        m_family.clear();
        return;
    }
    while (!metadata.atEnd()) {
        const auto fields = metadata.readLine().trimmed().split(' ');
        if (fields.size() != 2) continue;
        bool ok = false;
        const auto value = fields.at(1).toUInt(&ok, 16);
        if (!ok || value > 0x10ffff || (value >= 0xd800 && value <= 0xdfff)) continue;
        const char32_t codepoint = value;
        m_glyphs.insert(QString::fromUtf8(fields.first()), QString::fromUcs4(&codepoint, 1));
    }
    m_names = m_glyphs.keys();
    m_names.sort();
}
}
