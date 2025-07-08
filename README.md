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
   
2. **Run with desired depth, arguments.** *(learn more about command-line arguments* [arguments](#command-line-arguments) *)*

<p align="center">Lexicographic example</p>
<p align="center">
  <img src="https://i.imgur.com/TpJk564.png" width="80%" alt="find tree (lexicographic)"/>
</p>

I will show some argument examples on this example dir tree *(copy* [example](#scriptsh) *)*
```
testdir/
├── file1.txt
├── file2.log
├── .hiddenfile
├── .hiddendir/
│   └── secret.txt
├── subdir1/
│   ├── nested1.txt
│   └── subdir2/
│       ├── nested2.log
│       └── subdir3/
│           ├── deep.txt
│           └── subdir4/
│               └── deepest.txt
├── largefile.bin
├── owned_by_other.txt
└── exec.sh
```
<p align="center">Lexicographic example</p>
<p align="center">
  <img src="https://i.imgur.com/TpJk564.png" width="90%" alt="find tree (lexicographic)"/>
</p>

<p align="center">find all | find all + hidden/p>
<p align="center">
  <img src="https://i.imgur.com/1NUDmIP.png" width="44%" alt="find -a"/>
  <img src="https://i.imgur.com/F7qycYS.png" width="46%" alt="find "/>
</p>

<p align="center">find by name | find by name hidden</p>
<p align="center">
  <img src="https://i.imgur.com/P6jTTTN.png" width="50%" alt="find -n"/>
  <img src="https://i.imgur.com/OosagJ9.png" width="40%" alt="find -n -a "/>
</p>

<p align="center">find min depth 4 | find max depth 1 (note: root dir is depth 0)</p>
<p align="center">
  <img src="https://i.imgur.com/6K20o5j.png" width="48%" alt="find -f"/>
  <img src="https://i.imgur.com/pUSBl3e.png" width="40%" alt="find -t "/>
</p>


<p align="center">find by mask 755 (chmod +x) | find by user > nobody < </p>
<p align="center">
  <img src="https://i.imgur.com/8iKR8D0.png" width="40%" alt="find -m"/>
  <img src="https://i.imgur.com/YL9fHOB.png" width="44%" alt="find -u "/>
</p>
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

Example script
## script.sh 
```
#!/bin/bash

# basic structure
mkdir -p testdir/.hiddendir
mkdir -p testdir/subdir1/subdir2/subdir3/subdir4

# some files
echo "Hello" > testdir/file1.txt
dd if=/dev/zero of=testdir/file2.log bs=1K count=2

# hidden
echo "Hidden file" > testdir/.hiddenfile
echo "Secret content" > testdir/.hiddendir/secret.txt

# nested (depth 1-4)
echo "Level 1" > testdir/subdir1/nested1.txt
echo "Level 2" > testdir/subdir1/subdir2/nested2.log
echo "Level 3" > testdir/subdir1/subdir2/subdir3/deep.txt
echo "Level 4" > testdir/subdir1/subdir2/subdir3/subdir4/deepest.txt

# big file
dd if=/dev/zero of=testdir/largefile.bin bs=1M count=5

# exe file
echo -e "#!/bin/bash\necho Hello" > testdir/exec.sh
chmod +x testdir/exec.sh

# file with other owner (works only with sudo rights)
sudo touch testdir/owned_by_other.txt
sudo chown nobody:nogroup testdir/owned_by_other.txt
```

