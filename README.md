# simple_shell

A simple UNIX command interpreter project for Holberton School.

## Project Goal
This project asks us to build a minimal shell named `hsh`.

A shell is a program that:
- reads a command line,
- interprets the command,
- creates a child process,
- executes the requested program,
- waits for completion,
- then prompts for the next command.

In very simple words (child-friendly):
- Think of the shell like a helper that listens to what you ask your computer to do.
- If you say "show files", it finds the right program and asks the computer to run it.
- Then it waits until that job is done before listening again.

## Learning Objectives (Explained Clearly)
By the end of this project, we must be able to explain:

- Who designed and implemented the original UNIX operating system:
  Ken Thompson and Dennis Ritchie at Bell Labs.
- Who wrote the first UNIX shell:
  Ken Thompson.
- Who invented the B language (predecessor of C):
  Ken Thompson.
- Who Ken Thompson is:
  A pioneer computer scientist who co-created UNIX and key tools used in modern systems.
- How a shell works:
  Read -> Parse -> Fork -> Exec -> Wait -> Repeat.
- What `pid` and `ppid` are:
  `pid` is a process ID (unique process number), `ppid` is its parent process ID.
- How to manipulate the environment of the current process:
  Read/update environment variables that affect process behavior.
- Function vs system call:
  A function is regular code (often in libraries). A system call asks the kernel to do privileged OS work.
- How to create processes:
  Use `fork`.
- Three `main` prototypes:
  - `int main(void)`
  - `int main(int argc, char *argv[])`
  - `int main(int argc, char *argv[], char *envp[])`
- How PATH is used:
  The shell searches each directory listed in `PATH` to find executables.
- How to execute with `execve`:
  Replace current process image with a new program.
- How to wait for child termination:
  Use `wait` / `waitpid`.
- What EOF is:
  End-of-file condition (for example `Ctrl + D` in interactive input).

## Requirements (From Project Statement)
- Allowed editors: `vi`, `vim`, `emacs`
- Compilation target: Ubuntu 20.04 LTS
- Compiler: `gcc`
- Flags: `-Wall -Werror -Wextra -pedantic -std=gnu89`
- Every file must end with a new line
- `README.md` is mandatory
- Betty style required (`betty-style.pl`, `betty-doc.pl`)
- No memory leaks
- No more than 5 functions per file
- All header files must be include-guarded
- Use system calls only when necessary

## Allowed Functions and System Calls
The project allows specific C library functions and system calls (see project statement), including:
all functions from `string.h`, and:
`access`, `chdir`, `close`, `closedir`, `execve`, `exit`, `_exit`, `fflush`, `fork`, `free`, `getcwd`, `getline`, `getpid`, `isatty`, `kill`, `malloc`, `open`, `opendir`, `perror`, `printf`, `fprintf`, `vfprintf`, `sprintf`, `putchar`, `read`, `readdir`, `signal`, `stat`, `lstat`, `fstat`, `strtok`, `wait`, `waitpid`, `wait3`, `wait4`, `write`.

## Compilation
```bash
gcc -Wall -Werror -Wextra -pedantic -std=gnu89 *.c -o hsh
```

## Expected Execution Modes
Interactive mode:
```bash
./hsh
($) /bin/ls
($) exit
```

Non-interactive mode:
```bash
echo "/bin/ls" | ./hsh
```

Complete non-interactive example from subject:
```bash
cat test_ls_2
/bin/ls
/bin/ls
cat test_ls_2 | ./hsh
```

Expected output:
```bash
hsh main.c shell.c test_ls_2
hsh main.c shell.c test_ls_2
```

## Error Message Rule
The shell output and error behavior must match `/bin/sh`, except the program name in error output must match `argv[0]`.

Example:
```bash
echo "qwerty" | ./hsh
./hsh: 1: qwerty: not found
```

## Repository Content (Task 0)
This task requires:
- `README.md`
- `man_1_simple_shell`
- `AUTHORS`

## Simple shell 1.0 (Current Implementation)
Usage:
```bash
./hsh
```

Implemented for 1.0:
- Displays a prompt (`#cisfun$ `) in interactive mode.
- Reads one command line at a time with `getline`.
- Handles commands with arguments (split by spaces/tabs).
- Resolves commands through `PATH` when command is not an absolute/relative path.
- Does not call `fork` when command cannot be resolved.
- Executes with `fork` + `execve`.
- Prints an error with `perror` when execution fails.
- Waits for child process completion with `waitpid`.
- Handles EOF (`Ctrl + D`) by exiting cleanly.
- Passes `envp` to `execve`.
- Implements built-in `exit`.
- Implements built-in `env` (prints current environment).

Intentionally not implemented in 1.0:
- Pipes, redirections, semicolons.
- Special character handling (`"`, `'`, `` ` ``, `\\`, `*`, `&`, `#`).

## Terminology Quick Guide
- Command interpreter: program that understands user commands.
- Process: running instance of a program.
- System call: direct request from user program to OS kernel.
- Environment variable: key-value configuration available to processes.
- Executable: runnable binary file.

## Authors
See `AUTHORS`.
