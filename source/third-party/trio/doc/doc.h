/*************************************************************************
 *
 * $Id: doc.h,v 1.1 2003/11/25 18:50:07 kaben Exp $
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

/**
@mainpage

@author Bjørn Reese
@author Daniel Stenberg

@section intro Introduction

Trio is a fully matured and stable set of printf and string functions
designed be used by applications with focus on portability or with the
need for additional features that are not supported by standard stdio
implementation.

There are several cases where you may want to consider using trio:

@li Portability across heterogeneous platforms.
@li Embedded systems without stdio support.
@li Extendability of unsupported features.
@li Your native version does not do everything you need.

When you write applications that must be portable to a wide range of
platforms you often have to deal with inadequate implementations of the
stdio library functions. Most notably is the lack of secure formatting
functions, such as snprintf, or the lack of parameter reordering commonly
used for the internationalization of applications, such as the <num>$
modifier. Sometimes the feature you need is simply not present in stdio.
So you end up spending much effort on determining which platforms supports
what, and to write your own versions of various features. This is where
trio can help you. Trio is a platform-independent implementation of the
stdio printf and scanf functions and the string library functions.

The functionality described in the stdio standards is a compromise, and
does unfortunately not include a mechanism to extend the functionality for
an individual application. Oftentimes an application has the need for an
extra feature, and the application code can become much more clear and
readable by using an extension mechanism. Trio supports a range of useful
extensions such as user-defined specifiers, passing of arguments in arrays,
localized string scanning, thousand-separators, and arbitrary integer bases.

Trio fully implements the C99 (ISO/IEC 9899:1999) and UNIX98 (the Single
Unix Specification, Version 2) standards, as well as many features from
other implemenations, e.g. the GNU libc and BSD4.

@section examples Examples

@subsection ex1 Binary Numbers
Output an integer as a binary number using a trio extension.
@verbatim
  trio_printf("%..2i\n", number);
@endverbatim

@subsection ex2 Thousand-separator
Output a number with thousand-separator using a trio extension.
@verbatim
  trio_printf("%'f\n", 12345.6);
@endverbatim
The thousand-separator described by the locale is used. 

@subsection ex3 Fixed Length Array and Sticky Modifier
Output an fixed length array of floating-point numbers.
@verbatim
  double array[] = {1.0, 2.0, 3.0};
  printf("%.2f %.2f %.2f\n", array[0], array[1], array[2]);
@endverbatim
The same with two trio extensions (arguments are passed in an array, and
the first formatting specifier sets the sticky option so we do not have
to type all the formatting modifiers for the remaining formatting specifiers)
@verbatim
  trio_printfv("%!.2f %f %f\n", array);
@endverbatim
Another, and more powerful, application of being able to pass arguments in
an array is the creation of the printf/scanf statement at run-time, where
the formatting string, and thus the argument list, is based on an external
configuration file.

@subsection ex4 Localized scanning
Parse a string consisting of one or more upper-case alphabetic characters
followed by one or more numeric characters.
@verbatim
  sscanf(buffer, "%[A-Z]%[0-9]", alphabetic, numeric);
@endverbatim
The same but with locale using a trio extension.
@verbatim
  trio_sscanf(buffer, "%[:upper:]%[:digit:]", alphabetic, numeric);
@endverbatim

@section legal Legal Issues
Trio is distributed under the following license, which allows practically
anybody to use it in almost any kind of software, including proprietary
software, without difficulty.

"Copyright (C) 1998-2001 Bjorn Reese and Daniel Stenberg.

Permission to use, copy, modify, and distribute this software for any
purpose with or without fee is hereby granted, provided that the above
copyright notice and this permission notice appear in all copies.

THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE AUTHORS AND
CONTRIBUTORS ACCEPT NO RESPONSIBILITY IN ANY CONCEIVABLE MANNER."

@section contribution Contribution

@subsection contribute Contribute
We appreciate any type of contribution, from ideas over improvements to
error corrections.

The project space contains references to bug and feature tracking,
mailing-list, and the CVS repository. We prefer communication via the
mailing-list, but do not require you to be subscribed, because trio is a
small project.

The project space is located at http://sourceforge.net/projects/ctrio/

@subsection contributors Contributors
We have received contributions from the following persons (in alphabetic
order sorted by surname)

@li Craig Berry
@li Stan Boehm
@li Robert Collins
@li Danny Dulai
@li John Fotheringham
@li Markus Henke
@li Ken Gibson
@li Paul Janzen
@li Richard Jinks
@li Tero Jänkä
@li Howard Kapustein
@li Alexander Lukyanov
@li Emmanuel Mogenet
@li Jacob Navia
@li Jose Ortiz
@li Joe Orton
@li Gisli Ottarsson
@li Marc Werwerft
@li Igor Zlatkovic

Please let us know, and accept our apology, if we have omitted anybody.

*/
