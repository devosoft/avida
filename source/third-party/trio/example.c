/*************************************************************************
 * For testing purposes
 */

#include <stdarg.h>
#include <limits.h>
#include <math.h>
#include <unistd.h>
#include <wchar.h>
#include "triodef.h"
#include "strio.h"
#include "trio.h"
#undef printf

#if !defined(USE_LONGLONG)
# if defined(__GNUC__) && !defined(__STRICT_ANSI__)
#  define USE_LONGLONG
# elif defined(__SUNPRO_C)
#  define USE_LONGLONG
# endif
#endif

#if defined(USE_LONGLONG)
# define LONGLONG long long
#else
# define LONGLONG long
#endif

#if defined(TRIO_C99)
# define LONGEST intmax_t
#else
# define LONGEST LONGLONG
#endif

static TRIO_CONST char rcsid[] = "@(#)$Id: example.c,v 1.1 2003/11/25 18:50:07 kaben Exp $";

/*************************************************************************
 *
 */
void Dump
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
  else
    printf("buffer[% 3d] = \"%s\"\n", rc, buffer);
}

/*************************************************************************
 *
 */
int main(TRIO_NOARGS)
{
  char buffer[512];
  int rc;
  LONGLONG int dummy;
  char *fool;
  int num;
  int num2;
  int count;
  double dnum;
  float fnum, fnum2;
  trio_long_double_t ldnum;
  char *end;
  char text[256];
  char ch;
  int nerrors = 0;
  void *p1;
  char *p2;

  printf("%s\n", rcsid);

/*    printf("%d %u %d %u\n", */
/*  	 INT_MAX, INT_MAX, UINT_MAX, UINT_MAX); */
/*    trio_printf("%d %u %d %u\n", */
/*  	      INT_MAX, INT_MAX, UINT_MAX, UINT_MAX); */
/*    printf("%d %u\n", INT_MIN, INT_MIN); */
/*    trio_printf("%d %u\n", INT_MIN, INT_MIN); */
  
/*    printf("%ld %lu %ld %lu\n", */
/*  	 INT_MAX, INT_MAX, UINT_MAX, UINT_MAX); */
/*    trio_printf("%ld %lu %ld %lu\n", */
/*  	      INT_MAX, INT_MAX, UINT_MAX, UINT_MAX); */
/*    printf("%ld %lu\n", INT_MIN, INT_MIN); */
/*    trio_printf("%ld %lu\n", INT_MIN, INT_MIN); */

/*    printf("%lld %llu %lld %llu\n", */
/*  	 INT_MAX, INT_MAX, UINT_MAX, UINT_MAX); */
/*    trio_printf("%lld %llu %lld %llu\n", */
/*  	      INT_MAX, INT_MAX, UINT_MAX, UINT_MAX); */
/*    printf("%lld %llu\n", INT_MIN, INT_MIN); */
/*    trio_printf("%lld %llu\n", INT_MIN, INT_MIN); */

/*    return 0; */

  
/*    dnum = StrToDouble("3.14e+44", (const char **)&end); */
/*    printf("double = %e (%s)\n", dnum, end); */
/*    dnum = StrToDouble("0xA3.14p44", (const char **)&end); */
/*    printf("double = %e (%s)\n", dnum, end); */

  /*    trio_printf("%.*stext\n", 0, "-----"); */ /* fails */
  
/*    trio_printf("%Zd\n", (size_t)sizeof(char)); */
  
/*   rc = StrFormat(buffer, "%a", 3.14e+44); */
/*   Dump(buffer, rc); */
  
  /*  rc = StrFormat(buffer, "Filled string: %-16<fill=_>s", "test"); */

/*   errno = EINTR; */
/*   rc = StrFormat(buffer, "Error: %m"); */

/*   rc = StrFormat(buffer, "Count %lln", &dummy); */
/*   printf("dummy = %lld\n", dummy); */

/*   rc = StrFormat(buffer, "Char %<quote='>c", 'X'); */

/*   rc = StrFormatMax(buffer, 20, "Here goes %-20<adjust=_>s", "test"); */

/*    rc = StrFormat(buffer, "Hex-float %a", 3.1415); */
/*    Dump(buffer, rc); */
/*    rc = StrFormat(buffer, "Hex-float %A", 3.1415e20); */
/*    Dump(buffer, rc); */
/*    rc = StrFormat(buffer, "Double %#g", 3.1415e20); */
/*    Dump(buffer, rc); */
/*    rc = StrFormat(buffer, "Double %.3f", 3.1415); */
/*    Dump(buffer, rc); */
/*    rc = StrFormat(buffer, "Double %+e", 3.1415); */
/*    Dump(buffer, rc); */

/*    printf("'%.2f'\n", 99.99999); */
/*    trio_printf("'%.2f'\n", 99.99999); */
/*    printf("'%f'\n", 0.0); */
/*    trio_printf("'%f'\n", 0.0); */
/*    printf("'%f'\n", 3141.0); */
/*    trio_printf("'%f'\n", 3141.0); */
/*    printf("'%#f'\n", 3141.0); */
/*    trio_printf("'%#f'\n", 3141.0); */
/*    printf("'%'f'\n", 31415.2); */
/*    trio_printf("'%'f'\n", 31415.2); */
/*    printf("'%-16e'\n", 3141.5); */
/*    trio_printf("'%-16e'\n", 3141.5); */
/*    printf("'%#f'\n", 3141.0); */
/*    trio_printf("'%#f'\n", 3141.0); */
/*    printf("'%f'\n", 3141.5); */
/*    trio_printf("'%f'\n", 3141.5); */
/*    printf("'%#.6g'\n", 3141.5); */
/*    trio_printf("'%#.6g'\n", 3141.5); */
  
/*    printf("'%20e'\n", 314.5); */
/*    trio_printf("'%20e'\n", 314.5); */
  
/*    printf("'%-16e'\n", 3141.5); */
/*    trio_printf("'%-16e'\n", 3141.5); */
  
/*    printf("'%#.4g'\n", 314151.5); */
/*    trio_printf("'%#.4g'\n", 314151.5); */
  
/*    printf("'%#.4g'\n", 0.0); */
/*    trio_printf("'%#.4g'\n", 0.0); */
  
/*    printf("'%#.4g'\n", 11.0); */
/*    trio_printf("'%#.4g'\n", 11.0); */

/*    printf("%f\n", HUGE_VAL); */
/*    trio_printf("%f\n", HUGE_VAL); */
/*    printf("%f\n", -HUGE_VAL); */
/*    trio_printf("%f\n", -HUGE_VAL); */
/*  #define NAN (cos(HUGE_VAL)) */
/*    printf("%f\n", NAN); */
/*    trio_printf("%f\n", NAN); */
  
/*    printf("'%+06d'\n", 1234); */
/*    trio_printf("'%+06d'\n", 1234); */
/*    printf("'%-#6.3x'\n", 12); */
/*    trio_printf("'%-#06.3x'\n", 12); */
/*    printf("'%+6d'\n", 1234); */
/*    trio_printf("'%+6d'\n", 1234); */
/*    printf("'%-08d'\n", 12); */
/*    trio_printf("'%-08d'\n", 12); */
/*    printf("'%08.6d'\n", 12); */
/*    trio_printf("'%08.6d'\n", 12); */
/*    printf("'%4d'\n", 123456); */
/*    trio_printf("'%4d'\n", 123456); */
/*    printf("'%.4d'\n", 12); */
/*    trio_printf("'%.4d'\n", 12); */

/*    trio_printf("%!#08x %04x %..10x\n", 42, 42, 42); */
/*    trio_printf("%*.*.*i\n", 8, 4, 2, 23); */
/*    trio_printf("%8.4.2i %<base=2>08i %.8.2i %..2i\n", 23, 23, 23, 23); */

/*    trio_printf("%8i\n", 42); */
/*    trio_printf("%.7i\n", 42); */
/*    trio_printf("%..2i\n", 42); */
/*    trio_printf("%8.7i\n", 42); */
/*    trio_printf("%8..2i\n", 42); */
/*    trio_printf("%8.7.2i\n", 42); */
/*    trio_printf("%*.*.*i\n", 8, 7, 2, 42); */

/*    { */
/*      LONGLONG ll = 1234567890; */
/*      rc = trio_printf("%&i %d\n", sizeof(ll), ll, 42); */
/*      Dump(buffer, rc); */
/*    } */
/*    { */
/*      char ch = 12; */
/*      rc = trio_printf("%&i %d\n", sizeof(ch), ch, 42); */
/*      Dump(buffer, rc); */
/*    } */
/*    { */
/*      pid_t pid = 99; */
/*      rc = trio_printf("%&i %d\n", sizeof(pid), pid, 42); */
/*      Dump(buffer, rc); */
/*    } */
  
/*    rc = trio_printf("%*.*.*i\n", 6, 4, 10, 12); */
/*    Dump(buffer, rc); */
/*    rc = trio_printf("%1$0*3$.*2$d\n", 3141, 6, 10); */
/*    Dump(buffer, rc); */

/*    rc = trio_asprintf(&end, "%s%n", "0123456789", &num); */
/*    printf("%d %d '%s'\n", rc, num, end); */
/*    if (end) */
/*      free(end); */
  
/*    trio_printf("%016e\n", 3141.5); */
/*    trio_printf("%'f\n", 424242.42); */
/*    trio_printf("%#.4f\n", 0.0); */
/*    trio_printf("%'d\n", 424242); */

/*    rc = trio_sprintf(buffer, "%4$d %3$*8$d %2$.*7$d %1$*6$.*5$d\n", */
/*  		    123, */
/*  		    1234, */
/*  		    12345, */
/*  		    123456, */
/*  		    5, 6, 7, 8 */
/*  		    ); */
/*    Dump(buffer, rc); */
/*    rc = trio_sprintf(buffer, "%2$s %1$#s", "111", "222"); */
/*    Dump(buffer, rc); */
  
/*    trio_printf("  %x %!#x %g %09x %x\n", 123456, 123456, 123456.0, 123456, 123456); */
/*    trio_printf("%!'i %f %i\n", 12345, 12345.0, 12345); */
/*    trio_printf("%!<base=2>i %i\n", 23, 23); */

/*    rc = trio_sprintf(buffer, "%I32d", 12345); */
/*    Dump(buffer, rc); */
/*    rc = trio_sprintf(buffer, "%I32I8d", 12345); */
/*    Dump(buffer, rc); */

/*    rc = trio_sprintf(buffer, "*%5f*", 3.3); */
/*    Dump(buffer, rc); */
  
/*    { */
/*      wchar_t *wstring = L"some data"; */
/*      wchar_t wbuffer[512]; */
    
/*      rc = trio_sprintf(buffer, "%ls", wstring); */
/*      Dump(buffer, rc); */

/*      rc = trio_sscanf(buffer, "%ls", wbuffer); */
/*      Dump(buffer, rc); */
/*      rc = trio_sprintf(buffer, "%ls", wbuffer); */
/*      Dump(buffer, rc); */
/*    } */
  
  /* rc = StrFormat(buffer, "\040-\040\040-\n"); */

/*   rc = StrFormat(buffer, "%.*s@%s", 3, "Daniel", "Fool"); */
/*   rc = StrFormatAppendMax(buffer, 512, " %s is a doh", "Simpson"); */

/*   rc = StrFormat(buffer, "hello %1$d %1$d", 31, 32); */
/*   Dump(buffer, rc); */
/*   rc = StrFormat(buffer, "%2$d %3$d", 31, 32, 33); */
/*   Dump(buffer, rc); */
  
/*    rc = trio_sprintf(buffer, "%d say %g hey %s", 42, 3.14, "text"); */
/*    Dump(buffer, rc); */
/*    trio_sscanf(buffer, "%d %*s %e hey %s", &num, &dnum, text); */
/*    printf("num = %d, dnum = %e, text = \"%s\"\n", num, dnum, text); */

/*    rc = trio_sprintf(buffer, "%g", HUGE_VAL); */
/*    Dump(buffer, rc); */
/*    trio_sscanf(buffer, "%f", &dnum); */
/*    printf("dnum = %e\n", dnum); */

/*    rc = trio_sprintf(buffer, "%g", -HUGE_VAL); */
/*    Dump(buffer, rc); */
/*    trio_sscanf(buffer, "%f", &dnum); */
/*    printf("dnum = %e\n", dnum); */

/*  #if defined(NAN) */
/*    rc = trio_sprintf(buffer, "%g", NAN); */
/*    Dump(buffer, rc); */
/*    if ((rc = trio_sscanf(buffer, "%f", &dnum)) < 0) */
/*      Dump(buffer, rc); */
/*    else */
/*      printf("dnum = %e\n", dnum); */
/*  #endif */

/*    rc = trio_sprintf(buffer, "%*d", 6, 1234); */
/*    Dump(buffer, rc); */

/*    rc = trio_sprintf(buffer, "'%!08.6d' '%!d' '%d'", 4, 6, 8); */
/*    Dump(buffer, rc); */

/*    rc = trio_sprintf(buffer, "%0g", 0.123); */
/*    Dump(buffer, rc); */
  
/*    { */
/*      void *argarray[4]; */
/*      int value = 42; */
/*      double number = 123.456; */
/*      float small_number = 123.456; */
    
/*      argarray[0] = &value; */
/*      argarray[1] = "my string"; */
/*      rc = trio_sprintfv(buffer, "%d %s", argarray); */
/*      Dump(buffer, rc); */
/*      rc = trio_snprintfv(buffer, 8, "%d %s", argarray); */
/*      Dump(buffer, rc); */

/*      argarray[0] = &num; */
/*      argarray[1] = text; */
/*      rc = trio_sscanfv(buffer, "%d %s", argarray); */
/*      Dump(buffer, rc); */
/*      printf("num = %d  text = \"%s\"\n", num, text); */
    
/*      argarray[0] = &number; */
/*      argarray[1] = &small_number; */
/*      rc = trio_sprintfv(buffer, "%f %hf", argarray); */
/*      Dump(buffer, rc); */
/*      printf("number = %f  small_number = \"%f\"\n", number, small_number); */
/*    } */
  
/*    rc = trio_sprintf(buffer, "abcba"); */
/*    Dump(buffer, rc); */
/*    trio_sscanf(buffer, "%[ab]", text); */
/*    printf("text = \"%s\"\n", text); */
/*    trio_sscanf(buffer, "%*[ab]c%[^\n]", text); */
/*    printf("text = \"%s\"\n", text); */

/*    trio_sprintf(buffer, "aabcdba aaa"); */
/*    rc = trio_sscanf(buffer, "%s", text); */
/*    Dump(buffer, rc); */
/*    printf("text = \"%s\"\n", text); */
/*    rc = trio_sscanf(buffer, "%*1[aA]%[a-c]", text); */
/*    Dump(buffer, rc); */
/*    printf("text = \"%s\"\n", text); */

/*    rc = trio_sprintf(buffer, "10021"); */
/*    rc = trio_sscanf(buffer, "%b%n%d", &num, &count, &num2); */
/*    Dump(buffer, rc); */
/*    printf("num = %d %d %d\n", num, num2, count); */

/*    rc = trio_sprintf(buffer, "%'d", 10000); */
/*    rc = trio_sscanf(buffer, "%'d", &num); */
/*    Dump(buffer, rc); */
/*    printf("num = %d\n", num); */

/*    rc = trio_dprintf(STDOUT_FILENO, "%s\n", "hello there"); */
/*    Dump(buffer, rc); */
/*    rc = trio_dscanf(STDIN_FILENO, "%s", buffer); */
/*    Dump(buffer, rc); */

/*    rc = trio_scanf("%s", buffer); */
/*    Dump(buffer, rc); */
  
/*    rc = trio_sprintf(buffer, "Ttext"); */
/*    Dump(buffer, rc); */
/*    trio_sscanf(buffer, "%*[Tt]e%c", &ch); */
/*    printf("ch = %c\n", ch); */

/*    printf("%p\n", &main); */
/*    rc = trio_sprintf(buffer, "%p %p", &main, NULL); */
/*    Dump(buffer, rc); */
/*    trio_sscanf(buffer, "%p %p", &p1, &p2); */
/*    printf("pointer = %p %p\n", p1, p2); */

/*    rc = trio_sprintf(buffer, "%@.@.@i", 8, 7, 2, 42); */
/*    Dump(buffer, rc); */
/*    trio_sprintf(buffer, "abcdefghijklmnopqrstuvwxyz"); */
/*    rc = trio_sscanf(buffer, "%100s", text); */
/*    Dump(text, rc); */
/*    rc = trio_sscanf(buffer, "%@s", 100, text); */
/*    Dump(text, rc); */
  
/*    rc = trio_sprintf(buffer, "%..2i", 42); */
/*    Dump(buffer, rc); */
/*    rc = trio_sscanf(buffer, "%..2i", &num); */
/*    printf("%d\n", num); */
/*    rc = trio_sscanf(buffer, "%..@i", 2, &num); */
/*    printf("%d\n", num); */

/*    { */
/*      int num1, num2, num3, num4; */
    
/*      rc = trio_sprintf(buffer, "123_456 [12%%-34%%]"); */
/*      Dump(buffer, rc); */
/*      rc = trio_sscanf(buffer, "%5i%*1s%5i%*1s%5i%*2s%5i", */
/*  		     &num1, &num2, &num3, &num4); */
/*      Dump(buffer, rc); */
/*      printf("%d %d %d %d %d\n", rc, num1, num2, num3, num4); */
/*      rc = trio_sscanf(buffer, "%d_%d [%d%%-%d%%]", */
/*  		     &num1, &num2, &num3, &num4); */
/*      Dump(buffer, rc); */
/*      printf("%d %d %d %d %d\n", rc, num1, num2, num3, num4); */
/*    } */

/*    rc = trio_sprintf(buffer, "01 3456789"); */
/*    Dump(buffer, rc); */
/*    memset(&text, 0, sizeof(text)); */
/*    rc = trio_sscanf(buffer, "%4c", &text); */
/*    Dump(text, rc); */
/*    memset(&text, 0, sizeof(text)); */
/*    rc = sscanf(buffer, "%4c", &text); */
/*    Dump(text, rc); */
  
/*    rc = trio_sprintf(buffer, "12345 6"); */
/*    Dump(buffer, rc); */
/*    rc = trio_sscanf(buffer, "%2d", &num); */
/*    Dump(buffer, rc); */
/*    printf("%d\n", num); */
/*    rc = sscanf(buffer, "%2d", &num); */
/*    Dump(buffer, rc); */
/*    printf("%d\n", num); */
  
/*    rc = trio_sprintf(buffer, "aa\\x0abb"); */
/*    Dump(buffer, rc); */
/*    rc = trio_sscanf(buffer, "aa%#sbb", &text); */
/*    Dump(text, rc); */

/*    rc = trio_sscanf("0 ", "%f", &dnum, text); */
/*    printf("%d %f\n", rc, dnum); */
/*    rc = sscanf("0 ", "%f %s", &dnum, text); */
/*    printf("%d %f\n", rc, dnum); */
  
/*    rc = trio_sscanf("lære", "%[:alpha:]", text); */
/*    Dump(text, rc); */
  
/*    rc = trio_sscanf("-0.123e3", "%8e", &dnum); */
/*    printf("%d %f\n", rc, dnum); */

/*    rc = trio_sscanf("123,456.78", "%'f", &dnum); */
/*    printf("%d %f\n", rc, dnum); */

/*    trio_printf("%.24f\n%.24f\n%.24f\n", 39413.80, 1.4, (2.0/3.0)); */
/*    printf("%.24f\n%.24f\n%.24f\n", 39413.80, 1.4, (2.0/3.0)); */
  
/*    trio_printf("%f\n%f\n", (7.0/5.0), (2.0/3.0)); */
/*    printf("%f\n%f\n", (7.0/5.0), (2.0/3.0)); */

/*    trio_printf("%a\n", 42.0); */
/*    trio_printf("%.2e\n", 0.9999); */
/*    trio_printf("%e\n", 3.14e+123); */
/*    trio_printf("%010.3e\n", 3141.5); */
/*    trio_printf("%.32Rf\n", 1.4); */
/*    trio_printf("%Rf\n", 1.234567890123456789e20); */
/*    printf("%.2g\n", 0.9999); */
/*    trio_printf("%.2g\n", 0.9999); */
/*    printf("%.2g\n", 99.9999); */
/*    trio_printf("%.2g\n", 99.9999); */
/*    printf("%g\n", 0.0); */
/*    trio_printf("%g\n", 0.0); */
/*    printf("%f\n", 3141.0); */
/*    trio_printf("%f\n", 3141.0); */
/*    printf("%.0f\n", 3141.0); */
/*    trio_printf("%.0f\n", 3141.0); */
/*    trio_printf("%f\n", 2.0/3.0); */
/*    trio_printf("%f\n", 16.25); */
  
/*    trio_printf("A: %f\nB: %.20f\nC: %Rf\n", 0.1, 0.1, 0.1); */
/*    trio_printf("A: %f\nB: %.20f\nC: %Rf\n", 2.0/3.0, 2.0/3.0, 2.0/3.0); */
  
/*    trio_printf("R %.*Rhf\n", FLT_DIG + 4, (double)((float)1.4)); */
/*    trio_printf("T %.*hf\n", FLT_DIG + 4, (double)((float)(2.0/3.0))); */
/*    trio_printf("R %.*Rhf\n", FLT_DIG + 4, (double)((float)(2.0/3.0))); */
  
/*    trio_printf("T %.*f\n", DBL_DIG + 4, (2.0/3.0)); */
/*    trio_printf("R %.*Rf\n", DBL_DIG + 4, (2.0/3.0)); */
/*    printf("C %.*f\n", DBL_DIG + 4, (2.0/3.0)); */

/*    trio_printf("T %.*f\n", DBL_DIG + 4, 1.4); */
/*    trio_printf("R %.*Rf\n", DBL_DIG + 4, 1.4); */
/*    printf("C %.*f\n", DBL_DIG + 4, 1.4); */
    
/*    trio_printf("T %.*f\n", DBL_DIG + 6, 0.1); */
/*    trio_printf("R %.*Rf\n", DBL_DIG + 6, 0.1); */
/*    printf("C %.*f\n", DBL_DIG + 6, 0.1); */

/*    ldnum = 2.0L / 3.0L; */
/*    trio_printf("R %.*Lf\n", LDBL_DIG + 4, ldnum); */
/*    trio_printf("R %RLf\n", ldnum); */
  
/*    trio_printf("T %.*.2f\n", DBL_MANT_DIG, 1.4); */
/*    trio_printf("R %.*.2Rf\n", DBL_MANT_DIG, 1.4); */
  
/*    trio_printf("R %Rf\n", 1.234567890123456789e20); */
/*    trio_printf("T %10.3e\n", 3141.5); */
/*    trio_printf("T %f\n", 1.0/3.0); */

  /* 2^-1 + 2^-15 */
/*    trio_printf("T %.*g\n", DBL_DIG + 200, 0.500030517578125); */
/*    printf("C %.*g\n", DBL_DIG + 200, 0.500030517578125); */
/*    trio_printf("T %.*g\n", DBL_DIG + 200, 2.0/3.0); */
/*    printf("C %.*g\n", DBL_DIG + 200, 2.0/3.0); */
  
/*    trio_printf("T  %.*f\n", DBL_DIG + 2, 1.4); */
/*    trio_printf("TR %.*Rf\n", DBL_DIG + 2, 1.4); */
  
/*    rc = trio_sscanf("0.141882295971771490", "%Lf", &ldnum); */
/*    rc = trio_sprintf(buffer, "%.30Lf", ldnum); */
/*    Dump(buffer, rc); */

/*    ldnum = 1.0L; */
/*    ldnum /= 3.0L; */
/*    rc = trio_sprintf(buffer, "%.30Lf", ldnum); */
/*    Dump(buffer, rc); */

/*    { */
/*      float a = 5.9, b = 10.0, c = a * b; */
/*      double d = a * b; */
/*      trio_printf("%.15a %.15a\n", c, d); */
/*      trio_printf("%.15g %.15g\n", c, (float)(a * b)); */
/*      trio_printf("%.15a %.15a\n", c, (float)(a * b)); */
/*      trio_printf("%.30.2f %.30.2f\n", c, (float)(a * b)); */
/*    } */
  
/*    trio_locale_set_decimal_point(","); */
/*    trio_locale_set_thousand_separator("'"); */
/*    trio_locale_set_grouping("\1\3\2\177"); */
/*    trio_printf("%'d\n", 123456789); */
/*    trio_printf("%'f\n", 123456789.0); */
/*    trio_locale_set_grouping("\3\2\0"); */
/*    trio_printf("%'d\n", 123456789); */
/*    trio_printf("%'f\n", 123456789.0); */
  
/*    trio_printf("%.30Rf\n", 39413.80); */

  printf("% 10.5f\n", 5163723399.00000);
  trio_printf("% 10.5f\n", 5163723399.00000);
  
  printf("% 020.5f\n", 5163723399.00000);
  trio_printf("% 020.5f\n", 5163723399.00000);
  
  return 0;
}
