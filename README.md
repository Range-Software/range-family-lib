# Range Family Library

A C++/Qt static library providing the family tree domain model for Range Family. It defines the core data types and tree management used by both the GUI application and the CLI tool.

## Key components

- **FTree** — the top-level family tree container; holds persons and relations, supports XML/JSON serialisation, diff/merge, and Graphviz export
- **FPerson** — represents an individual with name, sex, birth/death events, picture, and free-text notes
- **FRelation** — links partners and their children with optional begin/end events (e.g. marriage dates)
- **FEvent** — a dated occurrence attached to a person or relation, composed of an `FDate` and `FPlace`
- **FDate** — flexible date representation supporting exact, approximate, and range values
- **FPlace** — geographic location with address and geo-coordinate
- **FName** — structured personal name (given, middle, surname prefix, surname)
- **FPicture** — embedded or referenced profile image
- **FTreeDiff** — structural diff between two trees, used for import and sync
- **FTreeModel / FTreeModelItem** — Qt item model wrapping `FTree` for use in views

## Dependencies

- `range-base-lib` — core utilities (logging, error handling, versioning)
- Qt 6 Core
