# uevrLCX Brand Guide

This guide standardizes the use of the uevrLCX logo and mascot across README, GitHub Pages, issues/PRs, and external documents.

## Core Assets
- **Logo (SVG):** `assets/logo/uevrLCX-logo.svg` and `docs/assets/logo/uevrLCX-logo.svg`
- **Mascot (SVG):** `assets/logo/uevrLCX-mascot.svg` and `docs/assets/logo/uevrLCX-mascot.svg`
- **Icon-only (SVG):** `assets/logo/uevrLCX-icon.svg` and `docs/assets/logo/uevrLCX-icon.svg`

Use the `docs/assets/...` copies on GitHub Pages. Use the root `assets/...` copies in the repository README.

## Visual Consistency
- **Headset geometry:** Mascot reuses the exact VR headset from the logo for consistency.
- **Gradient palette:** Cyan → violet → magenta (`#00F7FF → #7E4DFF → #B03CFF → #FF40E6`).
- **Background:** Prefer dark backgrounds (`#0B0F14`). Keep sufficient contrast for text (#D5E4FF).

## Usage Rules
- **README:** Use HTML `<img>` with alt text and fixed widths for consistent rendering.
  - Logo: width ~720px
  - Mascot: width ~920px
- **Icon-only:** Use for badges, small embeds, and social buttons.
- **Do not modify** gradients, proportions, or add vendor engine logos/trademarks.

## Context7 vs DeepWiki Referencing
- **Context7:** Use only for external library documentation references (APIs, hooks, OpenXR, D3D11/12, etc.).
- **DeepWiki:** Use for repository-specific knowledge, analyses, adapter notes, and implementation status.
- Avoid duplicating the same paragraph in both; cross-link instead.
  - Example: “See Context7 > OpenXR runtime docs” vs “See DeepWiki > VR integration status”.

## Light/Dark Variants (Next)
- If needed, create light-mode variants (reduced glow, darker text) in `assets/logo/variants/` and mirror under `docs/assets/logo/variants/`.

## File Paths Summary
- Repo README assets: `assets/logo/...`
- GitHub Pages assets: `docs/assets/logo/...`

## Contact
For changes or proposals, open an issue using the “Research Contribution” PR template and link the specific asset path.
