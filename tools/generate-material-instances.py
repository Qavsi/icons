#!/usr/bin/env python3
# Copyright (c) 2026 Enes Alp
# SPDX-License-Identifier: MIT
"""Prepare default Material endpoints; not required by CMake or at runtime."""
import hashlib
import json
from pathlib import Path

import fontTools
import pathops
from fontTools.ttLib import TTFont
from fontTools.varLib.instancer import instantiateVariableFont, OverlapMode

FONTS = Path(__file__).resolve().parents[1] / "src/Qavsi/Icons/Material/fonts"
SOURCE = FONTS / "MaterialSymbolsRounded.ttf"


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def main():
    provenance = json.loads((FONTS / "provenance.json").read_text())
    source_hash = digest(SOURCE)
    if source_hash != provenance["files"][SOURCE.name]:
        raise ValueError("Source font differs from the pinned upstream bytes")
    catalog = [int(line.split()[1], 16) for line in
               (FONTS / "MaterialSymbolsRounded.codepoints").read_text().splitlines()]
    records = []
    for fill, suffix in [(0, "Outline"), (1, "Filled")]:
        print("Preparing", suffix, flush=True)
        font = TTFont(SOURCE, recalcTimestamp=False)
        axes = {"FILL": fill, "wght": 400, "opsz": 24, "GRAD": 0}
        instantiateVariableFont(font, axes, inplace=True, overlap=OverlapMode.REMOVE)
        family = "Qavsi Material Symbols Rounded " + suffix
        names = {1: family, 2: "Regular", 3: "Qavsi-Material-" + provenance["fontVersion"] + "-" + suffix,
                 4: family + " Regular", 6: "QavsiMaterialSymbolsRounded" + suffix + "-Regular",
                 16: family, 17: "Regular"}
        for record in font["name"].names:
            if record.nameID in names:
                record.string = names[record.nameID].encode(record.getEncoding())
        for name_id, value in names.items():
            font["name"].setName(value, name_id, 3, 1, 0x409)
        output = FONTS / ("MaterialSymbolsRounded" + suffix + ".ttf")
        font.save(output)
        font.close()
        with TTFont(output) as check:
            cmap = check.getBestCmap()
            if any(cp not in cmap or check.getGlyphID(cmap[cp]) == 0 for cp in catalog):
                raise ValueError("Generated instance lost catalog coverage")
            if "fvar" in check:
                raise ValueError("Endpoint must be a full static instance")
        records.append({"file": output.name, "family": family, "axes": axes,
                        "sha256": digest(output), "bytes": output.stat().st_size,
                        "verifiedCatalogNames": len(catalog)})
        print(output.name, output.stat().st_size, "bytes", flush=True)
    manifest = {"source": SOURCE.name, "sourceSha256": source_hash,
                "upstreamRevision": provenance["revision"], "license": "Apache-2.0",
                "fonttools": fontTools.__version__, "skiaPathops": pathops.__version__,
                "transforms": ["pin all axes", "remove overlapping contours", "unique family names"],
                "instances": records}
    (FONTS / "instances.json").write_text(json.dumps(manifest, indent=2) + "\n")


if __name__ == "__main__":
    main()
