# Modified from IPython sources for use in AvidaEd
"""IPython -- An enhanced Interactive Python

This is just the startup wrapper script, kept deliberately to a minimum.

The shell's mainloop() takes an optional argument, sys_exit (default=0). If
set to 1, it calls sys.exit() at exit time. You can use the following code in
your PYTHONSTARTUP file:

import IPython
IPython.Shell.IPShell().mainloop(sys_exit=1)

[or simply IPython.Shell.IPShell().mainloop(1) ]

and IPython will be your working environment when you start python. The final
sys.exit() call will make python exit transparently when IPython finishes, so
you don't have an extra prompt to get out of.

This is probably useful to developers who manage multiple Python versions and
don't want to have correspondingly multiple IPython versions. Note that in
this mode, there is no way to pass IPython any command-line options, as those
are trapped first by Python itself.
"""

import sys

# When an OS X application is double-clicked, argv[1] is set to
# '-psn_0_xxx', to indicate a process serial number.
# FIXME # this will probably be deprecated in a future Mac OS release,
# so we need to find another way to distinguish between launches by
# double-clicking vs. launches by command-line.
# @kgn
if len(sys.argv) > 1 and sys.argv[1] >= 5 and sys.argv[1][:5] == '-psn_':
  import qt
  import AvidaCore    
                          
  def Run():      
    import AvidaGui2.pyEduMainCtrl
    edu_main_controller = AvidaGui2.pyEduMainCtrl.pyEduMainCtrl()
    edu_main_controller.construct()
    edu_main_controller.m_main_mdl.m_main_mdtr.m_main_controller_factory_mdtr.emit(
    sys.exc_clear()
    sys.exc_traceback = sys.last_traceback = None
      qt.PYSIGNAL("newMainControllerSig"), ("pySessionCtrl",))
    return edu_main_controller
  
  AvidaCore.cConfig.InitGroupList()
  a = qt.QApplication(sys.argv)
  ctrl=Run()
  a.exec_loop()

else:
  import site
  
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
  
  __builtins__.exit = """
  Use Ctrl-D (i.e. EOF) to exit."""
  
  __builtins__.quit = """
  Use Ctrl-D (i.e. EOF) to exit."""
  
  
  import IPython
  import __builtin__
  
  ipython_shell = IPython.Shell.IPShell(argv=['AvidaEd-interactive.py'])
  ipython_shell.mainloop()
