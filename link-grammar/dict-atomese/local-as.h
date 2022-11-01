/*
 * local-as.h
 *
 * Local copy of AtomSpace
 *
 * Copyright (c) 2022 Linas Vepstas <linasvepstas@gmail.com>
 */

#include <opencog/atomspace/AtomSpace.h>
#include <opencog/persist/api/StorageNode.h>

using namespace opencog;

class Local
{
public:
	bool using_external_as;
	const char* node_str; // (StorageNode \"foo://bar/baz\")
	AtomSpacePtr asp;
	StorageNodePtr stnp;
	Handle linkp;     // (Predicate "*-LG connector string-*")
	// Handle djp;       // (Predicate "*-LG disjunct string-*")
	Handle miks;      // (Predicate "*-Mutual Info Key cover-section")
	Handle mikp;      // (Predicate "*-Mutual Info Key-*")
	Handle lany;      // (LgLinkNode "ANY")
	int cost_index;   // Offset into the FloatValue
	double cost_scale;
	double cost_offset;
	double cost_cutoff;
	double cost_default;

	int pair_index;   // Offset into the FloatValue
	double pair_scale;
	double pair_offset;
};

Exp* make_exprs(Dictionary dict, const Handle& germ);
Exp* make_pair_exprs(Dictionary dict, const Handle& germ);
std::string cached_linkname(Local*, const Handle& pair);
