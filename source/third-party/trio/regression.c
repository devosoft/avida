/*************************************************************************
 * Regression test
 */

#include "triodef.h"
#if defined(TRIO_COMPILER_ANCIENT)
# include <varargs.h>
#else
# include <stdarg.h>
#endif
#include <math.h>
#include <limits.h>
#include <float.h>
#include <errno.h>

#include "trio.h"
#include "triop.h"
#include "trionan.h"
#if defined(TRIO_MINIMAL)
# define TRIO_STRING_PUBLIC static
# include "triostr.c"
#else
# include "triostr.h"
#endif
#undef printf

#if TRIO_WIDECHAR
# include <wchar.h>
#endif

#define QUOTE(x) #x

#define DOUBLE_EQUAL(x,y) (((x)>(y)-DBL_EPSILON) && ((x)<(y)+DBL_EPSILON))
#define FLOAT_EQUAL(x,y) (((x)>(y)-FLT_EPSILON) && ((x)<(y)+FLT_EPSILON))

static TRIO_CONST char rcsid[] = "@(#)$Id: regression.c,v 1.1 2003/11/25 18:50:07 kaben Exp $";


/*************************************************************************
 *
 */
static void
Dump
TRIO_ARGS2((buffer, rc),
	   char *buffer,
	   int rc)
{
  if (rc < 0)
    {
      printf("Err = %d (%s), Pos = %d\n",
	     TRIO_ERROR_CODE(rc),
	     TRIO_ERROR_NAME(rc),
	     TRIO_ERROR_POSITION(rc));
    }
  else if (buffer)
    printf("buffer[% 3d] = \"%s\"\n", rc, buffer);
}

/*************************************************************************
 *
 */
static void
Report0
TRIO_ARGS2((file, line),
          TRIO_CONST char *file,
          int line)
{
  printf("Verification failed in %s:%d.\n", file, line);
}

/*************************************************************************
 *
 */
static void
Report
TRIO_ARGS4((file, line, expected, got),
	   TRIO_CONST char *file,
	   int line,
	   TRIO_CONST char *expected,
	   TRIO_CONST char *got)
{
  Report0(file, line);
  printf("  Expected \"%s\"\n", expected);
  printf("  Got      \"%s\"\n", got);
}

/*************************************************************************
 *
 */
int
Verify
TRIO_VARGS5((file, line, result, fmt, va_alist),
	    TRIO_CONST char *file,
	    int line,
	    TRIO_CONST char *result,
	    TRIO_CONST char *fmt,
	    TRIO_VA_DECL)
{
  int rc;
  va_list args;
  char buffer[4096];

  TRIO_VA_START(args, fmt);
  rc = trio_vsnprintf(buffer, sizeof(buffer), fmt, args);
  if (rc < 0)
    Dump(buffer, rc);
  TRIO_VA_END(args);

  if (!trio_equal_case(result, buffer))
    {
      Report(file, line, result, buffer);
      return 1;
    }
  return 0;
}

/*************************************************************************
 *
 */
int
VerifyReturnValues(TRIO_NOARGS)
{
  int nerrors = 0;
  int rc;
  int count;
  char *expected;
  char buffer[4096];
  char result[4096];

  rc = trio_sprintf(buffer, "%s%n", "0123456789", &count);
  trio_sprintf(result, "%d %d %s", rc, count, buffer);
  expected = "10 10 0123456789";
  if (!trio_equal_case(result, expected))
    {
      nerrors++;
      Report(__FILE__, __LINE__, expected, result);
    }
  
  rc = trio_snprintf(buffer, sizeof(buffer), "%s%n", "0123456789", &count);
  trio_sprintf(result, "%d %d %s", rc, count, buffer);
  expected = "10 10 0123456789";
  if (!trio_equal_case(result, expected))
    {
      nerrors++;
      Report(__FILE__, __LINE__, expected, result);
    }
  
  rc = trio_snprintf(buffer, 4, "%s%n", "0123456789", &count);
  trio_sprintf(result, "%d %d %s", rc, count, buffer);
  expected = "10 3 012";
  if (!trio_equal_case(result, expected))
    {
      nerrors++;
      Report(__FILE__, __LINE__, expected, result);
    }

  /* The output buffer contains the empty string */
  rc = trio_snprintf(buffer, 1, "%s%n", "0123456789", &count);
  trio_sprintf(result, "%d %d %s", rc, count, buffer);
  expected = "10 0 ";
  if (!trio_equal_case(result, expected))
    {
      nerrors++;
      Report(__FILE__, __LINE__, expected, result);
    }

  /* The output buffer should be left untouched when max size is 0 */
  trio_sprintf(buffer, "DO NOT TOUCH");
  rc = trio_snprintf(buffer, 0, "%s%n", "0123456789", &count);
  trio_sprintf(result, "%d %d %s", rc, count, buffer);
  expected = "10 0 DO NOT TOUCH";
  if (!trio_equal_case(result, expected))
    {
      nerrors++;
      Report(__FILE__, __LINE__, expected, result);
    }
  
  return nerrors;
}

/*************************************************************************
 *
 */
#define TEST_STRING "0123456789"

int
VerifyAllocate(TRIO_NOARGS)
{
  int nerrors = 0;
  int rc;
  char *string;
  int count;
  int test_size = sizeof(TEST_STRING) - 1;

  /* Allocate a string with the result */
  rc = trio_asprintf(&string, "%s%n", TEST_STRING, &count);
  if (rc < 0)
    {
      nerrors++;
      Dump(string, rc);
    }
  else if (count != test_size)
    {
      nerrors++;
      printf("Validation failed in %s:%d\n", __FILE__, __LINE__);
      printf("  Expected %%n = %d\n", test_size);
      printf("  Got      %%n = %d\n", count);
    }
  else if (!trio_equal_case(string, TEST_STRING))
    {
      nerrors++;
      Report(__FILE__, __LINE__, TEST_STRING, string);
    }
  if (string)
    free(string);

  return nerrors;
}


/*************************************************************************
 *
 */
int
VerifyFormatting(TRIO_NOARGS)
{
  int nerrors = 0;
  char buffer[256];

  /* Normal text */
  nerrors += Verify(__FILE__, __LINE__, "Hello world",
		   "Hello world");
  /* String */
  nerrors += Verify(__FILE__, __LINE__, "Hello world",
		   "%s", "Hello world");
  /* Pointer */
  if (sizeof(void *) == 4)
    {
      nerrors += Verify(__FILE__, __LINE__, "Pointer 0x01234567",
			"Pointer %p", 0x1234567);
    }
  else if (sizeof(void *) == 8)
    {
      nerrors += Verify(__FILE__, __LINE__, "Pointer 0x0123456789012345",
			"Pointer %p", 0x123456789012345);
    }
  /* Nil pointer */
  nerrors += Verify(__FILE__, __LINE__, "Pointer (nil)",
		   "Pointer %p", NULL);
  /* Integer */
  nerrors += Verify(__FILE__, __LINE__, "Number 42",
		   "Number %d", 42);
  nerrors += Verify(__FILE__, __LINE__, "Number -42",
		   "Number %d", -42);
  nerrors += Verify(__FILE__, __LINE__, "Number 42",
		   "Number %ld", 42L);
  nerrors += Verify(__FILE__, __LINE__, "Number -42",
		   "Number %ld", -42L);
  /* Integer width */
  nerrors += Verify(__FILE__, __LINE__, "  1234",
		    "%6d", 1234);
  nerrors += Verify(__FILE__, __LINE__, "  1234",
		    "%*d", 6, 1234);
  /* Integer width overrun */
  nerrors += Verify(__FILE__, __LINE__, "123456",
		    "%4d", 123456);
  /* Integer precision */
  nerrors += Verify(__FILE__, __LINE__, "0012",
		    "%.4d", 12);
  nerrors += Verify(__FILE__, __LINE__, "0012",
		    "%.*d", 4, 12);
  nerrors += Verify(__FILE__, __LINE__, "  0012",
		    "%6.*d", 4, 12);
  nerrors += Verify(__FILE__, __LINE__, "  0012",
		    "%*.*d", 6, 4, 12);
  nerrors += Verify(__FILE__, __LINE__, "  0012",
		    "%*.*.*d", 6, 4, 2, 12);
  nerrors += Verify(__FILE__, __LINE__, "  0012",
		    "%*.*.*i", 6, 4, 10, 12);
  /* Integer sign, zero-padding, and width */
  nerrors += Verify(__FILE__, __LINE__, "+01234",
		    "%+06d", 1234);
  nerrors += Verify(__FILE__, __LINE__, " 01234",
		    "% 06d", 1234);
  nerrors += Verify(__FILE__, __LINE__, "+01234",
		    "% +06d", 1234);
  /* Integer adjust, zero-padding, and width */
  nerrors += Verify(__FILE__, __LINE__, "12      ",
		    "%-08d", 12);
  /* Integer zero-padding, width, and precision */
  nerrors += Verify(__FILE__, __LINE__, "  000012",
		    "%08.6d", 12);
  /* Integer base */
  nerrors += Verify(__FILE__, __LINE__, "42",
		   "%u", 42);
  nerrors += Verify(__FILE__, __LINE__, "-1",
		   "%d", -1);
  nerrors += Verify(__FILE__, __LINE__, "52",
		   "%o", 42);
  nerrors += Verify(__FILE__, __LINE__, "052",
		   "%#o", 42);
  nerrors += Verify(__FILE__, __LINE__, "2a",
		    "%x", 42);
  nerrors += Verify(__FILE__, __LINE__, "2A",
		    "%X", 42);
  nerrors += Verify(__FILE__, __LINE__, "0x2a",
		   "%#x", 42);
  nerrors += Verify(__FILE__, __LINE__, "0X2A",
		   "%#X", 42);
  nerrors += Verify(__FILE__, __LINE__, "0x00c ",
		   "%-#6.3x", 12);
  sprintf(buffer, "%u", UINT_MAX);
  nerrors += Verify(__FILE__, __LINE__, buffer,
		   "%u", -1);
  sprintf(buffer, "%x", UINT_MAX);
  nerrors += Verify(__FILE__, __LINE__, buffer,
		    "%x", -1);
  /* Double */
  nerrors += Verify(__FILE__, __LINE__, "3141.000000",
		    "%f", 3141.0);
  nerrors += Verify(__FILE__, __LINE__, "3141.500000",
		    "%f", 3141.5);
  nerrors += Verify(__FILE__, __LINE__, "3.141000e+03",
		    "%e", 3141.0);
  nerrors += Verify(__FILE__, __LINE__, "     -2.3420e-02",
		    "%16.4e", -2.342E-02);
  nerrors += Verify(__FILE__, __LINE__, "     -2.3420e-22",
		    "%16.4e", -2.342E-22);
  nerrors += Verify(__FILE__, __LINE__, "      2.3420e-02",
		    "% 16.4e", 2.342E-02);
  nerrors += Verify(__FILE__, __LINE__, " 2.3420e-02",
		    "% 1.4e", 2.342E-02);
  nerrors += Verify(__FILE__, __LINE__, "3.141000E-44",
		    "%E", 3.141e-44);
  nerrors += Verify(__FILE__, __LINE__, "0",
		    "%g", 0.0);
  nerrors += Verify(__FILE__, __LINE__, "-0",
		    "%g", trio_nzero());
  nerrors += Verify(__FILE__, __LINE__, "3141.5",
		    "%g", 3141.5);
  nerrors += Verify(__FILE__, __LINE__, "3.1415E-06",
		    "%G", 3.1415e-6);
  nerrors += Verify(__FILE__, __LINE__, "+3141.000000",
		    "%+f", 3141.0);
  nerrors += Verify(__FILE__, __LINE__, "-3141.000000",
		    "%+f", -3141.0);
  nerrors += Verify(__FILE__, __LINE__, "0.333333",
		    "%f", 1.0/3.0);
  nerrors += Verify(__FILE__, __LINE__, "0.666667",
		    "%f", 2.0/3.0);
  /* Beyond accuracy */
  nerrors += Verify(__FILE__, __LINE__, "0.000000",
		    "%f", 1.234567890123456789e-20);
  nerrors += Verify(__FILE__, __LINE__, "1.3999999999999999111821580299875",
		    "%.32g", 1.4);
  nerrors += Verify(__FILE__, __LINE__, "1.39999999999999991118215802998748",
		    "%.32f", 1.4);
  nerrors += Verify(__FILE__, __LINE__, "1.3999999999999999111821580300",
		    "%.28f", 1.4);
  nerrors += Verify(__FILE__, __LINE__, "1.399999999999999911182158",
		    "%.24f", 1.4);
  nerrors += Verify(__FILE__, __LINE__, "1.39999999999999991",
		    "%.17f", 1.4);
  nerrors += Verify(__FILE__, __LINE__, "1.40000000000000",
		    "%.14f", 1.4);
  nerrors += Verify(__FILE__, __LINE__, "39413.800000000002910383045673370361",
		    "%.30f", 39413.80);
  /* 2^-1 + 2^-15 */
  nerrors += Verify(__FILE__, __LINE__, "0.500030517578125",
		    "%.*g", DBL_DIG + 10, 0.500030517578125);
  /* Double decimal point */
  nerrors += Verify(__FILE__, __LINE__, "3141",
		    "%.0f", 3141.0);
  nerrors += Verify(__FILE__, __LINE__, "3141.000",
		    "%.3f", 3141.0);
  nerrors += Verify(__FILE__, __LINE__, "3141.000000",
		    "%#f", 3141.0);
  nerrors += Verify(__FILE__, __LINE__, "0.0000",
		    "%#.4f", 0.0);
  nerrors += Verify(__FILE__, __LINE__, "0.000",
		    "%#.4g", 0.0);
  nerrors += Verify(__FILE__, __LINE__, "3141.0000",
		    "%#.4f", 3141.0);
  nerrors += Verify(__FILE__, __LINE__, "3141.",
		    "%#.0f", 3141.0);
  nerrors += Verify(__FILE__, __LINE__, "11.0000",
		    "%#.4f", 11.0);
  nerrors += Verify(__FILE__, __LINE__, "100.00",
		    "%.2f", 99.9999);
  nerrors += Verify(__FILE__, __LINE__, "100",
		    "%.2g", 99.9999);
  nerrors += Verify(__FILE__, __LINE__, "100.",
		    "%#.2g", 99.9999);
  nerrors += Verify(__FILE__, __LINE__, "0.123",
		    "%0g", 0.123);
  nerrors += Verify(__FILE__, __LINE__, "1.00e+00",
		    "%.2e", 0.9999);
  nerrors += Verify(__FILE__, __LINE__, "1",
		    "%.2g", 0.9999);
  nerrors += Verify(__FILE__, __LINE__, "0.01",
		    "%.2g", 0.01);
  nerrors += Verify(__FILE__, __LINE__, "0.010",
		    "%#.2g", 0.01);
  /* Double width, precision, and zero padding */
  nerrors += Verify(__FILE__, __LINE__, "00003.141500e+03",
		    "%016e", 3141.5);
  nerrors += Verify(__FILE__, __LINE__, "    3.141500e+03",
		    "%16e", 3141.5);
  nerrors += Verify(__FILE__, __LINE__, "3.141500e+03    ",
		    "%-16e", 3141.5);
  nerrors += Verify(__FILE__, __LINE__, "03.142e+03",
		    "%010.3e", 3141.5);
#if !defined(TRIO_COMPILER_ANCIENT)
  /* Long double */
  nerrors += Verify(__FILE__, __LINE__, "1.400000",
		    "%Lf", 1.4L);
#endif
  
  /* Special cases */
  nerrors += Verify(__FILE__, __LINE__, "1.00",
		    "%.2f", 0.999);
  nerrors += Verify(__FILE__, __LINE__, "100",
		    "%.0f", 99.9);
  nerrors += Verify(__FILE__, __LINE__, "inf",
		    "%f", trio_pinf());
  nerrors += Verify(__FILE__, __LINE__, "-inf",
		    "%f", trio_ninf());
  nerrors += Verify(__FILE__, __LINE__, "INF",
		    "%F", trio_pinf());
  nerrors += Verify(__FILE__, __LINE__, "-INF",
		    "%F", trio_ninf());
  /* May fail if NaN is unsupported */
  nerrors += Verify(__FILE__, __LINE__, "nan",
		    "%f", trio_nan());
  nerrors += Verify(__FILE__, __LINE__, "NAN",
		    "%F", trio_nan());
  
  /* Char width alignment */
  nerrors += Verify(__FILE__, __LINE__, "Char X   .",
	 "Char %-4c.", 'X');
  /* String width / precision */
  nerrors += Verify(__FILE__, __LINE__, " testing",
		    "%8s", "testing");
  nerrors += Verify(__FILE__, __LINE__, "testing ",
		    "%-8s", "testing");
  nerrors += Verify(__FILE__, __LINE__, " testing",
		    "%*s", 8, "testing");
  nerrors += Verify(__FILE__, __LINE__, "test",
		    "%.4s", "testing");
  nerrors += Verify(__FILE__, __LINE__, "test",
		    "%.*s", 4, "testing");
#if TRIO_UNIX98
  /* Positional */
  nerrors += Verify(__FILE__, __LINE__, "222 111",
		    "%2$s %1$s", "111", "222");
  nerrors += Verify(__FILE__, __LINE__, "123456    12345 0001234  00123",
		    "%4$d %3$*8$d %2$.*7$d %1$*6$.*5$d",
		    123, 1234, 12345, 123456, 5, 6, 7, 8);
#endif
  
#if TRIO_GNU
  nerrors += Verify(__FILE__, __LINE__, "256",
		    "%Zd", sizeof(buffer));
  errno = EINTR;
  nerrors += Verify(__FILE__, __LINE__, "Interrupted system call",
		    "%m");
#endif
  
#if TRIO_BSD || TRIO_GNU
  /* This may fail if the preprocessor does not recognize LL */
  nerrors += Verify(__FILE__, __LINE__, "42",
		    "%qd", 42LL);
#endif

#if TRIO_C99
  nerrors += Verify(__FILE__, __LINE__, "0x2.a00000p+1",
		    "%a", 42.0);
  nerrors += Verify(__FILE__, __LINE__, "-0x2.a00000p+1",
		    "%a", -42.0);
  nerrors += Verify(__FILE__, __LINE__, "0x1.800000p+0",
		    "%a", 1.5);
  nerrors += Verify(__FILE__, __LINE__, "0x1.666666p+0",
		    "%a", 1.4);
  nerrors += Verify(__FILE__, __LINE__, "0xc.450000p+2",
		    "%a", 3141.0);
  nerrors += Verify(__FILE__, __LINE__, "0XC.450000P+2",
		    "%A", 3141.0);
  nerrors += Verify(__FILE__, __LINE__, "0xb.351c43p-25",
		    "%a", 3.141e-44);
  nerrors += Verify(__FILE__, __LINE__, "256",
		    "%zd", sizeof(buffer));
  nerrors += Verify(__FILE__, __LINE__, "42",
		    "%td", 42);
  /* Some compilers may not handle the LL suffix correctly */
  nerrors += Verify(__FILE__, __LINE__, "42",
		    "%jd", 42LL);
#endif

#if TRIO_WIDECHAR
  nerrors += Verify(__FILE__, __LINE__, "Hello World",
		    "%ls", L"Hello World");
  nerrors += Verify(__FILE__, __LINE__, "\\aHello World",
		    "%#ls", L"\aHello World");
  nerrors += Verify(__FILE__, __LINE__, "A",
		    "%lc", L'A');
  nerrors += Verify(__FILE__, __LINE__, "\\a",
		    "%#lc", L'\a');
#endif

#if TRIO_MICROSOFT
  nerrors += Verify(__FILE__, __LINE__, "42",
		    "%I8d", 42);
  nerrors += Verify(__FILE__, __LINE__, "ffffffff",
		    "%I16x", -1);
#endif
  
#if TRIO_EXTENSION
  nerrors += Verify(__FILE__, __LINE__, "  42   86",
		    "%!4d %d", 42, 86);
  nerrors += Verify(__FILE__, __LINE__, "0042 0086",
		    "%!04d %d", 42, 86);
  nerrors += Verify(__FILE__, __LINE__, "42",
		    "%&d", sizeof(long), 42L);
  /* Non-printable string */
  nerrors += Verify(__FILE__, __LINE__, "NonPrintable \\x01 \\a \\\\",
		    "NonPrintable %#s", "\01 \07 \\");
  nerrors += Verify(__FILE__, __LINE__, "\\a \\b \\t \\n \\v \\f \\r",
		    "%#s", "\007 \010 \011 \012 \013 \014 \015");
  /* Quote flag */
  nerrors += Verify(__FILE__, __LINE__, "Another \"quoted\" string",
		   "Another %'s string", "quoted");
  /* Integer base */
  nerrors += Verify(__FILE__, __LINE__, "Number 42 == 1120 (base 3)",
		    "Number %d == %..3i (base 3)", 42, 42);
  nerrors += Verify(__FILE__, __LINE__, "42",
		    "%..3d", 42);
  nerrors += Verify(__FILE__, __LINE__, "52",
		    "%..3o", 42);
  nerrors += Verify(__FILE__, __LINE__, "2a",
		    "%..3x", 42);
  /* Integer thousand separator */
  nerrors += Verify(__FILE__, __LINE__, "Number 1,000,000",
		    "Number %'d", 1000000);
  /* Float thousand separator */
  nerrors += Verify(__FILE__, __LINE__, "31,415.200000",
		    "%'f", 31415.2);
  nerrors += Verify(__FILE__, __LINE__, "1,000,000.000000",
		    "%'f", 1000000.0);
  /* Rounding modifier */
  nerrors += Verify(__FILE__, __LINE__, "1.4",
		    "%.32Rf", 1.4);
  nerrors += Verify(__FILE__, __LINE__, "1.4",
		    "%.17Rf", 1.4);
  nerrors += Verify(__FILE__, __LINE__, "39413.8",
		    "%.30Rf", 39413.80);
# if !defined(TRIO_COMPILER_ANCIENT)
  /* Long double */
  nerrors += Verify(__FILE__, __LINE__, "1.4",
		    "%RLf", 1.4L);
  nerrors += Verify(__FILE__, __LINE__, "1.4",
		    "%.30RLf", 1.4L);
# endif
#endif

#if defined(TRIO_BREESE)
  /*
   * These results depends on issues beyond our control. For example,
   * the accuracy of floating-point numbers depends on the underlying
   * floating-point hardware (e.g. whether IEEE 754 double or extended-
   * double format is used).
   *
   * These tests are therefore not part of the normal regression test,
   * but we keep them here for development purposes.
   */
  nerrors += Verify(__FILE__, __LINE__, "123456789012345680868.000000",
		    "%f", 1.234567890123456789e20);
  nerrors += Verify(__FILE__, __LINE__, "1.23456789012345677901e-20",
		    "%.20e", 1.2345678901234567e-20);
  nerrors += Verify(__FILE__, __LINE__, "0.666666666666666629659233",
		    "%.*g", DBL_DIG + 10, 2.0/3.0);
  nerrors += Verify(__FILE__, __LINE__, "123456789012345700000",
		    "%Rf", 1.234567890123456789e20);
# if !defined(TRIO_COMPILER_ANCIENT)
  nerrors += Verify(__FILE__, __LINE__, "0.666666666666666667",
		    "%RLf", (2.0L/3.0L));
  nerrors += Verify(__FILE__, __LINE__, "0.666666666666666667",
		    "%.30RLf", (2.0L/3.0L));
# endif
#endif
  
  return nerrors;
}

/*************************************************************************
 *
 */
int
VerifyErrors(TRIO_NOARGS)
{
  char buffer[512];
  int rc;
  int nerrors = 0;
  
  /* Error: Invalid argument 1 */
  rc = trio_snprintf(buffer, sizeof(buffer), "%d %r", 42, "text");
#if TRIO_ERRORS
  trio_snprintf(buffer, sizeof(buffer), "Err = %d (%s), Pos = %d",
		TRIO_ERROR_CODE(rc),
		TRIO_ERROR_NAME(rc),
		TRIO_ERROR_POSITION(rc));
  nerrors += Verify(__FILE__, __LINE__, "Err = 2 (Invalid argument), Pos = 5",
		    "%s", buffer);
#else
  nerrors += (rc != -1);
#endif
  /* Error: Invalid argument 2 */
  rc = trio_snprintf(buffer, sizeof(buffer), "%#");
#if TRIO_ERRORS
  trio_snprintf(buffer, sizeof(buffer), "Err = %d (%s), Pos = %d",
		TRIO_ERROR_CODE(rc),
		TRIO_ERROR_NAME(rc),
		TRIO_ERROR_POSITION(rc));
  nerrors += Verify(__FILE__, __LINE__, "Err = 2 (Invalid argument), Pos = 3",
		    "%s", buffer);
#else
  nerrors += (rc != -1);
#endif
  /* Error: Invalid argument 3 */
  rc = trio_snprintf(buffer, sizeof(buffer), "%hhhd", 42);
#if TRIO_ERRORS
  trio_snprintf(buffer, sizeof(buffer), "Err = %d (%s), Pos = %d",
		TRIO_ERROR_CODE(rc),
		TRIO_ERROR_NAME(rc),
		TRIO_ERROR_POSITION(rc));
  nerrors += Verify(__FILE__, __LINE__, "Err = 2 (Invalid argument), Pos = 4",
		    "%s", buffer);
#else
  nerrors += (rc != -1);
#endif
  /* Error: Double reference */
  rc = trio_snprintf(buffer, sizeof(buffer), "hello %1$d %1$d", 31, 32);
#if TRIO_ERRORS
  trio_snprintf(buffer, sizeof(buffer), "Err = %d (%s), Pos = %d",
		TRIO_ERROR_CODE(rc),
		TRIO_ERROR_NAME(rc),
		TRIO_ERROR_POSITION(rc));
# if TRIO_UNIX98
  nerrors += Verify(__FILE__, __LINE__, "Err = 4 (Double reference), Pos = 0",
		    "%s", buffer);
# else
  nerrors += Verify(__FILE__, __LINE__, "Err = 2 (Invalid argument), Pos = 9",
		    "%s", buffer);
# endif
#else
  nerrors += (rc != -1);
#endif
  /* Error: Reference gap */
  rc = trio_snprintf(buffer, sizeof(buffer), "%3$d %1$d", 31, 32, 33);
#if TRIO_ERRORS
  trio_snprintf(buffer, sizeof(buffer), "Err = %d (%s), Pos = %d",
		TRIO_ERROR_CODE(rc),
		TRIO_ERROR_NAME(rc),
		TRIO_ERROR_POSITION(rc));
# if TRIO_UNIX98
  nerrors += Verify(__FILE__, __LINE__, "Err = 5 (Reference gap), Pos = 1",
		    "%s", buffer);
# else
  nerrors += Verify(__FILE__, __LINE__, "Err = 2 (Invalid argument), Pos = 3",
		    "%s", buffer);
# endif
#else
  nerrors += (rc != -1);
#endif
  
  return nerrors;
}

/*************************************************************************
 *
 */
int
VerifyScanningOneInteger
TRIO_ARGS5((file, line, expected, format, original),
	   TRIO_CONST char *file,
	   int line,
	   TRIO_CONST char *expected,
	   TRIO_CONST char *format,
	   int original)
{
  int number;
  char data[512];
  
  trio_snprintf(data, sizeof(data), format, original);
  trio_sscanf(data, format, &number);
  return Verify(file, line, expected, format, number);
}

int
VerifyScanningIntegers(TRIO_NOARGS)
{
  int nerrors = 0;

  nerrors += VerifyScanningOneInteger(__FILE__, __LINE__, "42",
				      "%i", 42);
  nerrors += VerifyScanningOneInteger(__FILE__, __LINE__, "42",
				      "%d", 42);
  nerrors += VerifyScanningOneInteger(__FILE__, __LINE__, "-42",
				      "%d", -42);
  nerrors += VerifyScanningOneInteger(__FILE__, __LINE__, "2147483647",
				      "%d", 2147483647);
  nerrors += VerifyScanningOneInteger(__FILE__, __LINE__, "42",
				      "%u", 42);
  nerrors += VerifyScanningOneInteger(__FILE__, __LINE__, "2a",
				      "%x", 42);
  nerrors += VerifyScanningOneInteger(__FILE__, __LINE__, "52",
				      "%o", 42);
  nerrors += VerifyScanningOneInteger(__FILE__, __LINE__, "101010",
				      "%..2i", 42);
  nerrors += VerifyScanningOneInteger(__FILE__, __LINE__, "0x2a",
				      "%#x", 42);
  nerrors += VerifyScanningOneInteger(__FILE__, __LINE__, "052",
				      "%#o", 42);

  return nerrors;
}

/*************************************************************************
 *
 */
int
VerifyScanningOneFloat
TRIO_ARGS5((file, line, expected, format, original),
	   TRIO_CONST char *file,
	   int line,
	   TRIO_CONST char *expected,
	   TRIO_CONST char *format,
	   double original)
{
  double number;
  char data[512];
  
  trio_snprintf(data, sizeof(data), format, original);
  trio_sscanf(data, format, &number);
  return Verify(file, line, expected, format, number);
}

int
VerifyScanningFloats(TRIO_NOARGS)
{
  int nerrors = 0;

  nerrors += VerifyScanningOneFloat(__FILE__, __LINE__, "42.000000",
				      "%f", 42.0);
  nerrors += VerifyScanningOneFloat(__FILE__, __LINE__, "-42.000000",
				      "%f", -42.0);
  nerrors += VerifyScanningOneFloat(__FILE__, __LINE__, "4.200000e+01",
				      "%e", 42.0);
  nerrors += VerifyScanningOneFloat(__FILE__, __LINE__, "4.200000E+01",
				      "%E", 42.0);
  nerrors += VerifyScanningOneFloat(__FILE__, __LINE__, "42",
				      "%g", 42.0);
  nerrors += VerifyScanningOneFloat(__FILE__, __LINE__, "1.23457e+06",
				      "%g", 1234567.0);
  nerrors += VerifyScanningOneFloat(__FILE__, __LINE__, "1.23457e-06",
				      "%g", 1.234567e-6);
  nerrors += VerifyScanningOneFloat(__FILE__, __LINE__, "1.23457E+06",
				      "%G", 1234567.0);
  nerrors += VerifyScanningOneFloat(__FILE__, __LINE__, "1.234567e+06",
				      "%12e", 1234567.0);
  nerrors += VerifyScanningOneFloat(__FILE__, __LINE__, "1.234500e+00",
				      "%6e", 1234567.0);
  nerrors += VerifyScanningOneFloat(__FILE__, __LINE__, "1.234567e+06",
				      "%.6e", 1234567.0);
  nerrors += VerifyScanningOneFloat(__FILE__, __LINE__, "1.2345670000e+06",
				      "%.10e", 1234567.0);
  nerrors += VerifyScanningOneFloat(__FILE__, __LINE__, "1.23457e+06",
				      "%.6g", 1234567.0);
  nerrors += VerifyScanningOneFloat(__FILE__, __LINE__, "1234567",
				      "%.10g", 1234567.0);
#if TRIO_C99
  nerrors += VerifyScanningOneFloat(__FILE__, __LINE__, "0x2.a00000p+1",
				      "%a", 42.0);
  nerrors += VerifyScanningOneFloat(__FILE__, __LINE__, "0x1.2d6870p+5",
				      "%a", 1234567.0);
  nerrors += VerifyScanningOneFloat(__FILE__, __LINE__, "0X1.2D6870P+5",
				      "%A", 1234567.0);
#endif
  nerrors += VerifyScanningOneFloat(__FILE__, __LINE__, "1.79769e+308",
				      "%g", 1.79769e+308);
  nerrors += VerifyScanningOneFloat(__FILE__, __LINE__, "nan",
				      "%f", trio_nan());
  nerrors += VerifyScanningOneFloat(__FILE__, __LINE__, "NAN",
				      "%F", trio_nan());
  nerrors += VerifyScanningOneFloat(__FILE__, __LINE__, "-inf",
				      "%f", trio_ninf());
  
  return nerrors;
}

/*************************************************************************
 *
 */
int
VerifyScanningOneString
TRIO_ARGS5((file, line, expected, format, original),
	   TRIO_CONST char *file,
	   int line,
	   TRIO_CONST char *expected,
	   TRIO_CONST char *format,
	   char *original)
{
  char string[512];
  char data[512];
  
  trio_snprintf(data, sizeof(data), "%s", original);
  trio_sscanf(data, format, string);
  return Verify(file, line, expected, "%s", string);
}

int
VerifyScanningStrings(TRIO_NOARGS)
{
  int nerrors = 0;

  nerrors += VerifyScanningOneString(__FILE__, __LINE__, "hello",
				     "%s", "hello");
  nerrors += VerifyScanningOneString(__FILE__, __LINE__, "hello",
				     "%s", "hello world");
  nerrors += VerifyScanningOneString(__FILE__, __LINE__, "hello world",
				     "%[^\n]", "hello world");
  nerrors += VerifyScanningOneString(__FILE__, __LINE__, "(nil)",
				     "%s", NULL);
  nerrors += VerifyScanningOneString(__FILE__, __LINE__, "hello",
				     "%20s", "hello");
  nerrors += VerifyScanningOneString(__FILE__, __LINE__, "he",
				     "%2s", "hello");
  nerrors += VerifyScanningOneString(__FILE__, __LINE__, "ab",
				     "%[ab]", "abcba");
  nerrors += VerifyScanningOneString(__FILE__, __LINE__, "abcba",
				     "%[abc]", "abcba");
  nerrors += VerifyScanningOneString(__FILE__, __LINE__, "abcba",
				     "%[a-c]", "abcba");
#if TRIO_EXTENSION
  nerrors += VerifyScanningOneString(__FILE__, __LINE__, "abcba",
				     "%[[:alpha:]]", "abcba");
#endif
  nerrors += VerifyScanningOneString(__FILE__, __LINE__, "ba",
				     "%*[ab]c%[^\n]", "abcba");

  return nerrors;
}

/*************************************************************************
 *
 */
int
VerifyScanningRegression(TRIO_NOARGS)
{
  int nerrors = 0;
  int rc;
  int index;
  double dnumber;
  trio_long_double_t ldnumber;
  long lnumber;
  int number;
  char ch;

  rc = trio_sscanf("1.5", "%lf%n", &dnumber, &index);
  nerrors += Verify(__FILE__, __LINE__, "1 3 1.500000",
		    "%d %d %f", rc, index, dnumber);
  rc = trio_sscanf("q 123", "%c%ld", &ch, &lnumber);
  nerrors += Verify(__FILE__, __LINE__, "q 123",
		    "%c %ld", ch, lnumber);
  rc = trio_sscanf("abc", "%*s%n", &number);
  nerrors += Verify(__FILE__, __LINE__, "0 3",
		    "%d %d", rc, number);
  rc = trio_sscanf("abc def", "%*s%n", &number);
  nerrors += Verify(__FILE__, __LINE__, "0 3",
		    "%d %d", rc, number);
  rc = trio_sscanf("0.141882295971771490", "%lf", &dnumber);
  number = 33;
  rc = trio_sscanf("total 1", "total %d", &number);
  nerrors += Verify(__FILE__, __LINE__, "1 1",
		    "%d %d", rc, number);
#if defined(TRIO_BREESE)
  nerrors += Verify(__FILE__, __LINE__, "1 0.141882295971771488",
		    "%d %.18f", rc, dnumber);
  rc = trio_sscanf("0.141882295971771490", "%Lf", &ldnumber);
  nerrors += Verify(__FILE__, __LINE__, "1 0.141882295971771490",
		    "%d %.18Lf", rc, ldnumber);
#endif
  return nerrors;
}

/*************************************************************************
 *
 */
int
VerifyScanning(TRIO_NOARGS)
{
  int nerrors = 0;

  nerrors += VerifyScanningIntegers();
  nerrors += VerifyScanningFloats();
  nerrors += VerifyScanningStrings();
  nerrors += VerifyScanningRegression();
  
  return nerrors;
}

/*************************************************************************
 *
 */
int
VerifyStrings(TRIO_NOARGS)
{
  int nerrors = 0;
#if !defined(TRIO_MINIMAL)
  char buffer[512];
  double dnumber;
  float fnumber;
  char *end;

  /* Comparison */
  trio_copy(buffer, "Find me now");
  if (trio_length(buffer) != sizeof("Find me now") - 1) {
    nerrors++;
    Report0(__FILE__, __LINE__);
  }
  if (!trio_equal(buffer, "Find me now")) {
    nerrors++;
    Report0(__FILE__, __LINE__);
  }
  if (!trio_equal_case(buffer, "Find me now")) {
    nerrors++;
    Report0(__FILE__, __LINE__);
  }
  if (trio_equal_case(buffer, "FIND ME NOW")) {
    nerrors++;
    Report0(__FILE__, __LINE__);
  }
  if (!trio_equal_max(buffer, sizeof("Find me") - 1, "Find ME")) {
    nerrors++;
    Report0(__FILE__, __LINE__);
  }
  if (!trio_contains(buffer, "me")) {
    nerrors++;
    Report0(__FILE__, __LINE__);
  }
  if (trio_contains(buffer, "and me")) {
    nerrors++;
    Report0(__FILE__, __LINE__);
  }
  if (trio_substring(buffer, "me") == NULL) {
    nerrors++;
    Report0(__FILE__, __LINE__);
  }
  if (trio_substring_max(buffer, 4, "me") != NULL) {
    nerrors++;
    Report0(__FILE__, __LINE__);
  }
  if (!trio_match(buffer, "* me *")) {
    nerrors++;
    Report0(__FILE__, __LINE__);
  }
  if (trio_match_case(buffer, "* ME *")) {
    nerrors++;
    Report0(__FILE__, __LINE__);
  }
  if (trio_index(buffer, 'n') == NULL) {
    nerrors++;
    Report0(__FILE__, __LINE__);
  }
  if (trio_index(buffer, '_') != NULL) {
    nerrors++;
    Report0(__FILE__, __LINE__);
  }
  if (trio_index_last(buffer, 'n') == NULL) {
    nerrors++;
    Report0(__FILE__, __LINE__);
  }

  /* Append */
  trio_copy(buffer, "Find me now");
  if (!trio_append(buffer, " and again")) {
    nerrors++;
    Report0(__FILE__, __LINE__);
  }
  if (!trio_equal(buffer, "Find me now and again")) {
    nerrors++;
    Report0(__FILE__, __LINE__);
  }
  if (!trio_append_max(buffer, 0, "should not appear")) {
    nerrors++;
    Report0(__FILE__, __LINE__);
  }
  if (!trio_equal(buffer, "Find me now and again")) {
    nerrors++;
    Report0(__FILE__, __LINE__);
  }
  
  /* To upper/lower */
  trio_copy(buffer, "Find me now");
  trio_upper(buffer);
  if (!trio_equal_case(buffer, "FIND ME NOW")) {
    nerrors++;
    Report0(__FILE__, __LINE__);
  }
  trio_lower(buffer);
  if (!trio_equal_case(buffer, "find me now")) {
    nerrors++;
    Report0(__FILE__, __LINE__);
  }

  /* Double conversion */
  trio_copy(buffer, "3.1415");
  dnumber = trio_to_double(buffer, NULL);
  if (!DOUBLE_EQUAL(dnumber, 3.1415)) {
    nerrors++;
    Report0(__FILE__, __LINE__);
  }
  fnumber = trio_to_float(buffer, NULL);
  if (!FLOAT_EQUAL(fnumber, 3.1415)) {
    nerrors++;
    Report0(__FILE__, __LINE__);
  }

  /* Long conversion */
  trio_copy(buffer, "3.1415");
  if (trio_to_long(buffer, NULL, 10) != 3L) {
    nerrors++;
    Report0(__FILE__, __LINE__);
  }
  if (trio_to_long(buffer, NULL, 4) != 3L) {
    nerrors++;
    Report0(__FILE__, __LINE__);
  }
  trio_to_long(buffer, &end, 2);
  if (end != buffer) {
    nerrors++;
    Report0(__FILE__, __LINE__);
  }
  
#endif /* !defined(TRIO_MINIMAL) */
  return nerrors;
}

/*************************************************************************
 *
 */
int
VerifyDynamicStrings(TRIO_NOARGS)
{
  int nerrors = 0;
#if !defined(TRIO_MINIMAL)
  trio_string_t *string;

  string = trio_xstring_duplicate("Find me now");
  if (string == NULL) {
    nerrors++;
    goto error;
  }
  if (!trio_xstring_equal(string, "FIND ME NOW"))
    nerrors++;
  if (!trio_xstring_append(string, " and again") ||
      !trio_xstring_equal(string, "FIND ME NOW AND AGAIN"))
    nerrors++;
  if (!trio_xstring_contains(string, "me"))
    nerrors++;
  if (trio_xstring_contains(string, "ME"))
    nerrors++;
  if (!trio_xstring_match(string, "* me *"))
    nerrors++;
  if (trio_xstring_match_case(string, "* ME *"))
    nerrors++;
  
 error:
  if (string)
    trio_string_destroy(string);
  
#endif /* !defined(TRIO_MINIMAL) */
  return nerrors;
}

/*************************************************************************
 *
 */
int
VerifyNaN(TRIO_NOARGS)
{
  double ninf_number = trio_ninf();
  double pinf_number = trio_pinf();
  double nan_number = trio_nan();
  int nerrors = 0;
  
  nerrors += Verify(__FILE__, __LINE__, "-1",
		    "%d", trio_isinf(ninf_number));
  nerrors += Verify(__FILE__, __LINE__, "0",
		    "%d", trio_isinf(42.0));
  nerrors += Verify(__FILE__, __LINE__, "1",
		    "%d", trio_isinf(pinf_number));
  nerrors += Verify(__FILE__, __LINE__, "1",
		    "%d", trio_isnan(nan_number));
  nerrors += Verify(__FILE__, __LINE__, "0",
		    "%d", trio_isnan(42.0));

  return nerrors;
}

/*************************************************************************
 *
 */
int
main(TRIO_NOARGS)
{
  int nerrors = 0;

  printf("%s\n", rcsid);

  /* Override system locale settings */
  trio_locale_set_decimal_point(".");
  trio_locale_set_thousand_separator(",");
  trio_locale_set_grouping("\3");

  printf("Verifying strings\n");
  nerrors += VerifyStrings();
  
  printf("Verifying dynamic strings\n");
  nerrors += VerifyDynamicStrings();

  printf("Verifying special quantities\n");
  nerrors += VerifyNaN();
  
  printf("Verifying formatting\n");
  nerrors += VerifyFormatting();
  
  printf("Verifying scanning\n");
  nerrors += VerifyScanning();
  
  printf("Verifying return values\n");
  nerrors += VerifyErrors();
  nerrors += VerifyReturnValues();
  
  printf("Verifying allocation\n");
  nerrors += VerifyAllocate();

  if (nerrors == 0)
    printf("Regression test suceeded\n");
  else
    printf("Regression test failed in %d instance(s)\n", nerrors);
  
  return nerrors ? 1 : 0;
}
