# ChronoBase

> **A Generic, SQL-Inspired Scheduling and Resource Management Library in C**

ChronoBase is a reusable C library designed to serve as a core engine for any application involving **time-based scheduling, resource allocation, and conflict management**. Instead of creating separate systems for each domain, ChronoBase offers a unified, high-performance foundation that supports them all.

---

## Features

### Core Capabilities
-  **Complete Time/Date System** - Date, Time, DateTime, DateRange, TimeRange, DateTimeRange
-  **Generic Resource Model** - Manage rooms, vehicles, teachers, equipment, etc.
-  **Conflict Detection** - Automatic overlap detection for bookings
-  **SQL-like Operations** - SELECT, WHERE, INSERT, UPDATE, DELETE on in-memory tables
-  **Recurring Schedules** - Daily, weekly, monthly patterns
-  **Constraint System** - Custom validation rules for bookings
-  **Persistent Storage** - JSON/CSV export and import

### Use Cases
-  Hotel reservation systems
-  University enrollment systems
-  Transportation scheduling
-  Medical appointment booking
-  Meeting room management
-  Event and venue booking

---

## Project Structure

```
ChronoBase/
├── README.md
├── LICENSE
├── Makefile
├── .gitignore
│
├── include/
│   ├── database.h          # Generic table/query system
│   ├── chronotime.h        # Time & date types and operations
│   ├── resource.h          # Resource management
│   ├── schedule.h          # Scheduling engine with conflict detection
│   └── storage.h           # JSON/CSV import/export
│
├── src/
│   ├── database.c          # Database implementation
│   ├── chronotime.c        # Time & date implementation
│   ├── resource.c          # Resource implementation
│   ├── schedule.c          # Scheduling implementation
│   └── storage.c           # Storage implementation
│
├── examples/
│   ├── hotel_demo.c        # Hotel booking system demo
│   ├── enrollment_demo.c   # University course enrollment demo
│   └── bus_demo.c          # Bus scheduling demo
│
├── tests/
│   ├── test_database.c     # Database module tests
│   ├── test_chronotime.c   # Time module tests
│   ├── test_schedule.c     # Scheduling tests
│   └── test_runner.c       # Main test runner
│
├── docs/
│   ├── API.md              # API reference
│   ├── ARCHITECTURE.md     # System architecture
│   ├── QUICKSTART.md       # Getting started guide
│   └── EXAMPLES.md         # Usage examples
│
├── build/                  # Compiled objects (gitignored)
│   ├── database.o
│   ├── chronotime.o
│   ├── resource.o
│   ├── schedule.o
│   ├── storage.o
│   ├── hotel_demo
│   ├── enrollment_demo
│   └── bus_demo
│
└── lib/                    # Static library (gitignored)
    └── libchronobase.a
```
---

## Architecture

### Module Overview

#### 1. **ChronoTime Module** (`chronotime.h/c`)
Provides comprehensive time/date handling:
- `Date` - Year, month, day
- `LocalTime` - Hour, minute, second
- `DateTime` - Combined date and time
- `DateRange`, `TimeRange`, `DateTimeRange` - Periods
- Comparison, arithmetic, parsing, formatting

#### 2. **Database Module** (`database.h/c`)
SQL-like in-memory database:
- Generic `Table` structure
- Predicate-based filtering
- CRUD operations (Create, Read, Update, Delete)
- Sorting and joining
- Function-pointer-based queries

#### 3. **Resource Module** (`resource.h/c`)
Generic resource representation:
- ID, name, description
- Capacity tracking
- Active/inactive status

#### 4. **Schedule Module** (`schedule.h/c`)
Core scheduling engine:
- `Booking` - One-time reservations
- `RecurringSchedule` - Repeating patterns
- Conflict detection
- Availability checking
- Constraint validation

#### 5. **Storage Module** (`storage.h/c`)
Persistence layer:
- JSON export/import
- CSV export/import
- Binary serialization

---

## API Highlights

### Date Operations

```c
Date d1 = date_create(2025, 1, 15);
Date d2 = date_add_days(d1, 7);  // Jan 22, 2025
int diff = date_diff_days(d1, d2);  // 7 days
bool valid = date_is_valid(d1);
```

### Time Operations

```c
LocalTime t = time_create(14, 30, 0);  // 2:30 PM
LocalTime later = time_add_seconds(t, 3600);  // 3:30 PM
int seconds = time_diff_seconds(t, later);  // 3600
```

### DateTime Operations

```c
DateTime now = datetime_now();
DateTime later = datetime_add_seconds(now, 86400);  // +1 day
long long diff = datetime_diff_seconds(now, later);
```

### Overlap Detection

```c
DateRange r1 = daterange_create(date_create(2025, 1, 15), 
                                date_create(2025, 1, 20));
DateRange r2 = daterange_create(date_create(2025, 1, 18), 
                                date_create(2025, 1, 22));
bool overlaps = daterange_overlaps(r1, r2);  // true
```

### Table Queries

```c
// Filter with predicate
Table* results = table_select_all(table, predicate_fn, user_data);

// Count matching rows
int count = table_count(table, predicate_fn, user_data);

// Find first match
void* row = table_find_first(table, predicate_fn, user_data);

// Update rows
int updated = table_update(table, predicate_fn, mapper_fn, user_data);

// Delete rows
int deleted = table_delete(table, predicate_fn, user_data);
```

---

## Building Your Own Application

### 1. Include ChronoBase

```c
#include "chronotime.h"
#include "database.h"
#include "schedule.h"
```

### 2. Link Against the Library

```bash
gcc your_app.c -L./lib -lchronobase -lm -o your_app
```

### 3. Extend with Custom Constraints

```c
int capacity_constraint(const Table* bookings, const Booking* new_booking, 
                        void* user_data) 
{
    // Your custom validation logic
    return ;  // Allow booking
}

scheduler_add_constraint(sched, capacity_constraint);
```
# Contributing to ChronoBase

## Code Style

### Formatting
- **Brace style**: Allman (braces on new line)
- **Indentation**: 4 spaces (no tabs)
- **Line length**: 80-100 characters recommended

### Naming
- **Functions**: `module_verb_noun` (e.g., `table_create`, `table_insert`)
- **Variables**: `snake_case`
- **Types**: `PascalCase` (e.g., `Table`, `ColumnDef`)
- **Constants**: `UPPER_SNAKE_CASE`
- **Loop iterator**: Use `j` as first iterator, `k` for nested

### Pointers
- Attach asterisk to type: `Table* table` not `Table *table`

### Memory
- Always check `malloc`/`realloc` returns
- Free all allocated memory on error paths
- Cast allocation returns: `(Table*)malloc(...)`

### Returns
- `0` for false/failure
- `1` for true/success  
- `-1` for errors in numeric returns

---

## Git Workflow

### Branch Structure
```
master
  └── develop
       └── feature/your-module
```

### Creating a Feature
```bash
git checkout develop
git pull origin develop
git checkout -b feature/your-module
```

### Working
```bash
git add .
git commit -m "type: description"
git push
```

### Merging
```bash
git checkout develop
git merge feature/your-module
git push origin develop
```

---

## Commit Message Format

Follow **Conventional Commits** standard:

| Type | Purpose | Example |
|------|---------|---------|
| `feat` | Add new feature | `feat: add dynamic column resizing` |
| `fix` | Fix a bug | `fix: prevent memory leak in table_destroy` |
| `refactor` | Improve code structure | `refactor: simplify column lookup` |
| `perf` | Performance improvements | `perf: optimize table insertion` |
| `style` | Code formatting | `style: convert to Allman braces` |
| `test` | Add/update tests | `test: add table operation tests` |
| `docs` | Update documentation | `docs: update API reference` |
| `build` | Build system changes | `build: update Makefile targets` |
| `chore` | Maintenance tasks | `chore: update .gitignore` |

**Format:**
```
<type>: <description>

[optional body explaining the change]

[optional footer with issue references]
```

**Examples:**
```bash
git commit -m "feat: add table_reserve_rows function"
git commit -m "fix: correct pointer arithmetic in table_get_row"
git commit -m "refactor: use Allman style throughout database module"
```
---

## Before Submitting

- [ ] Code compiles without warnings
- [ ] All demos run successfully
- [ ] No memory leaks
- [ ] Follows code style
- [ ] Commit messages follow conventions
- [ ] Branch is up to date with develop

---

## Module Guidelines

Each module = separate files:
- `include/module.h` - Public API
- `src/module.c` - Implementation

**Only edit files related to your module.** Don't modify other modules.
---

