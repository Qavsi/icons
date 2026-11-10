# Qavsi Icons

Qt 6.8+, CMake 3.30+, C++20. Independent package icon routing with public Qt
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
software uses QtRendering. Optional `appearance.axes.FILL` accepts a finite numeric
value clamped to 0..1; absent or invalid values retain `filled`'s 0/1 behavior.
Use a QtObject for live appearance bindings, or replace a JS map to notify changes.
The Material catalog and licenses ship with the provider; applications package fonts explicitly.
There are no font-family or token defaults in Core.

Source: add_subdirectory(path/to/icons icons), link QavsiIcons::Core and the
optional QavsiIcons::Material. Installed: find_package(QavsiIcons 0.2 CONFIG
REQUIRED COMPONENTS Core Material), then the same targets/imports. Core-only
build: QAVSI_ICONS_BUILD_MATERIAL=OFF. No network fetch is needed.

For static modules with FetchContent, limit the variable to the dependency:

```cmake
include(FetchContent)
block(SCOPE_FOR VARIABLES)
    set(BUILD_SHARED_LIBS OFF)
    set(QAVSI_ICONS_BUILD_TESTS OFF)
    FetchContent_Declare(qavsi_icons GIT_REPOSITORY https://github.com/Qavsi/icons.git
        GIT_TAG <pinned-commit>)
    FetchContent_MakeAvailable(qavsi_icons)
endblock()
target_link_libraries(myapp PRIVATE QavsiIconsCorePlugin QavsiIconsMaterialPlugin)
qavsi_icons_add_material_fonts(myapp ICONS settings home)
```

For an installed package:

```cmake
find_package(QavsiIcons 0.2 CONFIG REQUIRED COMPONENTS Core Material)
target_link_libraries(myapp PRIVATE QavsiIcons::Core QavsiIcons::Material)
qavsi_icons_add_material_fonts(myapp ICONS settings home VARIABLE)
```

Use `ALL` instead of `ICONS ...` to embed both complete static fonts without
Python. Add `VARIABLE` to either form to include the variable font. `ICONS`
requires Python 3 and fontTools at configure and build time; set `Python3_EXECUTABLE` to the
interpreter containing fontTools. Unknown names and keywords fail configure.
Call the function once per final executable; merge icon needs from libraries there.
Separate executables may select different icons and font modes.
Applications may use `file(STRINGS)` to load names from their own file.
Without this function icons show the missing mark and warn once per name.
Without `VARIABLE`, fractional FILL rounds to 0 or 1 and custom axes use
400/0/24; the provider warns once per name when this fallback is needed.
Subset fonts drop GSUB ligatures; `ALL` copies the original fonts unchanged.

Linking these static plugin targets includes their Qt-generated plugin imports and
resource initializers. Omit the Material plugin for a Core-only build. See
`tests/static-consumer` for a runnable example.

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

### Material static instances

Material uses clean static instances at the default 400/24/GRAD0 and
FILL0/1. Fractional FILL and custom axes use the variable font when included. This keeps CurveRendering on RHI and QtRendering on software. The application selects the assets it embeds.

For a lighter or heavier icon weight, add `VARIABLE` and set `appearance.weight`
(for example `350`). On Linux (FreeType) the variable font renders every inner
detail, and at wght400 it matches the static instance pixel for pixel. On macOS,
Qt's default CoreText font engine can drop small inner contours of the variable
font at small sizes, such as the "i" in `info` at 20 px; the static instances are
not affected. For a faithful macOS preview, run the application with
`-platform cocoa:fontengine=freetype`.

The committed source assets need no generator at build/runtime; subset builds use fontTools. To regenerate from the
pinned upstream source:

```sh
python3 -m venv build/font-tools
build/font-tools/bin/python -m pip install fonttools==4.60.1 skia-pathops==0.8.0.post2
build/font-tools/bin/python tools/generate-material-instances.py
```

The generator removes overlapping contours, preserves all 4304 catalog names and
records output hashes in `fonts/instances.json`. See
[FontTools instancing](https://fonttools.readthedocs.io/en/latest/varLib/instancer.html)
for the overlap handling used for these assets.

### Breaking changes in 0.2

Material fonts moved from the provider into the application. Migrate by calling
`qavsi_icons_add_material_fonts(<exe> ALL)` on the final executable. Add
`VARIABLE` to keep fractional FILL animations and custom axes.
