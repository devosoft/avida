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

#ifndef TYPEMANIP_INC_
#define TYPEMANIP_INC_

namespace Loki {

   namespace Private {

      struct big { char c[2]; };

      struct any {
         template<typename T>
         any(const T&);
      };

   }  // namespace Private

////////////////////////////////////////////////////////////////////////////////
// class template Int2Type
// Converts each integral constant into a unique type
// Invocation: Int2Type<v> where v is a compile-time constant integral
// Defines 'value', an enum that evaluates to v
////////////////////////////////////////////////////////////////////////////////

    template <int v>
    struct Int2Type
    {
        enum { value = v };
    };
    
////////////////////////////////////////////////////////////////////////////////
// class template Type2Type
// Converts each type into a unique, insipid type
// Invocation Type2Type<T> where T is a type
// Defines the type OriginalType which maps back to T
////////////////////////////////////////////////////////////////////////////////

    template <typename T>
    struct Type2Type
    {
        typedef T OriginalType;
    };
    
////////////////////////////////////////////////////////////////////////////////
// class template Select
// Selects one of two types based upon a boolean constant
// Invocation: Select<flag, T, U>::Result
// where:
// flag is a compile-time boolean constant
// T and U are types
// Result evaluates to T if flag is true, and to U otherwise.
////////////////////////////////////////////////////////////////////////////////

namespace Private {

namespace Select_ {

struct ChooseT {
   template<typename T, typename U>
   struct Choose {
      typedef T Result;
   };
};

struct ChooseU {
   template<typename T, typename U>
   struct Choose {
      typedef U Result;
   };
};

template<bool flag>
struct Selector {
   typedef ChooseT Result;
};

template<>
struct Selector<false> {
   typedef ChooseU Result;
};

} // namespace Select_

} // namespace Private

template<bool flag, typename T, typename U>
struct Select {
private:
   typedef typename Private::Select_::Selector<flag>::Result selector;
public:
   typedef typename selector::Choose<T, U>::Result Result;
};

namespace Private {

template<typename T>
struct is_void {
   enum { value = 0 };
};

template<>
struct is_void<void> {
   enum { value = 1 };
};

namespace is_same_ {

template<typename T>
char test_same(T*, T*);

template<typename T>
big test_same(T*, any);

template<typename T, typename U>
struct is_same_imp {
   static T t;
   static U u;
   enum { result = sizeof(test_same(&t, &u)) == sizeof(char) };
};

}  // namespace is_same_

template<typename T, typename U>
struct is_same {
   enum { voidT = is_void<T>::value };
   enum { voidU = is_void<U>::value };
   struct BothVoid {
      enum { result = 1 };
   };
   struct OneVoid {
      enum { result = 0 };
   };
   typedef typename Select<voidT & voidU, 
                           BothVoid,
                           typename Select<voidT | voidU,
                                           OneVoid,
                                           is_same_::is_same_imp<T, U>
                                          >::Result
                          >::Result tester;
   enum { result = tester::result };
};

}  // namespace Private

////////////////////////////////////////////////////////////////////////////////
// class template Conversion
// Figures out the conversion relationships between two types
// Invocations (T and U are types):
// a) Conversion<T, U>::exists
// returns (at compile time) true if there is an implicit conversion from T
// to U (example: Derived to Base)
// b) Conversion<T, U>::exists2Way
// returns (at compile time) true if there are both conversions from T
// to U and from U to T (example: int to char and back)
// c) Conversion<T, U>::sameType
// returns (at compile time) true if T and U represent the same type
//
// Caveat: might not work if T and U are in a private inheritance hierarchy.
////////////////////////////////////////////////////////////////////////////////

namespace Private {

namespace Conversion_ {

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4181)
#pragma warning(disable:4800)
#pragma warning(disable:4244)
#endif
template<typename T, typename U>
struct Determine {
   template<typename X, typename Y>
   struct tester {
      static char test(X, Y);
      static big test(any, any);
   };
   static T t;
   static U u;
   enum { exists = sizeof(tester<T, U>::test(t, t)) == sizeof(char) };
   enum { exists2Way = exists & (sizeof(tester<U, T>::test(u, u)) == sizeof(char)) };
   enum { sameType = exists2Way & is_same<T, U>::result };
};
#ifdef _MSC_VER
#pragma warning(pop)
#endif

}  // namespace Conversion_

}  // namespace Private

template<typename T, typename U>
struct Conversion {
private:
   enum { voidT = Private::is_void<T>::value };
   enum { voidU = Private::is_void<U>::value };

   struct both_void {
      enum { exists = 1, exists2Way = 1, sameType = 1 };
   };

   struct one_void {
      enum { exists = 1, exists2Way = 0, sameType = 0 };
   };

   typedef typename Select<voidT & voidU,
                           both_void,
                           typename Select<voidT | voidU,
                                           one_void,
                                           Private::Conversion_::Determine<T, U>
                                          >::Result
                          >::Result Chooser;
public:
   enum { exists = Chooser::exists };
   enum { exists2Way = Chooser::exists2Way };
   enum { sameType = Chooser::sameType };
};

}  // namespace Loki

////////////////////////////////////////////////////////////////////////////////
// macro SUPERSUBCLASS
// Invocation: SUPERSUBCLASS(B, D) where B and D are types. 
// Returns true if B is a public base of D, or if B and D are aliases of the 
// same type.
//
// Caveat: might not work if T and U are in a private inheritance hierarchy.
////////////////////////////////////////////////////////////////////////////////

#define SUPERSUBCLASS(T, U) \
    (::Loki::Conversion<const U*, const T*>::exists && \
    !::Loki::Conversion<const T*, const void*>::sameType)

////////////////////////////////////////////////////////////////////////////////
// macro SUPERSUBCLASS
// Invocation: SUPERSUBCLASS(B, D) where B and D are types. 
// Returns true if B is a public base of D.
//
// Caveat: might not work if T and U are in a private inheritance hierarchy.
////////////////////////////////////////////////////////////////////////////////

#define SUPERSUBCLASS_STRICT(T, U) \
    (SUPERSUBCLASS(T, U) && \
    !::Loki::Conversion<const T, const U>::sameType)

////////////////////////////////////////////////////////////////////////////////
// Change log:
// June 20, 2001: ported by Nick Thurn to gcc 2.95.3. Kudos, Nick!!!
// August 22, 2001: ported by Jonathan H Lundquist to MSVC
////////////////////////////////////////////////////////////////////////////////

#endif // TYPEMANIP_INC_
