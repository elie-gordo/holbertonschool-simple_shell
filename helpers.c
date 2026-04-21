#include "shell.h"

/**
 * _is_number - Check if a string represents a valid integer
 * @s: string to check
 *
 * Return: 1 if the string is a valid number, 0 otherwise
 *
 * Description: Accepts optional leading + or -, followed by
 * one or more digits. Used by builtin_exit to validate arguments.
 */
int _is_number(char *s)
{
	int i = 0;

	if (s == NULL || s[0] == '\0')
		return (0); /* NULL or empty string */
	if (s[0] == '+' || s[0] == '-')
		i++; /* Skip optional sign */
	if (s[i] == '\0')
		return (0); /* Sign alone is not a number */
	while (s[i])
	{
		if (s[i] < '0' || s[i] > '9')
			return (0); /* Non-digit character found */
		i++;
	}
	return (1); /* All characters are digits */
}

/**
 * _atoi_custom - Convert a string to an integer
 * @s: string to convert (assumed valid by caller)
 *
 * Return: the integer value represented by the string
 *
 * Description: Handles optional leading sign (+ or -).
 * Processes digits left to right with base-10 accumulation.
 */
int _atoi_custom(char *s)
{
	long result = 0;
	int sign = 1, i = 0;

	if (s[0] == '-') /* Negative number */
	{
		sign = -1;
		i++;
	}
	else if (s[0] == '+') /* Explicit positive sign */
		i++;
	while (s[i] >= '0' && s[i] <= '9') /* Accumulate digits */
	{
		result = result * 10 + (s[i] - '0');
		i++;
	}
	return ((int)(result * sign)); /* Apply sign and return */
}

/**
 * _itoa - Convert an integer to a newly allocated string
 * @n: integer to convert
 *
 * Return: allocated string representation, or NULL on failure
 *
 * Description: Uses sprintf to format the number into a temporary
 * buffer, then allocates and copies the result. Used for $? and $$.
 */
char *_itoa(int n)
{
	char buf[32]; /* Temporary buffer (enough for any int) */
	int len;
	char *result;

	len = sprintf(buf, "%d", n); /* Format number as string */
	result = malloc(len + 1); /* Allocate exact size needed */
	if (result == NULL)
		return (NULL);
	strcpy(result, buf); /* Copy to allocated string */
	return (result);
}
