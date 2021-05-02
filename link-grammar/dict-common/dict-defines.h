/*************************************************************************/
/* Copyright (c) 2004                                                    */
/* Daniel Sleator, David Temperley, and John Lafferty                    */
/* Copyright (c) 2013 Linas Vepstas                                      */
/* All rights reserved                                                   */
/*                                                                       */
/* Use of the link grammar parsing system is subject to the terms of the */
/* license set forth in the LICENSE file included with this software.    */
/* This license allows free redistribution and use in source and binary  */
/* forms, with or without modification, subject to certain conditions.   */
/*                                                                       */
/*************************************************************************/

#ifndef _DICT_DEFINES_H_
#define _DICT_DEFINES_H_

/* The following define the names of the special strings in the dictionary. */
#define LEFT_WALL_WORD   ("LEFT-WALL")
#define RIGHT_WALL_WORD  ("RIGHT-WALL")

#define UNKNOWN_WORD "<UNKNOWN-WORD>"

/* If the maximum disjunct cost is yet uninitialized, the value defined in the
 * dictionary (or if not defined then DEFAULT_MAX_DISJUNCT_COST) is used. */
static const double UNINITIALIZED_MAX_DISJUNCT_COST = -10000.0;
static const double DEFAULT_MAX_DISJUNCT_COST = 2.7;
/* We need some of these as literal strings. */
#define LG_DISJUNCT_COST                        "max-disjunct-cost"
#define LG_DICTIONARY_VERSION_NUMBER            "dictionary-version-number"
#define LG_DICTIONARY_LOCALE                    "dictionary-locale"

/*      Some size definitions.  Reduce these for small machines */
/* MAX_WORD is large, because Unicode entries can use a lot of space */
#define MAX_WORD 180          /* maximum number of bytes in a word */

/* Word subscripts come after the subscript mark (ASCII ETX)
 * In the dictionary, a dot is used; but that dot interferes with dots
 * in the input stream, and so we convert dictionary dots into the
 * subscript mark, which we don't expect to see in user input.
 */
#define SUBSCRIPT_MARK '\3'
#define SUBSCRIPT_DOT '.'

static inline const char *subscript_mark_str(void)
{
	static const char sm[] = { SUBSCRIPT_MARK, '\0' };
	return sm;
}
#endif
