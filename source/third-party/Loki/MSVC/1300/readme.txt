Last update: May 19, 2002

Directions:

To use Loki, simply extract the files from the archive, give your compiler access to their path, and include them appropriately in your code via #include.

If you use the small object allocator directly or indirectly (through the Functor class) you must add SmallObj.cpp to your project/makefile.

If you use Singletons with longevity you must add Singleton.cpp to your project/makefile.

Compatibility:
This is ported version of Loki to VC.NET (VC7 - 9466) it is only targeted for this compiler.
To compile your sources with the original Loki you'll have to place it in a separate directory.

More info:

http://moderncppdesign.com
http://www.geocities.com/rani_sharoni/LokiPort.html
