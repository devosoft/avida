////////////////////////////////////////////////////////////////////////////////
// The Loki Library
// Copyright (c) 2001 by Andrei Alexandrescu
// This code accompanies the book:
// Alexandrescu, Andrei. "Modern C++ Design: Generic Programming and Design 
//     Patterns Applied". Copyright (c) 2001. Addison-Wesley.
// Permission to use, copy, modify, distribute and sell this software for any 
//     purpose is hereby granted without fee, provided that the above copyright 
//     notice appear in all copies and that both that copyright notice and this 
//     permission notice appear in supporting documentation.
// The author or Addison-Welsey Longman make no representations about the 
//     suitability of this software for any purpose. It is provided "as is" 
//     without express or implied warranty.
////////////////////////////////////////////////////////////////////////////////

// Last update: August 22, 2001

#ifndef TYPELIST_INC_
#define TYPELIST_INC_

#include "NullType.h"
#include "TypeManip.h"

////////////////////////////////////////////////////////////////////////////////
// macros TYPELIST_1, TYPELIST_2, ... TYPELIST_50
// Each takes a number of arguments equal to its numeric suffix
// The arguments are type names. TYPELIST_NN generates a typelist containing 
//     all types passed as arguments, in that order.
// Example: TYPELIST_2(char, int) generates a type containing char and int.
////////////////////////////////////////////////////////////////////////////////

#define TYPELIST_1(T1) ::Loki::Typelist<T1, ::Loki::NullType>

#define TYPELIST_2(T1, T2) ::Loki::Typelist<T1, TYPELIST_1(T2) >

#define TYPELIST_3(T1, T2, T3) ::Loki::Typelist<T1, TYPELIST_2(T2, T3) >

#define TYPELIST_4(T1, T2, T3, T4) \
    ::Loki::Typelist<T1, TYPELIST_3(T2, T3, T4) >

#define TYPELIST_5(T1, T2, T3, T4, T5) \
    ::Loki::Typelist<T1, TYPELIST_4(T2, T3, T4, T5) >

#define TYPELIST_6(T1, T2, T3, T4, T5, T6) \
    ::Loki::Typelist<T1, TYPELIST_5(T2, T3, T4, T5, T6) >

#define TYPELIST_7(T1, T2, T3, T4, T5, T6, T7) \
    ::Loki::Typelist<T1, TYPELIST_6(T2, T3, T4, T5, T6, T7) >

#define TYPELIST_8(T1, T2, T3, T4, T5, T6, T7, T8) \
    ::Loki::Typelist<T1, TYPELIST_7(T2, T3, T4, T5, T6, T7, T8) >

#define TYPELIST_9(T1, T2, T3, T4, T5, T6, T7, T8, T9) \
    ::Loki::Typelist<T1, TYPELIST_8(T2, T3, T4, T5, T6, T7, T8, T9) >

#define TYPELIST_10(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10) \
    ::Loki::Typelist<T1, TYPELIST_9(T2, T3, T4, T5, T6, T7, T8, T9, T10) >

#define TYPELIST_11(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11) \
    ::Loki::Typelist<T1, TYPELIST_10(T2, T3, T4, T5, T6, T7, T8, T9, T10, T11) >

#define TYPELIST_12(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12) \
    ::Loki::Typelist<T1, TYPELIST_11(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12) >

#define TYPELIST_13(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13) \
    ::Loki::Typelist<T1, TYPELIST_12(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13) >

#define TYPELIST_14(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14) \
    ::Loki::Typelist<T1, TYPELIST_13(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14) >

#define TYPELIST_15(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15) \
    ::Loki::Typelist<T1, TYPELIST_14(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15) >

#define TYPELIST_16(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16) \
    ::Loki::Typelist<T1, TYPELIST_15(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16) >

#define TYPELIST_17(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17) \
    ::Loki::Typelist<T1, TYPELIST_16(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17) >

#define TYPELIST_18(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18) \
    ::Loki::Typelist<T1, TYPELIST_17(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18) >

#define TYPELIST_19(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19) \
    ::Loki::Typelist<T1, TYPELIST_18(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19) >

#define TYPELIST_20(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20) \
    ::Loki::Typelist<T1, TYPELIST_19(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20) >

#define TYPELIST_21(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, T21) \
    ::Loki::Typelist<T1, TYPELIST_20(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, T21) >

#define TYPELIST_22(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, T21, T22) \
    ::Loki::Typelist<T1, TYPELIST_21(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, T21, T22) >

#define TYPELIST_23(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, T21, T22, T23) \
    ::Loki::Typelist<T1, TYPELIST_22(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, T21, T22, T23) >

#define TYPELIST_24(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, T21, T22, T23, T24) \
    ::Loki::Typelist<T1, TYPELIST_23(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, T21, T22, T23, T24) >

#define TYPELIST_25(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, T21, T22, T23, T24, T25) \
    ::Loki::Typelist<T1, TYPELIST_24(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25) >

#define TYPELIST_26(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26) \
    ::Loki::Typelist<T1, TYPELIST_25(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26) >

#define TYPELIST_27(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27) \
    ::Loki::Typelist<T1, TYPELIST_26(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27) >

#define TYPELIST_28(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28) \
    ::Loki::Typelist<T1, TYPELIST_27(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28) >

#define TYPELIST_29(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29) \
    ::Loki::Typelist<T1, TYPELIST_28(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29) >

#define TYPELIST_30(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30) \
    ::Loki::Typelist<T1, TYPELIST_29(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30) >

#define TYPELIST_31(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, T31) \
    ::Loki::Typelist<T1, TYPELIST_30(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, T31) >

#define TYPELIST_32(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, T31, T32) \
    ::Loki::Typelist<T1, TYPELIST_31(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, T31, T32) >

#define TYPELIST_33(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, T31, T32, T33) \
    ::Loki::Typelist<T1, TYPELIST_32(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, T31, T32, T33) >

#define TYPELIST_34(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, T31, T32, T33, T34) \
    ::Loki::Typelist<T1, TYPELIST_33(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, T31, T32, T33, T34) >

#define TYPELIST_35(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
        T31, T32, T33, T34, T35) \
    ::Loki::Typelist<T1, TYPELIST_34(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
        T31, T32, T33, T34, T35) >

#define TYPELIST_36(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
        T31, T32, T33, T34, T35, T36) \
    ::Loki::Typelist<T1, TYPELIST_35(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
        T31, T32, T33, T34, T35, T36) >

#define TYPELIST_37(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
        T31, T32, T33, T34, T35, T36, T37) \
    ::Loki::Typelist<T1, TYPELIST_36(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
        T31, T32, T33, T34, T35, T36, T37) >

#define TYPELIST_38(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
        T31, T32, T33, T34, T35, T36, T37, T38) \
    ::Loki::Typelist<T1, TYPELIST_37(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
        T31, T32, T33, T34, T35, T36, T37, T38) >

#define TYPELIST_39(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
        T31, T32, T33, T34, T35, T36, T37, T38, T39) \
    ::Loki::Typelist<T1, TYPELIST_38(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
        T31, T32, T33, T34, T35, T36, T37, T38, T39) >

#define TYPELIST_40(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
        T31, T32, T33, T34, T35, T36, T37, T38, T39, T40) \
    ::Loki::Typelist<T1, TYPELIST_39(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
        T31, T32, T33, T34, T35, T36, T37, T38, T39, T40) >

#define TYPELIST_41(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
        T31, T32, T33, T34, T35, T36, T37, T38, T39, T40, T41) \
    ::Loki::Typelist<T1, TYPELIST_40(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
        T31, T32, T33, T34, T35, T36, T37, T38, T39, T40, T41) >

#define TYPELIST_42(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
        T31, T32, T33, T34, T35, T36, T37, T38, T39, T40, T41, T42) \
    ::Loki::Typelist<T1, TYPELIST_41(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
        T31, T32, T33, T34, T35, T36, T37, T38, T39, T40, T41, T42) >

#define TYPELIST_43(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
        T31, T32, T33, T34, T35, T36, T37, T38, T39, T40, T41, T42, T43) \
    ::Loki::Typelist<T1, TYPELIST_42(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
        T31, T32, T33, T34, T35, T36, T37, T38, T39, T40, T41, T42, T43) >

#define TYPELIST_44(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
        T31, T32, T33, T34, T35, T36, T37, T38, T39, T40, T41, T42, T43, T44) \
    ::Loki::Typelist<T1, TYPELIST_43(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
        T31, T32, T33, T34, T35, T36, T37, T38, T39, T40, T41, T42, T43, T44) >

#define TYPELIST_45(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
        T31, T32, T33, T34, T35, T36, T37, T38, T39, T40, \
        T41, T42, T43, T44, T45) \
    ::Loki::Typelist<T1, TYPELIST_44(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
        T31, T32, T33, T34, T35, T36, T37, T38, T39, T40, \
        T41, T42, T43, T44, T45) >

#define TYPELIST_46(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
        T31, T32, T33, T34, T35, T36, T37, T38, T39, T40, \
        T41, T42, T43, T44, T45, T46) \
    ::Loki::Typelist<T1, TYPELIST_45(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
        T31, T32, T33, T34, T35, T36, T37, T38, T39, T40, \
        T41, T42, T43, T44, T45) >

#define TYPELIST_47(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
        T31, T32, T33, T34, T35, T36, T37, T38, T39, T40, \
        T41, T42, T43, T44, T45, T46, T47) \
    ::Loki::Typelist<T1, TYPELIST_46(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
        T31, T32, T33, T34, T35, T36, T37, T38, T39, T40, \
        T41, T42, T43, T44, T45, T46, T47) >

#define TYPELIST_48(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
        T31, T32, T33, T34, T35, T36, T37, T38, T39, T40, \
        T41, T42, T43, T44, T45, T46, T47, T48) \
    ::Loki::Typelist<T1, TYPELIST_47(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
        T31, T32, T33, T34, T35, T36, T37, T38, T39, T40, \
        T41, T42, T43, T44, T45, T46, T47, T48) >

#define TYPELIST_49(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
        T31, T32, T33, T34, T35, T36, T37, T38, T39, T40, \
        T41, T42, T43, T44, T45, T46, T47, T48, T49) \
    ::Loki::Typelist<T1, TYPELIST_48(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
        T31, T32, T33, T34, T35, T36, T37, T38, T39, T40, \
        T41, T42, T43, T44, T45, T46, T47, T48, T49) >

#define TYPELIST_50(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
        T31, T32, T33, T34, T35, T36, T37, T38, T39, T40, \
        T41, T42, T43, T44, T45, T46, T47, T48, T49, T50) \
    ::Loki::Typelist<T1, TYPELIST_49(T2, T3, T4, T5, T6, T7, T8, T9, T10, \
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, \
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30, \
        T31, T32, T33, T34, T35, T36, T37, T38, T39, T40, \
        T41, T42, T43, T44, T45, T46, T47, T48, T49, T50) >

namespace Loki
{
////////////////////////////////////////////////////////////////////////////////
// class template Typelist
// The building block of typelists of any length
// Use it through the TYPELIST_NN macros
// Defines nested types:
//     Head (first element, a non-typelist type by convention)
//     Tail (second element, can be another typelist)
////////////////////////////////////////////////////////////////////////////////

    template <class T, class U>
    struct Typelist
    {
       typedef T Head;
       typedef U Tail;
    };

    namespace TL
    {
////////////////////////////////////////////////////////////////////////////////
// class template Length
// Computes the length of a typelist
// Invocation (TList is a typelist):
// Length<TList>::value
// returns a compile-time constant containing the length of TList, not counting
//     the end terminator (which by convention is NullType)
////////////////////////////////////////////////////////////////////////////////

        template <class TList> struct Length;
        template <> struct Length<NullType>
        {
            enum { value = 0 };
        };
        
        template <class TList>
        struct Length
        {
           enum { value = 1 + Length<typename TList::Tail>::value };
        };

////////////////////////////////////////////////////////////////////////////////
// class template TypeAt
// Finds the type at a given index in a typelist
// Invocation (TList is a typelist and index is a compile-time integral 
//     constant):
// TypeAt<TList, index>::Result
// returns the type in position 'index' in TList
// If you pass an out-of-bounds index, the result is a compile-time error
////////////////////////////////////////////////////////////////////////////////

   namespace TypeAt_ {

      template<unsigned int i>
      struct Index {
         template<class TList>
         struct Which {
            typedef typename Index<i - 1>::Which<typename TList::Tail>::Result Result;
         };
      };

      template<>
      struct Index<0> {
         template<class TList>
         struct Which {
            typedef typename TList::Head Result;
         };
      };

   }  // namespace TypeAt_

   template<class TList, unsigned int i>
   struct TypeAt {
      typedef typename TypeAt_::Index<i>::Which<TList>::Result Result;
   };

////////////////////////////////////////////////////////////////////////////////
// class template TypeAtNonStrict
// Finds the type at a given index in a typelist
// Invocations (TList is a typelist and index is a compile-time integral 
//     constant):
// a) TypeAt<TList, index>::Result
// returns the type in position 'index' in TList, or NullType if index is 
//     out-of-bounds
// b) TypeAt<TList, index, D>::Result
// returns the type in position 'index' in TList, or D if index is out-of-bounds
////////////////////////////////////////////////////////////////////////////////

   namespace TypeAtNonStrict_ {

      template<class TList>
      struct ListType;

      template<unsigned int index>
      struct Index;

      template<>
      struct ListType<NullType> {
         template<unsigned int index, typename DefaultType>
         struct Remainder {
            typedef DefaultType Result;
         };
      };

      template<>
      struct Index<0> {
         template<class TList, typename DefaultType>
         struct Remainder {
            typedef typename TList::Head Result;
         };
      };

      template<unsigned int index>
      struct Index {
         template<class TList, typename DefaultType>
         struct Remainder {
            enum { isSameType = Conversion<NullType, typename TList::Tail>::sameType };
            typedef typename Select<isSameType, NullType, typename Index<index - 1>::Remainder<typename TList::Tail, DefaultType>::Result>::Result Result;
         };
      };

      template<class TList>
      struct ListType {
         template<unsigned int index, typename DefaultType>
         struct Remainder {
            typedef typename Index<index>::Remainder<TList, DefaultType>::Result Result;
         };
      };
 
   }  // namespace TypeAtNonStrict_

   template<class TList, unsigned int index, typename DefaultType = NullType>
   struct TypeAtNonStrict {
      typedef typename TypeAtNonStrict_::ListType<TList>::Remainder<index, DefaultType>::Result Result;
   };

}  // namespace TL

////////////////////////////////////////////////////////////////////////////////
// class template IndexOf
// Finds the index of a type in a typelist
// Invocation (TList is a typelist and T is a type):
// IndexOf<TList, T>::value
// returns the position of T in TList, or NullType if T is not found in TList
////////////////////////////////////////////////////////////////////////////////

namespace Private {

namespace IndexOf_ {

template<typename TList>
struct Head {
   typedef typename TList::Head Result;
};

template<>
struct Head<NullType> {
   typedef void Result;
};

struct IsNull {
   enum { value = -1 };
};

struct IsTHead {
   enum { value = 0 };
};

template<typename TList, typename T>
struct IsNotTHead {
   typedef typename TList::Tail Tail;
   typedef typename Select<is_same<Tail, NullType>::result, 
                           IsNull, 
                           typename Select<is_same<typename Head<Tail>::Result, T>::result, 
                                           IsTHead, IsNotTHead<Tail, T>
                                          >::Result
                          >::Result chooser;
   enum { temp = chooser::value };
   enum { value = temp == -1 ? -1 : 1 + temp };
};

template<typename TList, typename T>
struct IsNotNull {
   typedef typename Select<is_same<typename TList::Head, T>::result, 
                           IsTHead, 
                           IsNotTHead<TList, T>
                           >::Result chooser;
   enum { value = chooser::value };
};

}  // namespace IndexOf_

}  // namespace Private

namespace TL {

template<class TList, class T>
struct IndexOf {
private:
   typedef typename Select<Private::is_same<TList, NullType>::result, 
                           Private::IndexOf_::IsNull, 
                           Private::IndexOf_::IsNotNull<TList, T>
                          >::Result chooser;
public:
   enum { value = chooser::value };
};
                       
////////////////////////////////////////////////////////////////////////////////
// class template Append
// Appends a type or a typelist to another
// Invocation (TList is a typelist and T is either a type or a typelist):
// Append<TList, T>::Result
// returns a typelist that is TList followed by T and NullType-terminated
////////////////////////////////////////////////////////////////////////////////

   namespace Append_ {

   template<class T>
   struct Helper {
      struct big { char i[2]; };
      template<class Head, class Tail>
      static char Test(const Typelist<Head, Tail>&);
      static big Test(...);
      static T makeT();
   };

   }  // namespace Append_

   template<class TList, class T> 
   struct Append;
        
   template<> 
   struct Append<NullType, NullType> {
      typedef NullType Result;
   };

   template<class TList, class T>
   struct Append {
   private:
      enum { T_is_list = sizeof(Append_::Helper<T>::Test(Append_::Helper<T>::MakeT())) == sizeof(char) };
      enum { TList_is_null = Conversion<TList, NullType>::sameType };
      typedef typename Select<TList_is_null & T_is_list, T, NullType>::Result Result1;
      typedef typename Select<TList_is_null & !T_is_list, Typelist<T, NullType>, Result1>::Result Result2;
   public:
      typedef typename Select<!TList_is_null & !T_is_list, Typelist<typename TList::Head, typename Append<typename TList::Tail, T>::Result>, Result2>::Result Result;
   };
        
////////////////////////////////////////////////////////////////////////////////
// class template Erase
// Erases the first occurence, if any, of a type in a typelist
// Invocation (TList is a typelist and T is a type):
// Erase<TList, T>::Result
// returns a typelist that is TList without the first occurence of T
////////////////////////////////////////////////////////////////////////////////

      template<class TList, class T>
      struct Erase {
      private:
         enum { TList_is_null = Conversion<TList, NullType>::sameType };
         enum { Head_is_T = Conversion<typename TList::Head, T>::sameType };
         typedef typename Select<!TList_is_null & Head_is_T, typename TList::Tail, NullType>::Result Result1;
      public:
         typedef typename Select<!TList_is_null & !Head_is_T, Typelist<typename TList::Head, typename Erase<typename TList::Tail, T>::Result>, Result1>::Result Result;
      };

////////////////////////////////////////////////////////////////////////////////
// class template EraseAll
// Erases all first occurences, if any, of a type in a typelist
// Invocation (TList is a typelist and T is a type):
// EraseAll<TList, T>::Result
// returns a typelist that is TList without any occurence of T
////////////////////////////////////////////////////////////////////////////////

      template<class TList, class T>
      struct EraseAll {
      private:
         enum { TList_is_null = Conversion<TList, NullType>::sameType };
         enum { Head_is_T = Conversion<typename TList::Head, T>::sameType };
         typedef typename Select<!TList_is_null & Head_is_T, typename EraseAll<typename TList::Tail, T>::Result, NullType>::Result Result1;
      public:
         typedef typename Select<!TList_is_null & !Head_is_T, Typelist<typename TList::Head, typename EraseAll<typename TList::Tail, T>::Result>, Result1>::Result Result;
      };

////////////////////////////////////////////////////////////////////////////////
// class template NoDuplicates
// Removes all duplicate types in a typelist
// Invocation (TList is a typelist):
// NoDuplicates<TList, T>::Result
////////////////////////////////////////////////////////////////////////////////

   template<class TList>
   struct NoDuplicates;

   template<>
   struct NoDuplicates<NullType> {
      typedef NullType Result;
   };

   template<class TList>
   struct NoDuplicates {
   private:
      typedef typename NoDuplicates<typename TList::Tail>::Result L1;
      typedef typename Erase<L1, typename TList::Head>::Result L2;
   public:
      typedef Typelist<typename TList::Head, L2> Result;
   };


////////////////////////////////////////////////////////////////////////////////
// class template Replace
// Replaces the first occurence of a type in a typelist, with another type
// Invocation (TList is a typelist, T, U are types):
// Replace<TList, T, U>::Result
// returns a typelist in which the first occurence of T is replaced with U
////////////////////////////////////////////////////////////////////////////////

      template<class TList, class T, class U> 
      struct Replace {
      private:
         enum { TList_is_null = Conversion<TList, NullType>::sameType };
         enum { Head_is_T = Conversion<typename TList::Head, T>::sameType };
         typedef typename Select<!TList_is_null & Head_is_T, Typelist<U, typename TList::Tail>, NullType>::Result Result1;
      public:
         typedef typename Select<!TList_is_null & !Head_is_T, Typelist<typename TList::Head, typename Replace<typename TList::Tail, T, U>::Result>, Result1>::Result Result;
      };
        
////////////////////////////////////////////////////////////////////////////////
// class template ReplaceAll
// Replaces all occurences of a type in a typelist, with another type
// Invocation (TList is a typelist, T, U are types):
// Replace<TList, T, U>::Result
// returns a typelist in which all occurences of T is replaced with U
////////////////////////////////////////////////////////////////////////////////

   template<class TList, class T, class U> 
   struct ReplaceAll {
      private:
         enum { TList_is_null = Conversion<TList, NullType>::sameType };
         enum { Head_is_T = Conversion<typename TList::Head, T>::sameType };
         typedef typename Select<!TList_is_null & Head_is_T, Typelist<U, typename ReplaceAll<typename TList::Tail, T, U>::Result>, NullType>::Result Result1;
      public:
         typedef typename Select<!TList_is_null & !Head_is_T, Typelist<typename TList::Head, typename ReplaceAll<typename TList::Tail, T, U>::Result>, Result1>::Result Result;
   };
              
////////////////////////////////////////////////////////////////////////////////
// class template Reverse
// Reverses a typelist
// Invocation (TList is a typelist):
// Reverse<TList>::Result
// returns a typelist that is TList reversed
////////////////////////////////////////////////////////////////////////////////

      template<class TList>
      struct Reverse {
      private:
         enum { list_of_one = Conversion<typename TList::Tail, NullType>::sameType };
      public:
         typedef typename Select<list_of_one, 
                                 TList, 
                                 typename Append<typename Reverse<typename TList::Tail>::Result, 
                                                 typename TList::Head
                                                >::Result
                                >::Result Result;
      };

////////////////////////////////////////////////////////////////////////////////
// class template MostDerived
// Finds the type in a typelist that is the most derived from a given type
// Invocation (TList is a typelist, T is a type):
// Replace<TList, T>::Result
// returns the type in TList that's the most derived from T
////////////////////////////////////////////////////////////////////////////////

      template<class TList, class T>
      struct MostDerived {
      private:
         enum { TList_is_null = Conversion<TList, NullType>::sameType };
         typedef typename Select<TList_is_null, T, typename MostDerived<typename TList::Tail, T>::Result>::Result Candidate;
      public:
         typedef typename Select<TList_is_null, T, 
                                 typename Select<SUPERSUBCLASS(Candidate, typename TList::Head),
                                                 typename TList::Head, Candidate
                                                >::Result
                                >::Result Result;
      };
        
////////////////////////////////////////////////////////////////////////////////
// class template DerivedToFront
// Arranges the types in a typelist so that the most derived types appear first
// Invocation (TList is a typelist):
// DerivedToFront<TList>::Result
// returns the reordered TList 
////////////////////////////////////////////////////////////////////////////////

   namespace DerivedToFront_ {

      template<class TList>
      struct ListType {
      };

      template<>
      struct ListType<NullType> {
         typedef NullType Result;
      };

   }  // namespace DerivedToFront_

   template<class TList> 
   struct DerivedToFront;
        
   template<>
   struct DerivedToFront<NullType> {
      typedef NullType Result;
   };
        
   template<class TList>
   struct DerivedToFront {
   private:
      typedef typename MostDerived<typename TList::Tail, 
                                 typename TList::Head>::Result TheMostDerived;
      typedef typename Replace<typename TList::Tail, TheMostDerived, 
                               typename TList::Head>::Result L;
   public:
      typedef Typelist<TheMostDerived, L> Result;
   };
        
   }   // namespace TL

}   // namespace Loki

////////////////////////////////////////////////////////////////////////////////
// Change log:
// June 09, 2001: Fix bug in parameter list of macros TYPELIST_23 to TYPELIST_27
//      (credit due to Dave Taylor)
// June 20, 2001: ported by Nick Thurn to gcc 2.95.3. Kudos, Nick!!!
// August 22, 2001: ported by Jonathan H Lundquist to MSVC
////////////////////////////////////////////////////////////////////////////////

#endif // TYPELIST_INC_

