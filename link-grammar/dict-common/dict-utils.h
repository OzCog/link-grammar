/*************************************************************************/
/* Copyright (c) 2004                                                    */
/* Daniel Sleator, David Temperley, and John Lafferty                    */
/* Copyright (c) 2009, 2013 Linas Vepstas                                */
/* All rights reserved                                                   */
/*                                                                       */
/* Use of the link grammar parsing system is subject to the terms of the */
/* license set forth in the LICENSE file included with this software.    */
/* This license allows free redistribution and use in source and binary  */
/* forms, with or without modification, subject to certain conditions.   */
/*                                                                       */
/*************************************************************************/

#ifndef _DICT_UTILS_H_
#define _DICT_UTILS_H_

#include "dict-common.h"

/* Exp utilities ... */
void free_Exp(Exp *);
int  size_of_expression(Exp *);
Exp * copy_Exp(Exp *, Pool_desc *, Parse_Options);
bool is_exp_like_empty_word(Dictionary dict, Exp *);

/* X_node utilities ... */
X_node *    catenate_X_nodes(X_node *, X_node *);

/* Dictionary utilities ... */
bool word_has_connector(Dict_node *, const char *, char);
const char * word_only_connector(Dict_node *);
bool word_contains(Dictionary dict, const char * word, const char * macro);

#endif /* _DICT_UTILS_H_ */
