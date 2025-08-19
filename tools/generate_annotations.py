#!/usr/bin/env python3
import json
import os
import sys
from pathlib import Path
from datetime import datetime
import re

ROOT = Path(__file__).resolve().parents[1]
OUT_DIR = ROOT / "docs" / "annotations"

# Mapping rules: path substrings -> (context7 library id, deepwiki page)
RULES = [
    # Runtime and graphics
    (os.sep + "src" + os.sep + "openxr" + os.sep, ("/khronosgroup/openxr-docs", "3.2 VR Runtime Implementations")),
    (os.sep + "src" + os.sep + "openvr" + os.sep, ("/valvesoftware/openvr", "3.2 VR Runtime Implementations")),
    (os.sep + "src" + os.sep + "d3d11" + os.sep, ("/microsoft/directx-graphics-samples", "3.1 D3D11 and D3D12 Components")),
    (os.sep + "src" + os.sep + "d3d12" + os.sep, ("/microsoft/directx-graphics-samples", "3.1 D3D11 and D3D12 Components")),
    # uevr public interfaces and plugins
    (os.sep + "include" + os.sep + "uevr" + os.sep, ("/praydog/uevr", "4 Plugin and Extension System")),
    (os.sep + "src" + os.sep + "plugin" + os.sep, ("/praydog/uevr", "4.1 Plugin Loader")),
    (os.sep + "lua-api" + os.sep, ("/praydog/uevr", "4.2 Lua Scripting")),
    (os.sep + "docs" + os.sep + "uevr-docs" + os.sep, ("/praydog/uevr", "5 Configuration System")),
    # Project source snapshot
    (os.sep + "SOURCECODE" + os.sep, ("/praydog/uevr", "1.1 Architecture Overview")),
    # SDK / Unreal-related (general Unreal references)
    (os.sep + "src" + os.sep + "sdk" + os.sep, ("/mrrobinofficial/guide-unrealengine", "Architecture")),
]

DEFAULT_CTX = ("/praydog/uevr", "1.1 Architecture Overview")

# File extensions considered as text for annotation
TEXT_EXTS = {
    ".c", ".cc", ".cpp", ".cxx", ".h", ".hpp", ".hxx", ".inl", ".ipp",
    ".cs", ".py", ".lua", ".md", ".txt", ".cmake", ".json", ".toml", ".ini", ".cfg",
    ".bat", ".sh", ".ps1"
}

# Paths to skip
SKIP_DIRS = {".git", ".qodo", ".vs", "build", "bin", "obj", "Binaries", "Intermediate"}

# Skip output dir to avoid recursion
SKIP_PREFIXES = [str(OUT_DIR) + os.sep]


def classify(path: Path):
    p = str(path)
    for prefix in SKIP_PREFIXES:
        if p.startswith(prefix):
            return None
    for needle, pair in RULES:
        if needle in p:
            return pair
    return DEFAULT_CTX


def is_text_file(path: Path) -> bool:
    # Classify by extension first
    ext = path.suffix.lower()
    if ext in TEXT_EXTS:
        return True
    # Heuristic for no extension small files
    if ext == "":
        try:
            with open(path, "rb") as f:
                chunk = f.read(2048)
            # Consider text if it decodes as UTF-8/latin-1 and has low binary ratio
            try:
                txt = chunk.decode("utf-8")
            except UnicodeDecodeError:
                try:
                    txt = chunk.decode("latin-1")
                except UnicodeDecodeError:
                    return False
            # If it has many NULs, treat as binary
            if chunk.count(b"\x00") > 0:
                return False
            return True
        except Exception:
            return False
    return False


def annotate_file(path: Path):
    cls = classify(path)
    if cls is None:
        return None
    ctx7_id, wiki_page = cls
    try:
        text = path.read_text(encoding="utf-8")
    except UnicodeDecodeError:
        text = path.read_text(encoding="latin-1", errors="replace")
    # Use regex to split so trailing blank lines are preserved as empty entries,
    # ensuring we annotate every visual line including trailing newlines.
    lines = re.split(r"\r\n|\r|\n", text)

    annotations = {}
    for i, _ in enumerate(lines, start=1):
        annotations[str(i)] = [
            {"source": "deepwiki", "repo": "praydog/uevr", "page": wiki_page},
            {"source": "context7", "library": ctx7_id}
        ]

    rel = path.relative_to(ROOT)
    out_path = OUT_DIR / (str(rel) + ".json")
    out_path.parent.mkdir(parents=True, exist_ok=True)
    payload = {
        "file": str(path),
        "generated_at": datetime.now().isoformat(),
        "schema_version": 1,
        "annotations": annotations,
    }
    out_path.write_text(json.dumps(payload, ensure_ascii=False, indent=2), encoding="utf-8")
    return out_path


def walk_and_annotate(root: Path):
    written = []
    for dirpath, dirnames, filenames in os.walk(root):
        # prunes
        dirnames[:] = [d for d in dirnames if d not in SKIP_DIRS]
        for name in filenames:
            p = Path(dirpath) / name
            # Skip our own outputs
            if any(str(p).startswith(prefix) for prefix in SKIP_PREFIXES):
                continue
            if not is_text_file(p):
                continue
            try:
                out = annotate_file(p)
                if out:
                    written.append(str(out.relative_to(ROOT)))
            except Exception as e:
                # Best-effort: continue
                print(f"WARN: failed {p}: {e}")
    return written


def main():
    root = ROOT
    if len(sys.argv) > 1:
        root = Path(sys.argv[1]).resolve()
    print(f"Annotating repo under: {root}")
    OUT_DIR.mkdir(parents=True, exist_ok=True)
    written = walk_and_annotate(root)
    print(f"Done. Wrote {len(written)} annotation files under {OUT_DIR}.")

if __name__ == "__main__":
    main()
