# Context7 + DeepWiki Integration for uevrLCX

## Objectives
- Context7: structured, cross-referenced documentation for fast lookup during dev.
- DeepWiki: detailed deep-dives (hooking patterns, symbol maps, engine quirks).

## Where
- Context7 topics: `docs/UEVRLCX/*.md`, adapter specs in `src/adapters/` when added.
- DeepWiki root: `tools/docs/deepwiki/`
- Generator: `scripts/generate_context7_database.py`

## How
1. Author/update docs under `docs/UEVRLCX/` and DeepWiki pages under `tools/docs/deepwiki/uevrLCX/`.
2. Run the generator to refresh Context7 indices.
3. Cross-link Context7 pages to DeepWiki deep-dives.

## Notes
- Do not mention AI in docs; align with CONTRIBUTING.md.
- Use consistent titles and anchors to maximize retrieval quality.
