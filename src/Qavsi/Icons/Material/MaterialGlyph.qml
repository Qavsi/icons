// Copyright (c) 2026 Enes Alp
// SPDX-License-Identifier: MIT
pragma ComponentBehavior: Bound
import QtQuick
import Qavsi.Icons
import Qavsi.Icons.Material

IconGlyph {
    id: root
    function finiteBound(value: var, fallback: real, minimum: real, maximum: real): real {
        return typeof value === "number" && Number.isFinite(value) ? Math.max(minimum, Math.min(maximum, value)) : fallback
    }
    readonly property int weight: finiteBound(appearance?.weight, 400, 100, 700)
    readonly property real opticalSize: finiteBound(appearance?.opticalSize, 24, 20, 48)
    readonly property int grade: finiteBound(appearance?.grade, 0, -50, 200)
    readonly property int requestedRenderType: appearance?.renderType === Text.QtRendering
        || appearance?.renderType === Text.NativeRendering || appearance?.renderType === Text.CurveRendering
        ? appearance.renderType : Text.CurveRendering
    status: !name.length ? Image.Null : MaterialSymbols.ready && MaterialSymbols.contains(name)
        ? Image.Ready : Image.Error
    Text {
        objectName: "qavsi.material.glyph"
        anchors.fill: parent
        visible: Number.isFinite(root.size) && root.size > 0
        text: root.status === Image.Ready ? MaterialSymbols.glyph(root.name) : ""
        color: root.color
        font.family: MaterialSymbols.family
        font.pixelSize: root.finiteBound(root.size, 1, 1, 4096)
        font.weight: root.weight
        font.variableAxes: ({ "FILL": root.filled ? 1 : 0, "wght": root.weight,
            "opsz": root.opticalSize, "GRAD": root.grade })
        renderType: root.GraphicsInfo.api === GraphicsInfo.Software && root.requestedRenderType === Text.CurveRendering
            ? Text.QtRendering : root.requestedRenderType
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        antialiasing: true
        Accessible.ignored: true
    }
}
