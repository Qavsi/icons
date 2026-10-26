// Copyright (c) 2026 Enes Alp
// SPDX-License-Identifier: MIT
import QtQuick

Item {
    property string name: ""
    property real size: 0
    property color color: "transparent"
    property bool filled: false
    property var appearance: null
    property int status: Image.Null
    implicitWidth: Number.isFinite(size) ? Math.max(0, size) : 0
    implicitHeight: Number.isFinite(size) ? Math.max(0, size) : 0
    Accessible.ignored: true
}
