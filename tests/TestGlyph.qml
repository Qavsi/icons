// Copyright (c) 2026 Enes Alp
// SPDX-License-Identifier: MIT
import QtQuick
import Qavsi.Icons

IconGlyph {
    id: root
    status: root.name === "square" ? Image.Ready : Image.Error
    Rectangle { anchors.fill: parent; color: root.color }
}
