
import AvidaCore
import qt
import iqt

def Reload():
  import AvidaGui2.pyTemporaryReloads
  reload(AvidaGui2.pyTemporaryReloads)
  import linecache
  linecache.checkcache()

def UTs():
  Reload()
  AvidaGui2.pyTemporaryReloads.UTs()

def AvidaEd():
  Reload()
  edu_main_controller = AvidaGui2.pyEduMainCtrl.pyEduMainCtrl()
  edu_main_controller.construct()
  edu_main_controller.m_main_mdl.m_main_mdtr.m_main_controller_factory_mdtr.emit(
    qt.PYSIGNAL("newMainControllerSig"), ("pySessionCtrl",
    AvidaCore.cString("genesis.avida"), ))
  return edu_main_controller

AvidaCore.cConfig.InitGroupList()

import AvidaGui2.pyTemporaryReloads
reload(AvidaGui2.pyTemporaryReloads)

#UTs()
avida_ed=AvidaEd()

print """

Type 'avida_ed=AvidaEd()' to reload the AvidaEd user interface.

"""
