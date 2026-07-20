# Command-line-shell

A lightweight custom Unix-like command-line shell written in C++.

This project demonstrates how a shell works internally using:
- input parsing
- process creation with `fork()`
- program execution with `execvp()`
- inter-process communication using `pipe()`
- pipeline chaining (e.g., `ls | grep cpp | wc -l`)

---

## Features

- Interactive shell prompt with current working directory
- Executes standard system commands (via `execvp`)
- Supports command pipelines using `|`
- Handles multiple piped commands
- Graceful exit with:
  - `end` command
  - EOF (`Ctrl + D`)

---

## How It Works

### 1. Prompt and Input
The shell repeatedly:
1. prints the current working directory
2. reads a full line from standard input

Prompt format:
```bash
<current-directory><<$ 
```

---

### 2. Tokenization
The entered line is split by whitespace into tokens.

Example:
```bash
ls -la | grep mini | wc -l
```
Tokens become:
- `ls`
- `-la`
- `|`
- `grep`
- `mini`
- `|`
- `wc`
- `-l`

---

### 3. Pipeline Segmentation
Tokens are divided into command groups separated by `|`.

For the above example:
1. `["ls", "-la"]`
2. `["grep", "mini"]`
3. `["wc", "-l"]`

Each command is converted to a `char*` argument array ending with `nullptr` so it can be passed to `execvp()`.

---

### 4. Process and Pipe Execution
For each command in the pipeline:
- create a pipe (except for the last command)
- `fork()` a child process
- in the child:
  - connect previous pipe read-end to `STDIN` if needed
  - connect current pipe write-end to `STDOUT` if needed
  - run command using `execvp()`
- in the parent:
  - close unused FDs
  - carry forward next read-end for chaining

Finally, parent waits for all child processes using `wait()`.

---

## File Structure

```text
.
├── miniShell.cpp
└── README.md
```

---

## Build Instructions

### Requirements
- Linux / Unix-like OS
- `g++` compiler (C++11 or later)

### Compile
```bash
g++ -std=c++11 -o minishell miniShell.cpp
```

### Run
```bash
./minishell
```

---

## Usage Examples

### Basic command
```bash
<<$ pwd
```

### Listing files
```bash
<<$ ls -la
```

### Two-command pipeline
```bash
<<$ ls | grep cpp
```

### Multi-stage pipeline
```bash
<<$ ps aux | grep ssh | wc -l
```

### Exit shell
```bash
<<$ end
```
or press `Ctrl + D`.

---

## Supported Behavior

- External commands available in system `PATH`
- Command arguments separated by spaces
- Multiple pipes in one command line

---

## Current Limitations

This shell is intentionally minimal. It **does not** currently support:

- Built-in commands like `cd`, `export`, `history`
- Input/output redirection (`>`, `<`, `>>`)
- Background execution (`&`)
- Quoted strings (`"hello world"`) as single token
- Escape sequence handling
- Environment variable expansion (`$HOME`)
- Command chaining with `&&` / `||` / `;`
- Detailed error messages for command-not-found cases

---

## Error Handling Notes

- If `pipe()` fails: prints `Pipe creation failed`
- If `fork()` fails: prints process creation error
- If `execvp()` fails in child: child exits with status `1`

---

## Internal APIs Used

- `getcwd()` – display current working directory
- `std::getline()` – read command input
- `pipe()` – create communication channels between processes
- `fork()` – create child process
- `dup2()` – redirect standard input/output
- `execvp()` – execute external command
- `wait()` – synchronize parent with children

---

## Educational Value

This project is a good starting point for learning:
- Unix process model
- Shell internals
- File descriptor management
- Pipelining mechanics in operating systems

---

## Possible Enhancements

You can extend this shell by adding:
1. Built-in `cd` and `exit`
2. Redirection support (`<`, `>`, `>>`)
3. Background jobs (`&`) and job control
4. Signal handling (`Ctrl + C`)
5. Command history and arrow navigation
6. Quote-aware tokenizer
7. Autocomplete and tab support

---

## Author

@Krtk-k
