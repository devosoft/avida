#ifndef tMemTrack_h
#define tMemTrack_h

/*
To add memory-tracking to a class or struct cA, add a member variable of
type tMemTrack<cA> (this incurs a one-byte overhead):

class cA {
private:
  tMemTrack<A> mt;
public:
  ...
};

This also works with template classes:

template <class T>
class tA {
private:
  tMemTrack<tA<T> > mt;
public:
  ...
};

The number of instances of cA is returned by calling
tMemTrack<cA>::Instances(), and the number of tA<cFoo> by
tMemTrack<tA<cFoo> >::Instances().

To prevent the 1-byte overhead, one could manually increment and
decrement the instance count from within cA's constructors and
destructor:

struct cA {
  cA(){ tMemTrack<cA>::Instances(1); }
  cA(const cA &){ tMemTrack<cA>::Instances(1); }
  ~cA(){ tMemTrack<cA>::Instances(-1); }
};

***

I like the former method, with a one-byte overhead, because it seems
less error-prone. I've been wrapping the member variable in preprocessor
conditionals so that I can easily turn-off memory-tracking (including the
one-byte overhead):

class cA {
private:
#if USE_tMemTrack
  tMemTrack<cA> mt;
#endif
public:
  ...
};

***

Note: I think that if we build dynamic libraries on windows for
Avida-ED, we'll have to turn memory tracking off, because it uses a
static variable. But it will still help in debugging.

@kgn
*/

template <class T> class tMemTrack {
public:
  tMemTrack(){ Instances(1); }
  tMemTrack(const tMemTrack &){ Instances(1); }
  ~tMemTrack(){ Instances(-1); }
public:
  // @DMB - Note: This method is not thread safe.  Instance count may become
  //              inaccurate during threaded execution.
  static int Instances(int count = 0) {
    static int s_instances = 0;
    return s_instances += count;
  }
};

#endif
