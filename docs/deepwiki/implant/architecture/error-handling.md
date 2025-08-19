---
title: "Error Handling"
slug: "/deepwiki/implant/architecture/error-handling"
summary: "Approach to error surfaces and resilience."
audience: ["dev"]
tags: ["errors", "resilience"]
level: "guide"
status: "active"
owner: "@uevr/docs"
reviewers: ["@uevr/maintainers"]
last_verified: "2025-08-19"
---

- Fail fast in setup, degrade gracefully at runtime.
- Log with actionable context.
- Avoid exceptions across module boundaries.
