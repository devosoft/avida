/*************************************************************************
 *
 * $Id: doc_register.h,v 1.1 2003/11/25 18:50:07 kaben Exp $
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

/** @addtogroup UserDefined User-defined Formatted Printing Functions.
Functions for using customized formatting specifiers.

@b SYNOPSIS

@verbatim
cc ... -ltrio -lm

#include <trio.h>
#include <triop.h>
@endverbatim

@b DESCRIPTION

This documentation is incomplete.

@b User-defined @b Specifier

The user-defined specifier consists of a start character (\074 = '<'), an
optional namespace string followed by a namespace separator (\072 = ':'),
a format string, and an end character (\076 = '>').

The namespace string can consist of alphanumeric characters, and is used to
define a named reference (see below). The namespace is case-sensitive. If no
namespace is specified, then we use an unnamed reference (see below).

The format can consist of any character except the end character ('>'), the
namespace separator (':'), and the nil character (\000).

Any modifier can be used together with the user-defined specifier.

@b Registering

A user-defined specifier must be registered before it can be used.
Unregistered user-defined specifiers are ignored. The @ref trio_register
function is used to register a user-defined specifier. It takes two argument,
a callback function and a namespace, and it returns a handle. The handle must
be used to unregister the specifier later. 

The following example registers a user-define specifier with the "my_namespace"
namespace:

@verbatim
  my_handle = trio_register(my_callback, "my_namespace");
@endverbatim

There can only be one user-defined specifier with a given namespace. There
can be an unlimited number (subject to maximum length of the namespace) of
different user-defined specifiers.

Passing NULL as the namespace argument results in an anonymous reference.
There can be an unlimited number of anonymous references.

@b REFERENCES

There are two ways that a registered callback can be called. Either the
user-defined specifier must contain the registered namespace in the format
string, or the handle is passed as an argument to the formatted printing
function.

If the namespace is used, then a user-defined pointer must be passed as an
argument:

@verbatim
  trio_printf("<my_namespace:format>\n", my_data);
@endverbatim

If the handle is used, then the user-defined specifier must not contain a
namespace. Instead the handle must be passed as an argument, followed by a
user-defined pointer:

@verbatim
  trio_printf("<format>\n", my_handle, my_data);
@endverbatim

The two examples above are equivalent.

There must be exactly one user-defined pointer per user-defined specifier.
This pointer can be used within the callback function with the
@ref trio_get_argument getter function (see below).

The format string is optional. It can be used within the callback function
with the @ref trio_get_format getter function.

@b Anonymous @b References
Anonymous references are specified by passing NULL as the namespace.

The handle must be passed as an argument followed by a user-defined pointer.
No namespace can be specified.

@verbatim
  anon_handle = trio_register(callback, NULL);
  trio_printf("<format>\n", anon_handle, my_data);
@endverbatim

@b Restrictions

@li The length of the namespace string cannot exceed 63 characters.
@li The length of the user-defined format string cannot exceed 255 characters.
@li User-defined formatting cannot re-define existing specifiers.
This restriction was imposed because the existing formatting specifiers have
a well-defined behaviour, and any re-definition would apply globally to an
application (imagine a third-party library changing the behaviour of a
specifier that is crusial to your application).

@b CALLBACK @b FUNCTION

The callback function will be called if a matching user-defined specifier
is found within the formatting string. The callback function takes one input
parameter, an opaque reference which is needed by the private functions. It
returns an @c int, which is currently ignored. The prototype is

@verbatim
  int (*trio_callback_t)(void *ref);
@endverbatim

See the Example section for full examples.

@b PRINTING @b FUNCTIONS

The following printing functions must only be used inside a callback function.
These functions will print to the same output medium as the printf function
which invoked the callback function. For example, if the user-defined
specifier is used in an sprintf function, then these print functions will
output their result to the same string.

@b Elementary @b Printing

There are a number of function to print elementary data types.

@li @ref trio_print_int Print a signed integer. For example:
@verbatim
  trio_print_int(42);
@endverbatim
@li @ref trio_print_uint Print an unsigned integer.
@li @ref trio_print_double Print a floating-point number.
@li @ref trio_print_string Print a string. For example:
@verbatim
  trio_print_string("Hello World");
  trio_print_string(trio_get_format());
@endverbatim
@li @ref trio_print_pointer Print a pointer.

@b Formatted @b Printing

The functions @ref trio_print_ref, @ref trio_vprint_ref, and
@ref trio_printv_ref outputs a formatted string just like its printf
equivalents.

@verbatim
  trio_print_ref(ref, "There are %d towels\n", 42);
  trio_print_ref(ref, "%<recursive>\n", recursive_writer, trio_get_argument());
@endverbatim

@b GETTER @b AND @b SETTER @b FUNCTIONS

The following getter and setter functions must only be used inside a callback
function. They can either operate on the modifiers or on special data.

@b Modifiers

The value of a modifier, or a boolean indication of its presence or absence,
can be found or set with the getter and setter functions.
The generic prototypes of the these getter and setter functions are

@verbatim
  int  trio_get_???(void *ref);
  void trio_set_???(void *ref, int);
@endverbatim

where @c ??? refers to a modifier. For example, to get the width of the
user-defined specifier use

@verbatim
  int width = trio_get_width(ref);
@endverbatim

@b Special @b Data

Consider the following user-defined specifier, in its two possible referencing
presentations.

@verbatim
  trio_printf("%<format>\n", namespace_writer, argument);
  trio_printf("%<namespace:format>\n", argument);
@endverbatim

@ref trio_get_format will get the @p format string, and
@ref trio_get_argument} will get the @p argument parameter.
There are no associated setter functions.

@b EXAMPLES

The following examples show various types of user-defined specifiers. Although
each specifier is demonstrated in isolation, they can all co-exist within the
same application.

@b Time @b Example

Print the time in the format "HOUR:MINUTE:SECOND" if "time" is specified inside
the user-defined specifier.

@verbatim
  static int time_writer(void *ref)
  {
    const char *format;
    time_t *data;
    char buffer[256];

    format = trio_get_format(ref);
    if ((format) && (strcmp(format, "time") == 0)) {
      data = trio_get_argument(ref);
      if (data == NULL)
        return -1;
      strftime(buffer, sizeof(buffer), "%H:%M:%S", localtime(data));
      trio_print_string(ref, buffer);
    }
    return 0;
  }
@endverbatim

@verbatim
  int main(void)
  {
    void *handle;
    time_t now = time(NULL);

    handle = trio_register(time_print, "my_time");

    trio_printf("%<time>\n", handle, &now);
    trio_printf("%<my_time:time>\n", &now);

    trio_unregister(handle);
    return 0;
  }
@endverbatim

@b Complex @b Numbers @b Example

Consider a complex number consisting of a real part, re, and an imaginary part,
im.

@verbatim
  struct Complex {
    double re;
    double im;
  };
@endverbatim

This example can print such a complex number in one of two formats.
The default format is "re + i im". If the alternative modifier is used, then
the format is "r exp(i theta)", where r is the length of the complex vector
(re, im) and theta is its angle.

@verbatim
  static int complex_print(void *ref)
  {
    struct Complex *data;
    const char *format;

    data = (struct Complex *)trio_get_argument(ref);
    if (data) {
      format = trio_get_format(ref);

      if (trio_get_alternative(ref)) {
        double r, theta;

        r = sqrt(pow(data->re, 2) + pow(data->im, 2));
        theta = acos(data->re / r);
        trio_print_ref(ref, "%#f exp(i %#f)", r, theta);

      } else {
        trio_print_ref(ref, "%#f + i %#f", data->re, data->im);
      }
    }
    return 0;
  }
@endverbatim

@verbatim
  int main(void)
  {
    void *handle;

    handle = trio_register(complex_print, "complex");

    /* Normal format. With handle and the with namespace */
    trio_printf("%<>\n", handle, &complex);
    trio_printf("%<complex:>\n", &complex);
    /* In exponential notation */
    trio_printf("%#<>\n", handle, &complex);
    trio_printf("%#<complex:unused data>\n", &complex);

    trio_unregister(handle);
    return 0;
  }
@endverbatim

@b RETURN @b VALUES

@ref trio_register returns a handle, or NULL if an error occured.

@b SEE @b ALSO

@ref trio_printf

@b NOTES

User-defined specifiers, @ref trio_register, and @ref trio_unregister are
not thread-safe. In multi-threaded applications they must be guarded by
mutexes. Trio provides two special callback functions, called ":enter" and
":leave", which are invoked every time a thread-unsafe operation is attempted.
As the thread model is determined by the application, these callback functions
must be implemented by the application.

The following callback functions are for demonstration-purposes only.
Replace their bodies with locking and unlocking of a mutex to achieve
thread-safety.
@verbatim
  static int enter_region(void *ref)
  {
    fprintf(stderr, "Enter Region\n");
    return 1;
  }

  static int leave_region(void *ref)
  {
    fprintf(stderr, "Leave Region\n");
    return 1;
  }
@endverbatim
These two callbacks must be registered before other callbacks are registered.
@verbatim
  trio_register(enter_region, ":enter");
  trio_register(leave_region, ":leave");

  another_handle = trio_register(another_callback, NULL);
@endverbatim

*/
