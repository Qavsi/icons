#!/usr/bin/env python3
"""Subset one font by Unicode codepoint, discarding ligature substitutions."""
import sys
from fontTools import subset
from fontTools.ttLib import TTFont

font = TTFont(sys.argv[1], recalcTimestamp=False)
options = subset.Options()
options.recalc_timestamp = False
options.drop_tables += ["GSUB"]
subsetter = subset.Subsetter(options=options)
subsetter.populate(unicodes=[int(cp, 16) for cp in sys.argv[3:]])
subsetter.subset(font)
font.save(sys.argv[2])
font.close()
