---
title: "Performance"
slug: "/deepwiki/implant/architecture/performance"
summary: "Guidelines for keeping adapters and runtimes efficient."
audience: ["dev"]
tags: ["performance"]
level: "guide"
status: "active"
owner: "@uevr/docs"]
reviewers: ["@uevr/maintainers"]
last_verified: "2025-08-19"
---

- Avoid allocations on hot paths.
- Prefer precomputed matrices.
- Measure with profiling tools before changes.
