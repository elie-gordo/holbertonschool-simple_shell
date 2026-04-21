#include "shell.h"

/**
 * _fill_buf - Refill the static read buffer from fd
 * @buf: static buffer to fill
 * @pos: current position in buffer
 * @len: current length of valid data in buffer
 * @fd: file descriptor to read from
 *
 * Return: 0 on success, -1 on EOF or error
 */
static ssize_t _fill_buf(char *buf, size_t *pos, size_t *len, int fd)
{
	ssize_t r;

	if (*pos >= *len)
	{
		r = read(fd, buf, BUF_SIZE);
		*pos = 0;
		if (r <= 0)
		{
			*len = 0;
			return (-1);
		}
		*len = (size_t)r;
	}
	return (0);
}

/**
 * _ensure_cap - Grow line buffer if needed
 * @lineptr: pointer to line buffer
 * @n: pointer to buffer capacity
 * @needed: required capacity
 *
 * Return: 0 on success, -1 on failure
 */
static int _ensure_cap(char **lineptr, size_t *n, size_t needed)
{
	char *tmp;

	if (needed < *n)
		return (0);
	*n = (*n) * 2;
	if (*n < needed + 1)
		*n = needed + 1;
	tmp = realloc(*lineptr, *n);
	if (tmp == NULL)
		return (-1);
	*lineptr = tmp;
	return (0);
}

/**
 * _getline - Read a line from fd using buffered I/O (no libc getline)
 * @lineptr: pointer to the line buffer
 * @n: pointer to the buffer size
 * @fd: file descriptor to read from
 *
 * Return: number of characters read, or -1 on EOF/error
 */
ssize_t _getline(char **lineptr, size_t *n, int fd)
{
	static char buf[BUF_SIZE];
	static size_t pos, len;
	static int prev_fd = -1;
	size_t lp = 0;

	if (fd != prev_fd)
	{
		pos = 0;
		len = 0;
		prev_fd = fd;
	}
	if (*lineptr == NULL || *n == 0)
	{
		*n = 128;
		*lineptr = malloc(*n);
		if (*lineptr == NULL)
			return (-1);
	}
	while (1)
	{
		if (_fill_buf(buf, &pos, &len, fd) == -1)
			return (lp > 0 ? (ssize_t)lp : -1);
		(*lineptr)[lp++] = buf[pos++];
		if (_ensure_cap(lineptr, n, lp + 1) == -1)
			return (-1);
		if ((*lineptr)[lp - 1] == '\n')
			break;
	}
	(*lineptr)[lp] = '\0';
	return ((ssize_t)lp);
}
