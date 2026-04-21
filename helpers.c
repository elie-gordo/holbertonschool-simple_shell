#include "shell.h"

/**
 * _is_number - Check if a string represents a valid integer
 * @s: string to check
 *
 * Return: 1 if valid number, 0 otherwise
 */
int _is_number(char *s)
{
	int i = 0;

	if (s == NULL || s[0] == '\0')
		return (0);
	if (s[0] == '+' || s[0] == '-')
		i++;
	if (s[i] == '\0')
		return (0);
	while (s[i])
	{
		if (s[i] < '0' || s[i] > '9')
			return (0);
		i++;
	}
	return (1);
}

/**
 * _atoi_custom - Convert string to integer (handles negatives)
 * @s: string to convert
 *
 * Return: integer value
 */
int _atoi_custom(char *s)
{
	long result = 0;
	int sign = 1, i = 0;

	if (s[0] == '-')
	{
		sign = -1;
		i++;
	}
	else if (s[0] == '+')
		i++;
	while (s[i] >= '0' && s[i] <= '9')
	{
		result = result * 10 + (s[i] - '0');
		i++;
	}
	return ((int)(result * sign));
}

/**
 * _itoa - Convert an integer to a newly allocated string
 * @n: integer to convert
 *
 * Return: allocated string, or NULL on failure
 */
char *_itoa(int n)
{
	char buf[32];
	int len;
	char *result;

	len = sprintf(buf, "%d", n);
	result = malloc(len + 1);
	if (result == NULL)
		return (NULL);
	strcpy(result, buf);
	return (result);
}
