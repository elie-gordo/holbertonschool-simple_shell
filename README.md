# Simple Shell

A UNIX command line interpreter written in C as part of the Holberton School curriculum (tasks 0-6).

## Description

**hsh** is a simple UNIX shell that reads commands from standard input, parses them, resolves executables via `PATH`, and executes them using `fork` and `execve`.

### Features

- Interactive and non-interactive modes
- `PATH`-based command resolution (no fork if the command is not found)
- Command arguments support
- Built-in commands: `exit`, `env`
- Error messages matching `/bin/sh` format
- Correct exit status propagation (including `127` for "not found")

## Compilation

```
gcc -Wall -Werror -Wextra -pedantic -std=gnu89 *.c -o hsh
```

## Usage

### Interactive mode

```
$ ./hsh
$ /bin/ls
$ ls -l /tmp
$ exit
```

### Non-interactive mode

```
$ echo "ls -la" | ./hsh
$ echo "/bin/echo hello" | ./hsh
```

## Built-in Commands

| Command | Description |
|---------|-------------|
| `exit` | Exit the shell |
| `env` | Print the current environment variables |

## File Structure

| File | Description |
|------|-------------|
| `shell.h` | Header with prototypes and system includes |
| `main.c` | Entry point — delegates to `shell_loop` |
| `shell.c` | Main REPL loop (`shell_loop`) |
| `parser.c` | Line tokenization (`parse_line`, `free_args`) |
| `path.c` | `PATH` resolution (`find_command`) |
| `execute.c` | Fork + execve + wait (`execute_cmd`, `print_not_found`) |
| `builtins.c` | Builtin dispatcher (`exit`, `env`) |
| `man_1_simple_shell` | Man page |
| `AUTHORS` | Project contributors |

## Examples

```
$ echo "ls" | ./hsh
AUTHORS
README.md
builtins.c
...

$ echo "qwerty" | ./hsh
./hsh: 1: qwerty: not found

$ echo "/bin/echo hello world" | ./hsh
hello world

$ echo "env" | ./hsh | head -3
SHELL=/bin/bash
PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin
...
```

## Authors

See the [AUTHORS](AUTHORS) file.
