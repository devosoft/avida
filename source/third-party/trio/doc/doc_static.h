/*************************************************************************
 *
 * $Id: doc_static.h,v 1.1 2003/11/25 18:50:07 kaben Exp $
 *
 * Copyright (C) 2001 Bjorn Reese and Daniel Stenberg.
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
 * MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE AUTHORS AND
 * CONTRIBUTORS ACCEPT NO RESPONSIBILITY IN ANY CONCEIVABLE MANNER.
 *
 ************************************************************************/

/** @addtogroup StaticStrings Static String Functions.
Replacements for the standard C string functions.

@b SYNOPSIS

@verbatim
cc ... -ltrio -lm

#include <triostr.h>
@endverbatim

@b DESCRIPTION

This package renames, fixes, and extends the standard C string handling
functions.

@b Naming

Renaming is done to provide more clear names, to provide a consistant naming
and argument policy, and to hide portability issues.

@li All functions starts with "trio_".
@li Target is always the first argument, if present, except where the target
is optional, such as @ref trio_to_double.
@li Functions requiring a size for target includes "_max" in its name, and
the size is always the second argument.
@li Functions performing case-sensitive operations includes "_case" in its
name.

@b Fixing

Fixing is done to avoid subtle error conditions.
For example, @c strncpy does not terminate the result with a zero if the
source string is bigger than the maximal length, so technically the result
is not a C string anymore. @ref trio_copy_max makes sure that the result
is zero terminated.

@b Extending

Extending is done to provide a richer set of fundamental functions.
This includes functionality such as wildcard matching ( @c trio_match )
and calculation of hash values ( @c trio_hash ).

*/
