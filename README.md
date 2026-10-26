# Qavsi Icons

Qt 6.12+, CMake 3.30+, C++20. Independent package icon routing with public Qt
QIcon/QPainter and provider-owned font rendering. No Merce, design tokens,
StyleKit, theme selection or icon-name equivalence dictionary.

```qml
import Qavsi.Icons
import Qavsi.Icons.Material

AppIcon {
    name: "material:settings"
    size: 24
    color: root.palette.windowText
    filled: selected
}
```

Material3 style owns the selected/FILL policy; AppIcon forwards only the
requested state. A generic appearance object/map may provide provider-specific
weight/grade/opticalSize/renderType. Material defaults are 400/0/24/CurveRendering;
software uses QtRendering. Font/catalog/licenses are owned only by Material.
There are no font-family or token defaults in Core.

Source: add_subdirectory(path/to/icons icons), link QavsiIcons::Core and the
optional QavsiIcons::Material. Installed: find_package(QavsiIcons 0.1 CONFIG
REQUIRED COMPONENTS Core Material), then the same targets/imports. Core-only
build: QAVSI_ICONS_BUILD_MATERIAL=OFF. No network fetch is needed.

`name` accepts exact package:icon. `qt:document-save` uses QIcon::fromTheme;
`image:qrc:/...` or source:url uses a local/qrc image. `source` is the fallback
for an absent package/glyph. Relative image URLs resolve against the AppIcon
caller's QML context, including when the working directory differs. A failed
lookup warns once per name/engine, then
uses source or a generic missing mark. Remote image URLs are not supported in
this local native image bridge. Native SK.Button names/sources remain the Qt
path; packaged names require a delegate using AppIcon.

Each QQmlEngine owns its registry; Material's normal engine plugin registers
its renderer. Importing the provider after an icon exists updates it through
packagesChanged. Registration conflicts and reserved namespace overrides fail.
No QObject is shared between engines. Qt owns one immutable font registration.

Tests and remaining limits are recorded in DESIGN.md. No SF/FontAwesome
provider, mapping table, downloading, control wrapper or global QIcon theme
mutation is included in this slice.
