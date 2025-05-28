## Overview

A simple alternative to the UNIX `find(1)` command.
Supports directory traversal with file name, user ownership, and much more filtering.

---

## Languages and Tools

* **C (C99) + POSIX**
* **CMake** (for compilation)
* **Valgrind** (for memory correctness testing)

---

## Project Structure

| File        | Description                                                   |
| ----------- | ------------------------------------------------------------- |
| `main.c`    | Core logic – argument parsing, filtering, printing            |
| `find.c`    | parsing directories, handling errors                          |
| `utils.c`   | comparators, matching arguments, printing help                |
| `find.h`    | find.c structure and function declarations                    |
| `utils.h`   | utils.c function declaratioons                                |  
| `CMakeLists`| build script for compiling the project                        |

---

## Behavior

The program walks through the directory structure, collecting files that match all specified criteria.

* If no arguments are given, it lists all visible files in and below the current directory.
* Hidden files and directories are skipped unless explicitly provided.
* The output list of file paths is sorted lexicographically.

## How to Run

1. **Compile the project**
   
   ```bash
   cmake -S . -Bbuild
   cd build
   make
   ```
   
2. ** Run with desired depth, arguments. (learn more about command-line arguments [HERE](#command-line-arguments)

---

## Error Handling

| Scenario                           | Description                                         |
| ---------------------------------- | --------------------------------------------------- |
| Invalid option or missing argument | Prints error to stderr and exits with non-zero code |
| Start directory is inaccessible    | Prints error to stderr and exits with non-zero code |
| Some directories are unreadable    | Prints warning and continues search                 |

All errors and warnings are printed to **stderr**. The program never crashes on permission issues; it handles them gracefully.

---

## Command-Line Arguments

- `-n ATTR` – Filter by file name substring `ATTR`. Only the file name (not the full path) is checked.

- `-u USER` – Filter files owned by user with name `USER`.

* NOTE: Logical **AND** is applied between `-n` and `-u`, i.e., all filters must match. Matching is **case-sensitive**

- `-s f | s` – Sort output by full path (`f`) or file size (`s`).
-  Sorting is **lexicographical** for `f`, and **ascending numeric** for `s`.

- `-m MASK` – Filter files by permission `MASK` (octal format).
- The mask is compared against the file's permission bits using standard octal semantics.

- `-f NUM` – Filter files that are at **least** `NUM` directories deep from the start directory.  
  Default: `0` (include all files).

- `-t NUM` – Filter files that are at **most** `NUM` directories deep from the start directory.  
  Default: no upper limit.

- `-a` – Include **hidden files and directories** in the search.  
  By default, names starting with a dot (`.`) are skipped.

- `-0` – Use the null character (`\0`) instead of newline (`\n`) as a separator in output.  
  *Useful for piping results into other programs that can read null-delimited input.*

- `-h` – Display help text to **stderr** and exit.  
---
