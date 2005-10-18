
import inspect

DEBUG = True
#DEBUG = False

def descr(*details):
  if DEBUG:
    s = inspect.stack()
    l = len(s) - 1
    frame = s[1]
    mthd = getattr(inspect.getargvalues(frame[0])[3]['self'], frame[3], None)
    doc, cls, fnm = mthd is None and (None, None, None) or (mthd.im_func.__doc__, mthd.im_class.__name__, mthd.im_func.__name__)
    if doc is None: doc = "<no docs.>"
    pfx, txt = details is () and ('.'*l, doc) or (' '*l, str(details))
    print "%s %s.%s: %s" % (pfx, cls, fnm, txt)

