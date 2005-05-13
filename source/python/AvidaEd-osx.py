import sys

# When an OS X application is double-clicked, argv[1] is set to
# '-psn_0_xxx', to indicate a process serial number.
# FIXME # this will probably be deprecated in a future Mac OS release,
# so we need to find another way to distinguish between launches by
# double-clicking vs. launches by command-line.
# @kgn

if len(sys.argv) > 1 and sys.argv[1] >= 5 and sys.argv[1][:5] == '-psn_':

  # NonInteractive Avida-ED

  from AvidaGui2 import Avida_ED_startup
  Avida_ED_startup.NonInteractive()

else:

  # Interactive Avida-ED

  # The bundled version of Avida-ED has a stripped-down version of
  # Python, so here I'm adding back some of the missing stuff for the
  # convenience of developers using the interactive version of Avida-ED.

  import site

  class _Helper(object):
      """Define the built-in 'help'.
      This is a wrapper around pydoc.help (with a twist).
  
      """
  
      def __repr__(self):
          return "Type help() for interactive help, " \
                 "or help(object) for help about object."
      def __call__(self, *args, **kwds):
          import pydoc
          return pydoc.help(*args, **kwds)

  class _Printer(object):
      """interactive prompt objects for printing the license text, a list of
      contributors and the copyright notice."""
  
      MAXLINES = 23
  
      def __init__(self, name, data, files=(), dirs=()):
          self.__name = name
          self.__data = data
          self.__files = files
          self.__dirs = dirs
          self.__lines = None
  
      def __setup(self):
          if self.__lines:
              return
          data = None
          for dir in self.__dirs:
              for filename in self.__files:
                  filename = os.path.join(dir, filename)
                  try:
                      fp = file(filename, "rU")
                      data = fp.read()
                      fp.close()
                      break
                  except IOError:
                      pass
              if data:
                  break
          if not data:
              data = self.__data
          self.__lines = data.split('\n')
          self.__linecnt = len(self.__lines)
  
      def __repr__(self):
          self.__setup()
          if len(self.__lines) <= self.MAXLINES:
              return "\n".join(self.__lines)
          else:
              return "Type %s() to see the full %s text" % ((self.__name,)*2)
  
      def __call__(self):
          self.__setup()
          prompt = 'Hit Return for more, or q (and Return) to quit: '
          lineno = 0
          while 1:
              try:
                  for i in range(lineno, lineno + self.MAXLINES):
                      print self.__lines[i]
              except IndexError:
                  break
              else:
                  lineno += self.MAXLINES
                  key = None
                  while key is None:
                      key = raw_input(prompt)
                      if key not in ('', 'q'):
                          key = None
                  if key == 'q':
                      break

  __builtins__.credits = _Printer("credits", """
  Thanks to CWI, CNRI, BeOpen.com, Zope Corporation and a cast of thousands
  for supporting Python development.  See www.python.org for more information.""")
  
  __builtins__.copyright = _Printer("credits", """
  Copyright (c) 2001-2004 Python Software Foundation.
  All Rights Reserved.
  
  Copyright (c) 2000 BeOpen.com.
  All Rights Reserved.
  
  Copyright (c) 1995-2001 Corporation for National Research Initiatives.
  All Rights Reserved.
  
  Copyright (c) 1991-1995 Stichting Mathematisch Centrum, Amsterdam.
  All Rights Reserved.
  
  Copyright (c) 2001-2004 Fernando Perez, Janko Hauser, Nathan Gray.
  All Rights Reserved.""")
  
  __builtins__.exit = """Use Ctrl-D (i.e. EOF) to exit."""
  
  __builtins__.quit = """Use Ctrl-D (i.e. EOF) to exit."""

  __builtins__.help = _Helper()


  #

  from AvidaGui2 import Avida_ED_startup
  Avida_ED_startup.Interactive()

