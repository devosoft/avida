/*************************************************************************
 *
 * $Id: doc_scanf.h,v 1.1 2003/11/25 18:50:07 kaben Exp $
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

/** @addtogroup Scanf Formatted Scanning Functions.
Variations of formatted scanning functions.

@b SYNOPSIS

@verbatim
cc ... -ltrio -lm

#include <trio.h>
@endverbatim

@b DESCRIPTION

This documentation is incomplete.
The documentation of the scanf family in [C99] and [UNIX98] also applies
to the trio counterparts.

@b SCANNING

The scanning is controlled by the format string.
The format string can contain normal text and conversion indicators.
The normal text can be any character except the nil character
(\000) and the percent character (\045 = '\%').
Conversion indicators consists of an indication character (%), followed by
zero or more conversion modifiers, and exactly one conversion specifier.

@b Modifiers

@em Positional ( @c 9$ ) [UNIX98]

See @ref trio_printf.

@b Specifiers

@em Percent ( @c % )

@em Character ( @c c )

@em Decimal ( @c d )

@em Floating-point ( @c a, @c A, @c e, @c E, @c f, @c F, @c g, @c G )

@em Integer ( @c i )

@em Count ( @c n )

@em Octal ( @c o )

@em Pointer ( @c p )

@em String ( @c s )

@em Unsigned ( @c u )

@em Hex ( @c x, @c X )

@em Scanlist ( @c [] )

Scanlist Exclusion (@c ^ )

Scanlist Range ( @c - ) [TRIO]

@li Only increasing ranges, i.e. @c [a-b], but not @c [b-a].
@li Transitive ranges, ie. @c [a-b-c] equals @c [a-c].
@li Trailing minus, ie. @c [a-] is interpreted as an @c a and a @c -.
@li Duplicates are ignored.

Scanlist Equivalence Class Expression ( @c [= @c =] ) [TRIO]

Locale dependent (LC_COLLATE).
Only one expression can appear inside the delimiters.
@li @c [=a=] All letters in the same equivalence class as the letter @c a.
@verbatim
  trio_scanf("%[[=a=]b]\n", buffer);
  trio_scanf("%[[=a=][=b=]]\n", buffer);
@endverbatim

Scanlist Character Class Expression ( @c [: @c :]) [TRIO]
Locale dependent (LC_CTYPE).
Only one expression can appear inside the delimiters.
@li @c [:alnum:] Same as @c [:alpha:] and @c [:digit:]
@li @c [:alpha:] Same as @c [:lower:] and @c [:upper:]
@li @c [:cntrl:] Control characters
@li @c [:digit:] Decimal digits
@li @c [:graph:] Printable characters except space
@li @c [:lower:] Lower case alphabetic letters
@li @c [:print:] Printable characters
@li @c [:punct:] Punctuation
@li @c [:space:] Whitespace characters
@li @c [:upper:] Upper case alphabetic letters
@li @c [:xdigit:] Hexadecimal digits
@verbatim
  trio_scanf("%[[:alnum:]]\n", buffer);
  trio_scanf("%[[:alpha:][:digit:]]\n", buffer);
@endverbatim

@b RETURN @b VALUES

@b SEE @b ALSO

@ref trio_printf

*/
