#ifndef WATCH_RETAINS_PROTO_HH
#define WATCH_RETAINS_PROTO_HH

/*
Instantiate cWatchRetains "watcher" objects to check for memory leaks of
cRetainable ("watched") objects.  Any cRetainable objects created
between instantiation and deletion of a watcher will be
watched for memory leaks.

Multiple watchers can be instantiated, but only the most recently
created watcher will be used to track new cRetainable objects, until
the watcher is deleted; then the previously created watcher, if one
exists, will be used to track new cRetainable objects.

The watched objects can be named using nameWatched(); the watcher object
can be named using nameWatcher().  When a watcher is deleted, it prints
a list of objects it was watching that haven't been deleted.  The
printout includes the name of the watched objects, its retain count, and
the name of the watcher.

Here's a good way to use watcher objects:

void Foo::bar(){
  cWatchRetains watcher;
  // Do a bunch of stuff that causes cRetainable to be instantiated and
  // hopefully, eventually, deleted.

  // As Foo::bar() finishes, "watcher" will go out of scope and be
  // deleted, listing the undeleted cRetainable objects it was
  // watching.
}
*/


class cActivateWatcher;
class cWatchRetains : public cRetainable {
  friend class cRetainable;
  friend class cActivateWatcher;
private:
  /* disabled */ cWatchRetains(const cWatchRetains &);
  /* disabled */ cWatchRetains operator =(const cWatchRetains &);
protected:
  std::set<cRetainable *> m_watchset;
protected:
  void activate(){ deactivate(); cRetainable::s_retain_watchlist.Push(this); }
  void deactivate(){ cRetainable::s_retain_watchlist.Remove(this); }
public:
  cWatchRetains(){ cRetainable::s_retain_watchlist.Push(this); }
  ~cWatchRetains();
  void nameWatched(const cString &name);
};


#endif
