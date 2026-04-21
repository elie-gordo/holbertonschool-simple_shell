# Simple Shell

A UNIX command line interpreter written in C as part of the Holberton School curriculum.

## Description

**hsh** is a simple UNIX shell that reads commands from standard input or a file, parses them, resolves executables via PATH, and executes them using `fork` and `execve`.

### Features

- Interactive and non-interactive modes
- PATH-based command resolution
- Command arguments support
- Built-in commands: `exit`, `env`, `setenv`, `unsetenv`, `cd`, `alias`, `help`, `history`
- Command separators: `;`, `&&`, `||`
- Variable expansion: `$?`, `$$`, `$VAR`
- Comment support (`#`)
- Signal handling (Ctrl+C does not quit the shell)
- Custom `getline` using buffered `read()` (no libc `getline`)
- Custom tokenizer (no `strtok`)
- File input mode: `./hsh filename`
- Command history with persistent storage

## Compilation

```
gcc -Wall -Werror -Wextra -pedantic -std=gnu89 *.c -o hsh
```

## Usage

### Interactive mode

```
$ ./hsh
$ ls -la
$ echo hello
$ exit
```

### Non-interactive mode

```
$ echo "ls -la" | ./hsh
$ echo "ls /var ; echo done" | ./hsh
```

### File input mode

```
$ ./hsh commands.txt
```

## Built-in Commands

| Command | Description |
|---------|-------------|
| `exit [status]` | Exit the shell with optional status code |
| `env` | Print current environment variables |
| `setenv VAR VALUE` | Set or modify an environment variable |
| `unsetenv VAR` | Remove an environment variable |
| `cd [dir]` | Change directory (`cd -` for previous, `cd` for HOME) |
| `alias [name[='value']]` | Define or display aliases |
| `help [builtin]` | Display help for built-in commands |
| `history` | Display command history |

## Operators

| Operator | Description |
|----------|-------------|
| `;` | Execute commands sequentially |
| `&&` | Execute next command only if previous succeeded |
| `\|\|` | Execute next command only if previous failed |

## File Structure

| File | Description |
|------|-------------|
| `shell.h` | Header with struct, defines, and prototypes |
| `main.c` | Entry point and signal handler |
| `shell_loop.c` | Main REPL loop and file mode |
| `input.c` | Custom `_getline` using buffered read |
| `tokenizer.c` | Custom tokenizer (no strtok) |
| `operators.c` | Handling of `;`, `&&`, `\|\|` operators |
| `variables.c` | Variable expansion and comment stripping |
| `path.c` | PATH resolution |
| `execute.c` | Fork, execve, and wait |
| `builtins.c` | Built-in dispatcher, exit, env |
| `builtins2.c` | setenv, unsetenv, cd |
| `builtins3.c` | alias |
| `builtins4.c` | help, history |
| `environ.c` | Dynamic environment management |
| `history.c` | History load, save, and management |
| `helpers.c` | Utility functions (atoi, itoa, is_number) |
| `man_1_simple_shell` | Man page |

## Examples

```
$ echo "ls" | ./hsh
file1 file2 file3

$ echo "qwerty" | ./hsh
./hsh: 1: qwerty: not found

$ echo 'echo $?' | ./hsh
0

$ echo "ls /var && echo success" | ./hsh
backups cache crash lib local lock log mail opt run snap spool tmp
success

$ echo "ls /fake || echo fallback" | ./hsh
ls: cannot access '/fake': No such file or directory
fallback
```

## Authors

See the [AUTHORS](AUTHORS) file.
