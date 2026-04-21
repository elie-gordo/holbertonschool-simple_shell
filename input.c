#include "shell.h"

/**
 * _fill_buf - Refill the static read buffer from a file descriptor
 * @buf: static buffer to fill with raw bytes
 * @pos: pointer to current read position in the buffer
 * @len: pointer to number of valid bytes in the buffer
 * @fd: file descriptor to read from
 *
 * Return: 0 on success, -1 on EOF, -2 on signal interrupt (EINTR)
 */
static ssize_t _fill_buf(char *buf, size_t *pos, size_t *len, int fd)
{
	ssize_t r;

	if (*pos >= *len) /* Buffer exhausted, need to read more */
	{
		r = read(fd, buf, BUF_SIZE); /* Read up to BUF_SIZE bytes */
		*pos = 0; /* Reset position to start of buffer */
		if (r == -1) /* read() was interrupted by signal */
		{
			*len = 0;
			return (-2); /* Signal interrupt code */
		}
		if (r == 0) /* End of file reached */
		{
			*len = 0;
			return (-1); /* EOF code */
		}
		*len = (size_t)r; /* Store how many bytes were read */
	}
	return (0); /* Buffer has data ready */
}

/**
 * _ensure_cap - Grow the line buffer if more space is needed
 * @lineptr: pointer to the line buffer (may be reallocated)
 * @n: pointer to current buffer capacity
 * @needed: minimum required capacity
 *
 * Return: 0 on success, -1 on allocation failure
 */
static int _ensure_cap(char **lineptr, size_t *n, size_t needed)
{
	char *tmp;

	if (needed < *n)
		return (0); /* Already enough space */
	*n = (*n) * 2; /* Double the capacity */
	if (*n < needed + 1)
		*n = needed + 1; /* Ensure minimum size */
	tmp = realloc(*lineptr, *n); /* Grow the buffer */
	if (tmp == NULL)
		return (-1); /* Allocation failed */
	*lineptr = tmp; /* Update pointer to new buffer */
	return (0);
}

/**
 * _getline - Read one line from fd using buffered I/O (replaces libc getline)
 * @lineptr: pointer to the line buffer (allocated/grown as needed)
 * @n: pointer to the buffer size
 * @fd: file descriptor to read from
 *
 * Return: number of chars read, -1 on EOF, -2 on signal interrupt
 *
 * Description: Uses static variables to maintain a read buffer between
 * calls. Reads one character at a time from the buffer until a newline
 * is found or the buffer needs refilling via read().
 */
ssize_t _getline(char **lineptr, size_t *n, int fd)
{
	static char buf[BUF_SIZE]; /* Persistent read buffer */
	static size_t pos, len;    /* Current position and valid length */
	static int prev_fd = -1;   /* Track fd changes to reset buffer */
	size_t lp = 0;             /* Current position in the output line */
	ssize_t rc;                /* Return code from _fill_buf */

	if (fd != prev_fd) /* New fd: reset the static buffer state */
	{
		pos = 0;
		len = 0;
		prev_fd = fd;
	}
	if (*lineptr == NULL || *n == 0) /* First call: allocate buffer */
	{
		*n = 128;
		*lineptr = malloc(*n);
		if (*lineptr == NULL)
			return (-1);
	}
	while (1)
	{
		rc = _fill_buf(buf, &pos, &len, fd); /* Ensure buffer has data */
		if (rc == -2)
			return (-2); /* Signal: propagate to caller */
		if (rc == -1)
			return (lp > 0 ? (ssize_t)lp : -1); /* EOF */
		(*lineptr)[lp++] = buf[pos++]; /* Copy one char to output */
		if (_ensure_cap(lineptr, n, lp + 1) == -1)
			return (-1); /* Grow buffer if needed */
		if ((*lineptr)[lp - 1] == '\n')
			break; /* End of line found */
	}
	(*lineptr)[lp] = '\0'; /* Null-terminate the line */
	return ((ssize_t)lp);
}
