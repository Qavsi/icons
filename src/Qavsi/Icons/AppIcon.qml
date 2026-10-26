// Copyright (c) 2026 Enes Alp
// SPDX-License-Identifier: MIT
pragma ComponentBehavior: Bound
import QtQuick

Item {
    id: root
    property string name: ""
    property url source: ""
    required property real size
    required property color color
    property bool filled: false
    property var appearance: null
    readonly property int separator: name.indexOf(":")
    readonly property string packageName: separator > 0 ? name.substring(0, separator) : ""
    readonly property string glyphName: separator > 0 ? name.substring(separator + 1) : ""
    readonly property url rendererSource: glyphName.length > 0 ? IconRegistry.packages[packageName] ?? "" : ""
    readonly property bool requested: name.length > 0 || source.toString().length > 0
    readonly property int providerStatus: provider.status === Loader.Ready
        ? (provider.item as IconGlyph)?.status ?? Image.Error
        : provider.status === Loader.Error ? Image.Error : Image.Loading
    readonly property bool nativeSelected: rendererSource.toString().length === 0 || providerStatus === Image.Error
    readonly property int status: !requested ? Image.Null : nativeSelected
        ? nativeGlyph.status === Image.Null ? Image.Error : nativeGlyph.status : providerStatus
    readonly property bool missing: requested && status === Image.Error
    readonly property Item selectedRenderer: nativeSelected ? nativeGlyph : provider.item as Item
    property bool complete: false
    implicitWidth: Number.isFinite(size) ? Math.max(0, size) : 0
    implicitHeight: Number.isFinite(size) ? Math.max(0, size) : 0
    Accessible.ignored: true

    function loadProvider(): void {
        provider.setSource(rendererSource, {
            name: Qt.binding(() => root.glyphName),
            size: Qt.binding(() => root.size),
            color: Qt.binding(() => root.color),
            filled: Qt.binding(() => root.filled),
            appearance: Qt.binding(() => root.appearance)
        })
    }
    function warnMissing(): void {
        if (root.requested && (root.missing
            || root.name.length > 0 && root.packageName !== "qt" && root.packageName !== "image" && root.rendererSource.toString().length === 0
            || root.rendererSource.toString().length > 0 && root.providerStatus === Image.Error))
            IconRegistry.warnOnce(root.name || root.source.toString(), qsTr("icon unavailable; using source or missing fallback"))
    }
    onNameChanged: Qt.callLater(root.warnMissing)
    onSourceChanged: Qt.callLater(root.warnMissing)
    onRendererSourceChanged: { if (complete) loadProvider() }
    onStatusChanged: Qt.callLater(root.warnMissing)
    onProviderStatusChanged: Qt.callLater(root.warnMissing)
    Component.onCompleted: {
        complete = true
        loadProvider()
        Qt.callLater(root.warnMissing)
    }
    Loader {
        id: provider
        anchors.fill: parent
        active: root.rendererSource.toString().length > 0
        visible: !root.nativeSelected && root.providerStatus === Image.Ready
    }
    NativeIconImage {
        id: nativeGlyph
        objectName: "qavsi.icons.native"
        anchors.fill: parent
        visible: root.nativeSelected && status === Image.Ready
        name: root.nativeSelected && root.packageName === "qt" ? root.glyphName : ""
        source: root.nativeSelected ? root.source.toString().length > 0 ? root.source
            : root.packageName === "image" ? root.glyphName : "" : ""
        color: root.color
        sourceOwner: root
    }
    Item {
        anchors.fill: parent
        visible: root.missing
        Accessible.ignored: true
        Rectangle {
            anchors.centerIn: parent
            width: parent.width * 0.75
            height: parent.height * 0.75
            color: "transparent"
            border.color: root.color
            border.width: Math.max(1, root.size / 16)
        }
        Rectangle {
            anchors.centerIn: parent
            width: parent.width * 0.75
            height: Math.max(1, root.size / 16)
            rotation: -45
            color: root.color
        }
    }
}
