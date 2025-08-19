---
title: "Style Guide"
slug: "/deepwiki/implant/style-guide"
summary: "Editorial and structural rules to keep documentation precise and high-signal."
audience: ["dev", "docs"]
tags: ["style", "docs"]
level: "guide"
status: "active"
owner: "@uevr/docs"
reviewers: ["@uevr/maintainers"]
last_verified: "2025-08-19"
---

# Principles
- Write for humans first. Keep statements concrete and verifiable.
- Prefer short paragraphs (1–3 sentences). Avoid filler.
- Always set `last_verified` on each file in the PR.

# Standard Frontmatter Template
```markdown
---
title: "<Title>"
slug: "/deepwiki/implant/<section>/<slug>"
summary: "2–3 lines max. No hype."
audience: ["dev", "maintainer"]
tags: ["uevr", "<topic>"]
level: "overview|guide|reference|playbook"
status: "draft|active|deprecated"
owner: "@uevr/docs"
reviewers: ["@uevr/maintainers"]
last_verified: "2025-08-19"
source:
  code: ["<path/to/code>"]
  docs: ["<path/to/related-doc>"]
related:
  - "/deepwiki/implant/<section>/<doc>"
research_references:
  deepwiki_topic: "uevr/<topic>"
  context7_ids: []
invariants:
  - "State critical invariants here."
---
```

# Checklists in Docs
- Use actionable checklists for validation.
- Prefer code paths and exact filenames when referencing code.
