/*
 * Author: Nikolay Dvurechensky
 * Site: https://dvurechensky.pro/
 * Gmail: dvurechenskysoft@gmail.com
 * Last Updated: 20 апреля 2026 16:23:13
 * Version: 1.0.468
 */

// Copyright (C) 1996 - 2002 Florian Schintke
//
// This is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2, or (at your option) any later
// version.
//
// Thanks to the E.S.O. - ACS project that has done this C++ interface
// to the wildcards pttern matching algorithm

#include "wildcards.h"

int
Wildcard::wildcardfit(const char *wildcard, const char *test)
{
	int fit = 1;

	for (; ('\000' != *wildcard) && (1 == fit) && ('\000' != *test); wildcard++)
	{
		switch (*wildcard)
		{
		case '[':
			wildcard++; /* Уберите открывающую квадратную скобку */
			fit = set(&wildcard, &test);
			/* Нам не нужно уменьшать подстановочный знак, как в случае */
			/* звездочки, потому что закрывающая ] все еще там. */
			break;
		case '?':
			test++;
			break;
		case '*':
			fit = asterisk(&wildcard, &test);
			/* Звездочка была пропущена asterisk(), но цикл будет увеличиваться сам по себе. */
			/* Так что нам приходится уменьшаться */
			wildcard--;
			break;
		default:
			fit = (int)(*wildcard == *test);
			test++;
		}
	}
	while ((*wildcard == '*') && (1 == fit))
		/* Здесь тестовая строка пуста, иначе вы не сможете выйти из предыдущего цикла */
		wildcard++;
	return (int)((1 == fit) && ('\0' == *test) && ('\0' == *wildcard));
}

int
Wildcard::set(const char **wildcard, const char **test)
{
	int fit = 0;
	int negation = 0;
	int at_beginning = 1;

	if ('!' == **wildcard)
	{
		negation = 1;
		(*wildcard)++;
	}
	while ((']' != **wildcard) || (1 == at_beginning))
	{
		if (0 == fit)
		{
			if (('-' == **wildcard)
				&& ((*(*wildcard - 1)) < (*(*wildcard + 1)))
				&& (']' != *(*wildcard + 1))
				&& (0 == at_beginning))
			{
				if (((**test) >= (*(*wildcard - 1)))
					&& ((**test) <= (*(*wildcard + 1))))
				{
					fit = 1;
					(*wildcard)++;
				}
			}
			else if ((**wildcard) == (**test))
			{
				fit = 1;
			}
		}
		(*wildcard)++;
		at_beginning = 0;
	}
	if (1 == negation)
		/* изменение с нуля на единицу и наоборот */
		fit = 1 - fit;
	if (1 == fit)
		(*test)++;

	return (fit);
}

int
Wildcard::asterisk(const char **wildcard, const char **test)
{
	/* Warning: Использует несколько возвратов */
	int fit = 1;

	/* Сотрите ведущую звездочку */
	(*wildcard)++;
	while (('\000' != (**test))
		&& (('?' == **wildcard)
			|| ('*' == **wildcard)))
	{
		if ('?' == **wildcard)
			(*test)++;
		(*wildcard)++;
	}
	/* Теперь может случиться так, что тест пуст и подстановочный знак содержит атериски. */
	/* Затем мы удаляем их, чтобы получить правильное состояние */
	while ('*' == (**wildcard))
		(*wildcard)++;

	if (('\0' == (**test)) && ('\0' != (**wildcard)))
		return (fit = 0);
	if (('\0' == (**test)) && ('\0' == (**wildcard)))
		return (fit = 1);
	else
	{
		/* Ни тест, ни подстановочный знак не являются пустыми!          */
		/* Первый символ Wildcard отсутствует в [*?] */
		if (0 == wildcardfit(*wildcard, (*test)))
		{
			do
			{
				(*test)++;
				/* Пропустите как можно больше символов в тестовой строке */
				/* Остановка, если произошло совпадение символов */
				while (((**wildcard) != (**test))
					&& ('[' != (**wildcard))
					&& ('\0' != (**test)))
					(*test)++;
			} while ((('\0' != **test)) ?
				(0 == wildcardfit(*wildcard, (*test)))
				: (0 != (fit = 0)));
		}
		if (('\0' == **test) && ('\0' == **wildcard))
			fit = 1;
		return (fit);
	}
}