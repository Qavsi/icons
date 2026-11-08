// Copyright (c) 2026 Enes Alp
// SPDX-License-Identifier: MIT
#include "materialsymbols.h"
#include <QFile>
#include <QFontDatabase>
#include <QDebug>
#include <QRawFont>
namespace Qavsi::Icons::Material {
MaterialSymbols::MaterialSymbols(QObject *parent) : QObject(parent)
{
    const auto registerFont = [](const QString &file) {
        if (!QFile::exists(QStringLiteral(QAVSI_ICONS_MATERIAL_FONT_QRC_PATH) + file)) return QString{};
        const int id = QFontDatabase::addApplicationFont(
            QStringLiteral(QAVSI_ICONS_MATERIAL_FONT_QRC_PATH) + file);
        const auto families = QFontDatabase::applicationFontFamilies(id);
        if (id < 0 || families.isEmpty())
            qWarning() << "Qavsi.Icons.Material: bundled font could not be registered:" << file;
        return families.value(0);
    };
    // Qt owns immutable font registrations; no QObject crosses engine boundaries.
    static const QString variable = registerFont(QStringLiteral("MaterialSymbolsRounded.ttf"));
    static const QString outline = registerFont(QStringLiteral("MaterialSymbolsRoundedOutline.ttf"));
    static const QString filled = registerFont(QStringLiteral("MaterialSymbolsRoundedFilled.ttf"));
    m_family = variable;
    m_outlineFamily = outline;
    m_filledFamily = filled;
    if (m_outlineFamily.isEmpty() || m_filledFamily.isEmpty()) return;
    const QRawFont raw(QStringLiteral(QAVSI_ICONS_MATERIAL_FONT_QRC_PATH "MaterialSymbolsRoundedOutline.ttf"), 24);
    QFile metadata(QStringLiteral(QAVSI_ICONS_MATERIAL_FONT_QRC_PATH "MaterialSymbolsRounded.codepoints"));
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
        if (!ok || value > 0x10ffff || (value >= 0xd800 && value <= 0xdfff)
            || !raw.supportsCharacter(value)) continue;
        const char32_t codepoint = value;
        m_glyphs.insert(QString::fromUtf8(fields.first()), QString::fromUcs4(&codepoint, 1));
    }
    m_names = m_glyphs.keys();
    m_names.sort();
}
}
