/*************************************************************************/
/* Copyright (c) 2004                                                    */
/* Daniel Sleator, David Temperley, and John Lafferty                    */
/* Copyright 2019 Amir Plivatsky                                         */
/* All rights reserved                                                   */
/*                                                                       */
/* Use of the link grammar parsing system is subject to the terms of the */
/* license set forth in the LICENSE file included with this software.    */
/* This license allows free redistribution and use in source and binary  */
/* forms, with or without modification, subject to certain conditions.   */
/*                                                                       */
/*************************************************************************/

#include "const-prime.h"
#include "connectors.h"
#include "tracon-set.h"
#include "utilities.h"

/**
 * This is an adaptation of the string_set module for detecting unique
 * connector trailing sequences (aka tracons).
 *
 * It is used to generate tracon encoding for the parsing and for the
 * pruning steps. Not like string_set, the actual hash values here
 * are external.
 *
 * A tracon is identified by (the address of) its first connector.
 *
 * The API here is similar to that of string_set, with this differences:
 *
 * 1. tracon_set_add() returns a pointer to the hash slot of the tracon if
 * it finds it. Else it returns a pointer to a NULL hash slot, and
 * he caller is expected to assign to it the tracon (its address
 * after it is copied to the destination buffer).
 *
 * 2. A new API tracon_set_shallow() is used to require that tracons
 * which start with a shallow connector will not be considered the same
 * as ones that start with a deep connector. The power pruning algo
 * depends on that.
 *
 * 3. A new API tracon_set_reset() is used to clear the hash table slots
 * (only, their value remains intact).
 */

static unsigned int hash_connectors(int k, const Connector *c, unsigned int shallow)
{
	unsigned int accum = shallow && c->shallow;

	for (; c != NULL; c = c->next)
	{
		accum = (k * accum) +
		((c->desc->uc_num)<<18) +
		(((unsigned int)c->multi)<<31) +
		(unsigned int)c->desc->lc_letters;
	}

	return accum;
}

static unsigned int primary_hash_connectors(const Connector *c,
                                            const Tracon_set *ss)
{
	return hash_connectors(7, c, ss->shallow);
}

static unsigned int stride_hash_connectors(const Connector *c,
                                           const Tracon_set *ss)
{
	unsigned int accum = hash_connectors(17, c, ss->shallow);
	accum = ss->mod_func(accum);
	/* This is the stride used, so we have to make sure that
	 * its value is not 0 */
	if (accum == 0) accum = 1;
	return accum;
}

void tracon_set_reset(Tracon_set *ss)
{
	ss->prime_idx = 0;
	ss->size = s_prime[ss->prime_idx];
	ss->mod_func = prime_mod_func[ss->prime_idx];
	memset(ss->table, 0, ss->size*sizeof(clist_slot));
	ss->count = 0;
}

Tracon_set *tracon_set_create(void)
{
	Tracon_set *ss = (Tracon_set *) malloc(sizeof(Tracon_set));

	ss->prime_idx = 0;
	ss->size = s_prime[ss->prime_idx];
	ss->mod_func = prime_mod_func[ss->prime_idx];
	ss->table = (clist_slot *) malloc(ss->size * sizeof(clist_slot));
	memset(ss->table, 0, ss->size*sizeof(clist_slot));
	ss->count = 0;
	ss->shallow = false;
	return ss;
}

/**
 * The connectors must be exactly equal.
 */
static bool connector_equal(const Connector *c1, const Connector *c2)
{
	return c1->desc == c2->desc && (c1->multi == c2->multi);
}

/** Return TRUE iff the tracon is exactly the same. */
static bool connector_list_equal(const Connector *c1, const Connector *c2)
{
	while ((c1 != NULL) && (c2 != NULL)) {
		if (!connector_equal(c1, c2)) return false;
		c1 = c1->next;
		c2 = c2->next;
	}
	return (c1 == NULL) && (c2 == NULL);
}

static bool place_found(const Connector *c, const clist_slot *slot, unsigned int hash,
                         Tracon_set *ss)
{
	if (slot->clist == NULL) return true;
	if (hash != slot->hash) return false;
	if (!connector_list_equal(slot->clist, c)) return false;
	if (ss->shallow && (slot->clist->shallow != c->shallow)) return false;
	return connector_list_equal(slot->clist, c);
}

/**
 * lookup the given string in the table.  Return an index
 * to the place it is, or the place where it should be.
 */
static unsigned int find_place(const Connector *c, unsigned int h, Tracon_set *ss)
{
	unsigned int key = ss->mod_func(h);
	if (place_found(c, &ss->table[key], h, ss)) return key;

	unsigned int s = stride_hash_connectors(c, ss);
	while (true)
	{
		key += s;
		if (key >= ss->size) key = ss->mod_func(key);
		if (place_found(c, &ss->table[key], h, ss)) return key;
	}
}

static void grow_table(Tracon_set *ss)
{
	Tracon_set old = *ss;

	ss->prime_idx++;
	ss->size = s_prime[ss->prime_idx];
	ss->mod_func = prime_mod_func[ss->prime_idx];
	ss->table = (clist_slot *)malloc(ss->size * sizeof(clist_slot));
	memset(ss->table, 0, ss->size*sizeof(clist_slot));
	for (size_t i = 0; i < old.size; i++)
	{
		if (old.table[i].clist != NULL)
		{
			unsigned int p = find_place(old.table[i].clist, old.table[i].hash, ss);
			ss->table[p] = old.table[i];
		}
	}
	/* printf("growing from %zu to %zu\n", old.size, ss->size); */
	free(old.table);
}

void tracon_set_shallow(bool shallow, Tracon_set *ss)
{
	ss->shallow = shallow;
}

Connector **tracon_set_add(Connector *clist, Tracon_set *ss)
{
	assert(clist != NULL, "Connector-ID: Can't insert a null list");

	/* We may need to add it to the table.  If the table got too big, first
	 * we grow it.  Too big is defined as being more than 3/8 full.
	 * There's a huge boost from keeping this sparse. */
	if ((8 * ss->count) > (3 * ss->size)) grow_table(ss);

	unsigned int h = primary_hash_connectors(clist, ss);
	unsigned int p = find_place(clist, h, ss);

	if (ss->table[p].clist != NULL)
		return &ss->table[p].clist;

	ss->table[p].hash = h;
	ss->count++;

	return &ss->table[p].clist;
}

Connector *tracon_set_lookup(const Connector *clist, Tracon_set *ss)
{
	unsigned int h = primary_hash_connectors(clist, ss);
	unsigned int p = find_place(clist, h, ss);
	return ss->table[p].clist;
}

void tracon_set_delete(Tracon_set *ss)
{
	if (ss == NULL) return;
	free(ss->table);
	free(ss);
}
