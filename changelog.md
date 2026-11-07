# Changelog

## 0.19.5 - 2026-11-07

### Added

- News filter conditions support a list mode: edit one value per line in a
  large editor, with automatic removal of blank lines and duplicates and a
  live count of valid items.
- A large editor is also available for single-value conditions.
- List conditions are expanded at runtime into the same SQL clauses as
  manually added conditions. Large lists are queried in batches to stay below
  SQLite's expression depth limit.

### Changed

- Filter conditions are loaded in insertion order (`ORDER BY id`).
- Database schema version 18 adds the `filterConditions.list` column.

### Fixed

- The "News doesn't contain" condition now requires both title and description
  to not contain the value.
