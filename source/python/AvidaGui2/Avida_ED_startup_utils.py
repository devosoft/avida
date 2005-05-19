
import qt

# Make a splash screen to give feedback to the user about progress in
# launchning Avida-ED.
#
s_pixmap = qt.QPixmap("avidalogo.png")
s_splash = qt.QSplashScreen(s_pixmap)
font = s_splash.font()
font.setPointSize(9)
s_splash.setFont(font)
del font
s_splash.show()
s_splash.message("Loading core Avida modules ...")

import AvidaCore
import sys

# For developers : this asks the pyTemporaryReloads class to reload
# certain parts of the python source code for the Avida gui, picking up
# any changes since the last launch of the gui. This can be called
# without restarting the application (convenient for quickly checking
# the results of changes to the source code).
#
def Reload():
  import AvidaGui2.pyTemporaryReloads
  reload(AvidaGui2.pyTemporaryReloads)
  # Ask the linecache module to refresh its cache with new versions of
  # the source code.
  import linecache
  linecache.checkcache()

# Runs unit tests. Can be called while gui is running, and will
# instantiate a new copy of Avida for testing (but this will disturb
# Avida's random number generator).
#
def UTs():
  Reload()
  AvidaGui2.pyTemporaryReloads.UTs()

# Instantiate (or reinstantiate) the Avida-ED gui. Return controller for
# new gui instance.
#
def AvidaEd():
  sys.exc_clear()
  sys.exc_traceback = sys.last_traceback = None
  s_splash.message("Loading Avida-ED user-interface ...")
  s_splash.show()
  try:
    Reload()
    from AvidaGui2.pyEduMainCtrl import pyEduMainCtrl
    s_splash.message("Assembling Avida-ED user-interface ...")

    edu_main_controller = pyEduMainCtrl()
    edu_main_controller.construct()
    edu_main_controller.m_main_mdl.m_main_mdtr.m_main_controller_factory_mdtr.emit(
      qt.PYSIGNAL("newMainControllerSig"), ("pySessionCtrl",))

    # Get the edu_workspace_ctrl to browse interactively via
    # 'avida_ed.m_edu_workspace_ctrl'
    #
    session_ctrl = edu_main_controller.m_main_controller_factory.m_main_controllers_list[1]
    edu_main_controller.m_edu_workspace_ctrl = session_ctrl.m_session_controller_factory.m_session_controllers_list[2]

  finally:
    s_splash.clear()
    s_splash.hide()
    print """
    
    Type 'avida_ed=AvidaEd()' to reload the AvidaEd user interface.
    
    """
  return edu_main_controller
