Last update: Aug 29, 2002

Directions:

To use Loki, simply extract the files from the archive, give your compiler access to their path, and include them appropriately in your code via #include.

If you use the small object allocator directly or indirectly (through the Functor class) you must add SmallObj.cpp to your project/makefile.

If you use Singletons with longevity you must add Singleton.cpp to your project/makefile.


Compatibility:

Supported Compilers:
Gcc v2.95.3
Microsoft Visual C++ v7.0
Borland C++ Builder v6.0

Mostly Supported:
CodeWarrior 6.0

TypeList:
Microsoft Visual C++ v6.0


CodeWarrior has a problem with the Conversion template (see TypeManip.h) and, though it compiles it, it doesn't provide correct results. Consequently, the DerivedToFront algorithm in Typelist.h does not function. This affects the static dispatcher in Multimethods.h. As a fix, you must order the types (putting the most derived ones in the front) when providing the typelist argument to StaticDispatcher.



More info:

http://moderncppdesign.com
http://sourceforge.net/projects/loki-lib/
http://sourceforge.net/projects/loki-exp/


