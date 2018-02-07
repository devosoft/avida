//
//  PtrUtilities.h
//  Avida-Core
//
//  Created by Matthew Rupp on 2/6/18.
//

#ifndef PtrUtilities_h
#define PtrUtilities_h


/*
@MRR
Casting unique_ptrs in C++11 doesn't quite exist as a single method.
From: http://www.ficksworkshop.com/blog/post/how-to-static-cast-std-unique-ptr
we get a generic way to make this work.

The key is to release ownership of the pointer and then make a new unique_ptr
with the released ptr.
*/
template<typename D, typename B>
std::unique_ptr<D> static_cast_ptr(std::unique_ptr<B>& base)
{
    return std::unique_ptr<D>(static_cast<D*>(base.release()));
}

template<typename D, typename B>
std::unique_ptr<D> static_cast_ptr(std::unique_ptr<B>&& base)
{
    return std::unique_ptr<D>(static_cast<D*>(base.release()));
}

#endif /* PtrUtilities_h */
