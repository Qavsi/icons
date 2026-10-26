# Qavsi Icons contract

Core is design-system independent. Size and color are explicit caller inputs;
there are no semantic colors, typography tokens, theme selection or selected
state decisions. AppIcon(name,source,size,color,filled,appearance) routes exact
package:icon names. AppIcon and glyph renderers are decorative to accessibility.
Controls own accessible names and user input.

Material owns its full matching font/catalog, font axes, and render fallback.
FILL0/1 is driven by the caller/style filled state; weight400, optical-size24,
grade0 and CurveRendering are Material defaults. Software falls back to
QtRendering. Exact metadata lookup returns a codepoint, never a ligature string
for a misspelled name. Color including transparent remains caller-owned.

Unknown package/glyph emits at most one warning per name/engine and uses an
explicit source fallback or a simple missing mark. Native local/qrc image tint
uses public QIcon/QPainter/QQuickPaintedItem::Image on RHI and software; no
private Qt Quick implementation types or shaders are needed. Relative URLs
resolve through the AppIcon caller's QQmlContext, not the helper module or process
working directory. Remote sources
are outside this bridge's current contract.

Provider registration is engine-local via the standard QQmlEngine plugin
lifecycle, never a process-global QObject registry. Identical registration is
idempotent; a namespace collision or qt/image override fails. Late imports
notify existing icons after provider module initialization. Optional Material
is excluded completely from a Core-only build/install, including font assets.
The only process-global state is Qt's immutable bundled font registration.

Verification must cover exact name and missing fallback, full metadata, FILL
axes and pixel shape, explicit tint/transparent, software renderer, separate
engines, late provider imports, registration conflict, source/installed and
relocated package consumption. This first slice does not include SF/FA or a
general remote-image loader, icon aliases, FILL animation or icon theme API.

Appearance is an opaque provider-specific value: a QtObject supports live property
bindings; replace a plain JS map as a whole to notify changes. Core does not
declare font fields. Material validates finite numeric axis values and bounds.

Native fallback retains one tinted QImage, rasterized in GUI-thread polish.
Render-thread paint only reads that image. Source/name/color/geometry/window/DPR
changes invalidate it; the CPU raster is bounded to 4096 device pixels per edge.
The official Rounded2.978 font/catalog is pinned to Google10ecc147,4304 names;
all 4004 unique codepoints map to nonzero font glyphs. Source bytes are unchanged.
A zero or non-finite icon size collapses the font glyph; it never selects QFont's
implicit default size. Invalid appearance values use the provider defaults.
