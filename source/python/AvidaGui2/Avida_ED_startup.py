def NonInteractive():
  import sys
  import qt
  a = qt.QApplication(sys.argv)
  from AvidaGui2 import Avida_ED_startup_utils as Avida_ED_startup_utils
  Avida_ED_startup_utils.AvidaCore.cConfig.InitGroupList()
  ctrl = Avida_ED_startup_utils.AvidaEd()
  a.exec_loop()

def Interactive():
  import IPython
  import __builtin__
  ipython_shell = IPython.Shell.IPShell(argv=["-c", "import iqt; import AvidaGui2; from AvidaGui2.Avida_ED_startup_utils import *; AvidaCore.cConfig.InitGroupList(); avida_ed = AvidaEd(); edu_workspace_ctrl = Edu_WorkSpace_Ctrl(avida_ed);"])
  ipython_shell.mainloop()

