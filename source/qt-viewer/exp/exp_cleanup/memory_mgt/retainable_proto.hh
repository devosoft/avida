#ifndef RETAINABLE_PROTO_HH
#define RETAINABLE_PROTO_HH

/*
Inherit from cRetainable to provide primitive garbage collection via
retain counting a la Objective C.  The important part of the exposed
interface:

  class cRetainable {
  public:
    void retain();
    void release();
    int refs() const;
  };

where:

  void retain()
    Increments its retain count.
  void release()
    Decrements its retain count.
    If the retain count drops to zero, deletes itself.
  int refs()
    Returns its retain count (for debugging).
  void nameWatched(const cString &name)
    For debugging purposes, gives it a name.
  const cString &watchedName()
    Retrieves its name, for debugging purposes.

The following discussion is adapted from Don Yackman's article "Hold
Me, Use Me, Free Me" on StepWise.com:

  http://www.stepwise.com/Articles/Technical/HoldMe.html

(Note:  use QGuardedPtr<> to store any object inheriting from both
cRetainable and QObject.  See Rule 7 for an explanation.)

Rule 1:  The retention count rule:

- Within a given block the use of new and retain() should equal the use
  of release().
  - Example:
    void Foo::hello(){
      cSomethingRetainable *bar = new cSomethingRetainable;
      // do stuff.
      bar->release();
      // bar deletes itself if nobody else has retained bar.
    }

Rule 2:  Always use accessor methods:

- Sometimes it might seem tedious or pedantic, but if you use accessor
  methods consistently the chances of problems with memory management
  will decrease considerably. 
- If you are using retain() and release() on a class' instance variables
  throughout your code, you're almost certainly doing the wrong thing.
- Example:

  class MyRetainableClass : public cRetainable {
    ...
  };

  class I_use_a_retainable_class {
    MyRetainableClass *m_retainable;
  public:
    // To get and set m_retainable we should use two accessor methods:
    MyRetainableClass *getRetainableThing(){
      return m_retainable;
      // no retain or release since we're just passing back a variable.
    }
    void setRetainableThing(MyRetainableClass *retainable){
      if(retainable) retainable->retain();
      if(m_retainable) m_retainable->release();
      m_retainable = retainable;
    }

    // In the class constructors, initialize m_retainable to 'null'.
    I_use_a_retainable_class():m_retainable(0){
      // do stuff.
    }
    // Since we have a retainable instance variable, we must deallocate
    // it in the class destructors.
    ~I_use_a_retainable_class(){
      setRetainableThing(0);
      // Whatever m_retainable contained earlier has had its retain
      // count decremented.  If nobody else has retained it, it deletes
      // itself.
    }

    // Suppose we want to implement a method to reset the retainable
    // instance variable.  According to the retention count rule, we
    // must balance the 'new' with a release.
    void reset(){
      MyRetainableClass *retainable = new MyRetainableClass();
      // 'retainable' now has a retain count of 1.
      setRetainableThing(retainable);
      // 'retainable' now has a retain count of 2;
      // whatever m_retainable contained earlier has had its retain
      // count decremented.  If nobody else has retained it, it deletes
      // itself.
      retainable->release().
      // 'retainable' now has a retain count of 1.
    }

    // Common mistakes:
      // The following will almost certainly work for some simple cases,
      // but tempting as it may be to eschew accessor methods, this will
      // almost certainly lead to a mistake at some stage (if for
      // example you forget to release the retainable thing, or try to
      // reference a null pointer).
      void reset(){
        MyRetainableClass *retainable = new MyRetainableClass();
        if(m_retainable) m_retainable->release();
        m_retainable = retainable;
      }
      // The following results in a memory leak; the retain count of the
      // new retainable thing is 1 from 'new' and 2 from
      // setRetainableThing(), and we have not balanced the 'new' with a
      // release within the scope of the block.
      void reset(){
        MyRetainableClass *retainable = new MyRetainableClass();
        setRetainableThing(retainable);
      }
  };


Rules 1 & 2 should be sufficient.  But read further discussion if you
wish.


If you follow the retention and accessor rules then:

Rule 3: If a block of code did not allocate or retain an object, then
that block of code should not release it.

Rule 4: When you obtain an object as a result of a method call it should
remain valid until the end of your code block, and your code block can
safely return the object.  If you need the object to live longer than
this -- for example if you plan to store it in an instance variable --
then you need to retain() it.

Rule 5: Factory methods are an exception.  A factory method should
behave like 'new' in that the factory method returns an object with
retain count of 1, and relinquishes ownership of the object to the
caller.


There is a problem that the preceding rules do *not* handle: the problem
of "retain cycles".  A retain cycle is a special kind of memory leak
that can occur with a retain counting scheme when object A retains
object B, and object B retains object A.  These two objects never reach
a zero retain count, but when they are no longer used by any part of the
program, they hit a count of one, so never delete themselves.  It is
possible to have very complex retain cycles where the minimum retain
count is higher than one (multiple objects referring to each other), or
a long chain of objects retaining each other in what looks like a
circular linked list.

The best solution to the problem is to be careful with your designs;
decide who owns who, and retain accordingly.  For example, in a
child-parent relationship in which the child knows about the parent and
the parent knows about the child, you can have the parent retain() the
child but not the other way around.  This would seem to break rule 4
above since the child wants a guarantee that the parent will stick
around, but with careful design the retain() is not needed to guarantee
this condition and would, in effect, be redundant.  The one thing that
should be done for safety is that the parent should instruct the child
to set its parent pointer to 'null' when the parent releases the child.
As long as the parent is retaining the child, the childs pointer to the
parent will be valid.  When the child is released, the pointer will be
taken away.  No problem.

Unfortunately there isn't a much better solution to this problem than
starting with a careful design.  Be sure to expend adequate thought
about how your program's object will interact.


On the other hand (as you might have noticed) say you want to give the
child to another parent.  If you just remove it from its current parent
then the child will be released and destroyed.  You must first obtain
the child, retain() it, remove it from its parent, hand it off to its
new parent, and then release() it.  Which leads to a sixth rule:

Rule 6: Use retain() and release() when you want to prevent an object
from being destroyed as a side effect of the operations you're
performing.


Note: interactions with Qt's QObject class: any object inheriting from
both cRetainable and QObject can be deleted by Qt, even if the object is
retained.  To prevent dangling pointers to deleted QObjects:

Rule 7: Use QGuardedPtr<> to store retainable QObjects.  If you follow
this rule, your accessor getThingy() will return 0 (as it should) when
the thingy has been deleted.  The code looks like this:

  class MyClass {
    // m_thing behaves like a pointer, so *m_thingy, m_thingy->, and
    // m_thingy= all work fine.
    QGuardedPtr<cThingy> m_thingy;
  public:
    void setThingy(cThingy *thingy){ 
      if(thingy) thingy->retain();
      // don't worry, if m_thingy was deleted by Qt, it won't be
      // released below.
      if(m_thingy) m_thingy->release();
      m_thingy = thingy;
    }
    // m_thingy is automatically converted to a pointer below, unless it
    // was deleted by Qt, in which case it's converted to 0.
    cThingy *getThingy(){ return m_thingy; }
  }
*/


class cWatchRetains;
class cRetainable {
  friend class cWatchRetains;
private:
  /* disabled */ cRetainable operator =(const cRetainable &other);
protected:
  static tRList<cWatchRetains> s_retain_watchlist;
  static cString s_noname_str;
  static cString s_copyof_str;
protected:
  cWatchRetains *m_watcher;
  int m_retain_count;
  cString m_ret_name;
protected:
  cRetainable()
  : m_retain_count(0)
  , m_ret_name(s_noname_str)
  { beWatched(); retain(); }
  cRetainable(const cRetainable &other)
  : m_retain_count(0)
  , m_ret_name(s_copyof_str + other.watchedName())
  { beWatched(); retain(); }
  virtual ~cRetainable(){ endWatch(); }
  void endWatch();
  void beWatched();
public:
  const int refs() const { return m_retain_count; }
  void retain(){ ++m_retain_count; }
  void release(){ if(--m_retain_count < 1) delete this; }
  const cString &watchedName() const { return m_ret_name; }
  void nameWatched(const cString &name);
};


#define SETretainable(storage,arg) \
{ \
  if(arg) (arg)->retain(); \
  if(storage) (storage)->release(); \
  (storage) = (arg); \
}


#endif

// arch-tag: proto file for retainable object base class
