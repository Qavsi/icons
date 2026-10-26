# Core-only isolation

Qt 6.12.0/macOS; `QAVSI_ICONS_BUILD_MATERIAL=OFF`.

- Source build: `/private/tmp/qavsi-icons-core-only`.
- Final source tests: 56 assertions, zero failures at software DPR 1 and DPR 2.
- Installed prefix: `/private/tmp/qavsi-icons-core-prefix`, refreshed after the
  caller-context source fix. It contains Core library/plugin/QML/headers/config
  and licenses; no Material target, module, or font.
- An independent `find_package(QavsiIcons CONFIG REQUIRED COMPONENTS Material)`
  fails clearly: `Unavailable QavsiIcons component: Material`.

The final runtime includes relative image source resolution, size/appearance
finite guards, GUI-thread rasterization, and per-name warnings. No native UI,
SDK, or Git changes were made for this check. SF/FA and remote images are outside
this slice. Full provider/installed/native checks are separate gates.
