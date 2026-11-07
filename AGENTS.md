# QuiteRSS fork - project instructions

## Build

- The local machine has no Qt toolchain. Build through GitHub Actions
  (`.github/workflows/build.yml`).
- The Linux job uses Qt 5 + QtWebKit from Ubuntu packages.
- The Windows job uses MSYS2 Qt 5 + QtWebKit and produces a deployed artifact.

## Database

- The SQLite schema version is `versionDB` in `src/database/database.cpp`.
  Bump it for every schema change and add a matching migration block.
- Migrations must be additive (`ALTER TABLE ... ADD COLUMN`). Never drop or
  rewrite user subscription tables.
- A new application version triggers an automatic backup of `feeds.db` and the
  settings file before migration.

## Code style

- Follow the existing Qt/C++ style, two-space indentation, and `tr()` for UI
  strings.
- Keep source files ASCII; translations live in `lang/*.ts`.
- Keep changes focused on the feature branch `feature/filter-content-list`.
