/*************************************************************************
 *
 * $Id: doc_printf.h,v 1.1 2003/11/25 18:50:07 kaben Exp $
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

/** @addtogroup Printf Formatted Printing Functions.
Variations of formatted printing functions.

@b SYNOPSIS

@verbatim
cc ... -ltrio -lm

#include <trio.h>
@endverbatim

@b DESCRIPTION

This documentation is incomplete.
The documentation of the printf family in [C99] and [UNIX98] also applies
to the trio counterparts.

All these functions outputs a string which is formatted according to the
@p format string and the consecutive arguments. The @p format string is
described in the Formatting section below.

@ref trio_printf, @ref trio_vprintf, and @ref trio_printfv writes the
output to the standard output stream (stdout).

@ref trio_fprintf, @ref trio_vfprintf, and @ref trio_fprintfv writes the
output to a given output stream.

@ref trio_dprintf, @ref trio_vdprintf, and @ref trio_dprintfv writes the
output to a file descriptor (this includes, for example, sockets).

@ref trio_sprintf, @ref trio_vsprintf, and @ref trio_sprintfv writes the
output into @p buffer.

@ref trio_snprintf, @ref trio_vsnprintf, and @ref trio_snprintfv writes @p
max - 1 characters into @p buffer followed by a terminating zero character.
If @p max is 1, then @p buffer will be an empty string. If @p max is 0,
then @p buffer is left untouched, and can consequently be NULL. The number
of characters that would have been written to @p buffer, had there been
sufficient space, is returned.

@ref trio_snprintfcat appends the formatted text at the end of @p buffer.

@ref trio_asprintf and @ref trio_vasprintf allocates and returns an
allocated string in @p buffer containing the formatted text.

@b FORMATTING

The @p format string can contain normal text and conversion indicators.
The normal text can be any character except the nil character (\000 =
'\0') and the percent character (\045 = '%'). Conversion indicators
consists of an indication character (%), followed by zero or more conversion
modifiers, and exactly one conversion specifier.

@b Modifiers

Some modifiers exhibit the same behaviour for all specifiers, other modifiers
indicate different behaviours for different specifiers, and other modifiers
are only applicable to certain specifiers. The relationship is described for
each modifier. The number 9 is used to denotes an arbitrary integer.

@em Positional ( @c 9$ ) [UNIX98]

Normally the arguments supplied to these functions are interpreted
incrementially from left to right. Arguments can be referenced specifically in
the format string. The modifier n$ selects the nth argument. The first
argument is referred as 1$. If this modifier is used, it must be the first
modifier after the indication character. n$ can also be used for argument
width, precision, and base.

The performance penalty of using positionals is almost neglible (contrary to
most other printf implementations).

@li @em Reference @em Mix.
Mixing normal and positional specifiers is allowed [TRIO]. For example,
@verbatim
  trio_printf("%d %3$d %2$d\n", 1, 2, 3);
@endverbatim
results in
@verbatim
  1 3 2
@endverbatim
Arguments for the printf family are passed on the stack. On most platforms it
is not possible to determine the size of individual stack elements, so it is
essential that the format string corresponds exactly to the passed arguments.
If this is not the case, incorrect values may be put into the result.

@li @em Reference @em Gap.
For the same reason it is also essential that the format string does not
contain any &quot;gaps&quot; in the positional arguments. For example,
@verbatim
  trio_printf("%1$d %3$d\n", 1, 2, 3);
@endverbatim
is NOT allowed. The format string parser has no knowledge about whether the
second argument is, say, an integer or a long double (which have different
sizes).
@verbatim
@endverbatim
[UNIX98] describes this as unspecified behaviour. [TRIO] will detect reference
gaps and return an error.

@li @em Double @em Reference.
It is also not allowed to reference an argument twice or more. For example,
@verbatim
  trio_printf("%1$d %1$lf\n", 1);
@endverbatim
is NOT allowed, because it references the first argument as two differently
sized objects.
@verbatim
@endverbatim
[UNIX98] describes this as unspecified behaviour. [TRIO] will detect double
references and return an error.

The following two statements are equivalent
@verbatim
  trio_printf("|%d %s\n|", 42, "meanings");
  |42 meanings|

  trio_printf("|%1$d %2$s|\n", 42, "meanings");
  |42 meanings|
@endverbatim

@em Width ( @c 9 )

Specifies the minimum width of a field. If the fields has less characters than
specified by the width, the field will be left adjusted and padded by spaces.
The adjustment and padding can be changed by the Alignment ( @c - ) and
Padding ( @c 0 ) modifiers.

The width is specified as a number. If an asterix ( @c * ) is used instead, the
width will be read from the argument list.

Prefixes, such as 0x for hexadecimal integers, are part of width.
@verbatim
  trio_printf("|%10i|\n", 42);
  |        42|
@endverbatim

@em Precision ( @c .9 )

The precision has different semantics for the various data types.
The precision specifies the maximum number of printed characters for strings,
the number of digits after the decimal-point for floating-point numbers,
the number of significant digits for the @c g (and @c G) representation of
floating-point numbers, the minimum number of printed digits for integers.
@verbatim
  trio_printf("|%10.8i|%.8i|\n", 42, 42);
  |  00000042|00000042|
@endverbatim
 
@em Base ( @c ..9 ) [TRIO]

Sets the base that the associated integer must be converted to. The base can
be between 2 and 36 (both included).
@verbatim
  trio_printf("|%10.8.2i|%10..2i|%..2i|\n", 42, 42, 42);
  |  00101010|    101010|101010|

  trio_printf("|%*.8.*i|\n", 10, 2, 42);
  |  00101010|
@endverbatim
 
@em Padding ( @c 0 )

Integer and floating point numbers are prepended by zeros. The number of
leading zeros are determined by the precision. If precision is not present,
width is used instead.

@em Short ( @c h )

Integer arguments are read as an ( @c unsigned ) @c short @c int. String
and character arguments are read as @c char @c * and @c char respectively.

@em Short @em short ( @c hh ) [C99, GNU]

The argument is read as an ( @c unsigned ) @c char.

@em Fixed @em Size ( @c I ) [MSVC]

The argument is read as a fixed sized integer. The modifier is followed by
a number, which specifies the number of bits in the integer, and can be one
of the following

@li @c I8
@li @c I16
@li @c I32
@li @c I64 (if 64-bits integers are supported)

Works only for integers (i, u, d, o, x, X)

@em Largest ( @c j ) [C99]

The argument is read as an @c intmax_t / @c uintmax_t, which is defined to
be the largest signed/unsigned integer.

@em Long ( @c l )

An integral argument is read as an ( @c unsigned ) @c long @c int. A string
argument is read as a @c wchar_t @c *, and output as a multi-byte character
sequence.

@em Long @em long ( @c ll ) [C99, UNIX98, GNU]

The argument is read as an ( @c unsigned ) @c long @c long @c int.

@em Long @em double ( @c L ) [C99, UNIX98, GNU]

The argument is read as a @c long @c double.

@em ptrdiff_t ( @c t ) [C99]

The argument is read as a @c ptrdiff_t, which is defined to be the signed
integer type of the result of subtracting two pointers.

@em Quad ( @c q ) [BSD, GNU]

Corresponds to the long long modifier ( @c ll ).

@em Wide ( @c w ) [MISC]

For a string argument this is equivalent to using the long modifier ( @c l ).

@em size_t ( @c z ) [C99]

The argument is read as a @c size_t, which is defined to be the type
returned by the @c sizeof operator.

@em size_t ( @c Z ) [GNU]

Corresponds to the size_t modifier ( @c z ).

@em Alternative ( @c # )

Prepend radix indicator for hexadecimal, octal, and binary integer numbers
and for pointers.
Always add a decimal-pointer for floating-point numbers.
Escape non-printable characters for strings.

@em Spacing ( )

Prepend leading spaces when necessary.

@em Sign ( @c + )

Always prepend a sign to numbers. Normally only the negative sign is prepended
to a number. With this modifier the positive sign may also be prepended.

@em Alignment ( @c - )

The output will be left-justified in the field specified by the width.

@em Argument ( @c * )

Width, precision, or base is read from the argument list, rather than from
the formatting string.

@em Quote / @em Grouping ( @c ' ) [MISC]

Groups integers and the integer-part of floating-point numbers according to
the locale. Quote strings and characters.

@em Sticky ( @c ! ) [TRIO]

The modifiers listed for the current specifier will be reused by subsequent
specifiers of the same group.
The following specifier groups exists
@li Integer ( @c i, @c u, @c d, @c o, @c x, @c X )
@li Floating-point ( @c f, @c F, @c e, @c E, @c g, @c G, @c a, @c A )
@li Character ( @c c )
@li String ( @c s )
@li Pointer ( @c p )
@li Count ( @c n )
@li Errno ( @c m )
@li Group ( @c [] )

The sticky modifiers are active until superseeded by other sticky modifiers,
or the end of the format string is reached.
Local modifiers overrides sticky modifiers for the given specifier only.
@verbatim
  trio_printf("|%!08#x|%04x|%x|\n", 42, 42, 42);
  |0x00002a|0x2a|0x00002a|
@endverbatim

@b Specifiers

@em Percent ( @c % )

Produce a percent ( @c % ) character. This is used to quote the indication
character. No modifiers are allowed.
The full syntax is @c %%.
@verbatim
  trio_printf("Percent is %%\n");
  Percent is %
@endverbatim

@em Hex @em floats ( @c a, @c A ) [C99]

Output a hexadecimal (base 16) representation of a floating point number. The
number is automatically preceeded by @c 0x ( or @c 0X ). The exponent is
@c p ( or @c P ).
@verbatim
  trio_printf("|%a|%A|\n", 3.1415, 3.1415e20);
  |0x3.228bc|0X3.228BCP+14|
@endverbatim

@em Binary @em numbers ( @c b, @c B ) [MISC - SCO UnixWare 7]

DEPRECATED: Use Base modifier @c %..2i instead.

@em Character ( @c c )

Output a single character.

@li Quote ( @c ' ) [TRIO].
Quote the character.

@em Decimal ( @c d )

Output a decimal (base 10) representation of a number.

@li Grouping ( @c ' ) [TRIO].
The number is separated by the locale thousand separator.
@verbatim
  trio_printf("|%'ld|\n", 1234567);
  |1,234,567|
@endverbatim

@em Floating-point ( @c e, @c E)

Output a decimal floating-point number.
The style is @c [-]9.99e[-]9, where
@li @c [-]9.99 is the mantissa (as described for the @c f, @c F specifier), and
@li @c e[-]9 is the exponent indicator (either @c e or @c E, depending on the
floating-point specifier), followed by an optional sign and the exponent

If the precision is wider than the maximum number of digits that can be
represented by the floating-point unit, then the number will be adequately
rounded. For example, assuming DBL_DIG is 15
@verbatim
  trio_printf("|%.18e|\n", (1.0 / 3.0));
  |3.333333333333333000e-01|
@endverbatim

@em Floating-point ( @c f, @c F )

Output a decimal floating-point number.
The style is @c [-]9.99, where
@li @c [-] is an optional sign (either @c + or @c -),
@li @c 9 is the integer-part (possibly interspersed with thousand-separators),
@li @c . is the decimal-point (depending on the locale), and
@li @c 99 is the fractional-part.

If more digits are needed to output the number, than can be represented with
the accuracy of the floating-point unit, then the number will be adequately
rounded. For example, assuming that DBL_DIG is 15
@verbatim
  trio_printf("|%f|\n", (2.0 / 3.0) * 1E18);
  |666666666666666700.000000|
@endverbatim

The following modifiers holds a special meaning for this specifier
@li Alternative ( @c # ) [C99].
Add decimal point.
@li Grouping ( @c ' ) [TRIO].
Group integer part of number into thousands (according to locale).

@em Floating-point ( @c g, @c G)

Output a decimal floating-point representation of a number. The format of
either the @c f, @c F specifier or the @c e, @c E specifier is used, whatever
produces the shortest result.

@em Integer ( @c i )

Output a signed integer. Default base is 10.

@em Errno ( @c m ) [GNU]

@em Count ( @c n )

Insert into the location pointed to by the argument, the number of octets
written to the output so far.

@em Octal ( @c o )

Output an octal (base 8) representation of a number.

@em Pointer ( @c p )

Ouput the address of the argument. The address is printed as a hexadecimal
number. If the argument is the NULL pointer the text @c (nil) will be used
instead.
@li Alternative ( @c # ) [TRIO].
Prepend 0x

@em String ( @c s, @c S )

Output a string. The argument must point to a zero terminated string. If the
argument is the NULL pointer the text @c (nil) will be used instead.
@c S is equivalent to @c ls.
@li Alternative ( @c # ) [TRIO].
Escape non-printable characters.

Non-printable characters are converted into C escapes, or hexadecimal numbers
where no C escapes exists for the character. The C escapes, the hexadecimal
number, and all backslashes are prepended by a backslash ( @c \ ).
The supported C escapes are
@li @c \a (\007) = alert
@li @c \b (\010) = backspace
@li @c \f (\014) = formfeed
@li @c \n (\012) = newline
@li @c \r (\015) = carriage return
@li @c \t (\011) = horizontal tab
@li @c \v (\013) = vertical tab

@verbatim
  trio_printf("|One %s Three|One %'s Three|\n", "Two", "Two");
  |One Two Three|One "Two" Three|

  trio_printf("|Argument missing %s|\n", NULL);
  |Argument missing (nil)|

  trio_printf("|%#s|\n", "\007 \a.");
  |\a \a.|
@endverbatim

@em Unsigned ( @c u )

Output an unsigned integer. Default base is 10.

@em Hex ( @c x, @c X )

Output a hexadecimal (base 16) representation of a number.

@li Alternative ( @c # ).
Preceed the number by @c 0x ( or @c 0X ). The two characters are counted
as part of the width.

@em User-defined ( @c <> )

Invoke user-defined formatting.
See @ref trio_register for further information.

@b RETURN @b VALUES

All functions returns the number of outputted characters. If an error occured
then a negative error code is returned [TRIO]. Note that this is a deviation
from the standard, which simply returns -1 (or EOF) and errno set
appropriately.
The error condition can be detected by checking whether the function returns
a negative number or not, and the number can be parsed with the following
macros. The error codes are primarily intended as debugging aide for the
developer.

@li TRIO_EINVAL: Invalid argument.
@li TRIO_ETOOMANY: Too many arguments.
@li TRIO_EDBLREF: Double argument reference.
@li TRIO_EGAP: Argument reference gap.
@li TRIO_ENOMEM: Out of memory.
@li TRIO_ERANGE: Invalid range.
@li TRIO_ERRNO: The error is specified by the errno variable.

Example:
@verbatim
  int rc;

  rc = trio_printf("%r\n", 42);
  if (rc < 0) {
    if (TRIO_ERROR_CODE(rc) != TRIO_EOF) {
      trio_printf("Error: %s at position %d\n",
                  TRIO_ERROR_NAME(rc),
                  TRIO_ERROR_POSITION(rc));
    }
  }
@endverbatim

@b SEE @b ALSO

@e trio_scanf, @e trio_register.

@b NOTES

The printfv family uses an array rather than the stack to pass arguments.
This means that @c short @c int and @c float values will not be handled by
the default argument promotion in C. Instead, these values must be explicitly
converted with the Short (h) modifier in both cases.

Example:
@verbatim
  void *array[2];
  float float_number = 42.0;
  short short_number = 42;

  array[0] = &float_number;
  array[1] = &short_number;

  trio_printfv("%hf %hd\n", array); /* CORRECT */
  trio_printfv("%f %d\n", array); /* WRONG */
@endverbatim

@b CONFORMING @b TO

Throughout this document the following abbreviations have been used to
indicate what standard a feature conforms to. If nothing else is indicated
ANSI C (C89) is assumed.

@li [C89] ANSI X3.159-1989
@li [C99] ISO/IEC 9899:1999
@li [UNIX98] The Single UNIX Specification, Version 2
@li [BSD] 4.4BSD
@li [GNU] GNU libc
@li [MSVC] Microsoft Visual C
@li [MISC] Other non-standard sources
@li [TRIO] Extensions specific for this package

*/
