
print("""
# Importing Avida proper (the stuff written in c++)...
""")
import AvidaCore
# Initialize Avida proper (does nothing if Avida has already been
# initialized)...
AvidaCore.cConfig.InitGroupList()

# Show python where to find python modules in the Avida source tree.
import site
site.addsitedir('/Users/kaben/Projects/Software/Avida/branch.kaben/current--HEAD--cvs/source/bindings/Boost.Python')
del(site)

print("""
# Importing PyQt...
""")
import qt

print("""
# Importing PyQwt...
""")
import qwt

print("""
# Importing iQt...
""")
import iqt

print("""
# Importing Avida gui modules...
""")
# These are imported by name so that they may be reloaded each time this
# script is run.
import AvidaGui.py_avida_core_data
import AvidaGui.py_avida_driver_controller
import AvidaGui.py_avida_state_mediator
import AvidaGui.py_avida_threaded_driver
import AvidaGui.py_main_controller_data
#
import AvidaGui.py_brainstorms
# Reload all of the gui code -- this picks-up changes to the python
# sources.
reload(AvidaGui.py_avida_core_data)
reload(AvidaGui.py_avida_driver_controller)
reload(AvidaGui.py_avida_state_mediator)
reload(AvidaGui.py_avida_threaded_driver)
reload(AvidaGui.py_main_controller_data)
#
reload(AvidaGui.py_brainstorms)

# After the (re-)initialization code above has run, only the following
# commands are needed to (re-)instantiate avida and the gui.
print("""
# Instantiating Avida...
""")
main_controller_data = AvidaGui.py_main_controller_data.pyMainControllerData()

print("""
# Instantiating experimental gui...
""")
avida_driver_controller = AvidaGui.py_avida_driver_controller.pyAvidaDriverController(main_controller_data)

avida_dumb_gui = AvidaGui.py_brainstorms.pyAvidaDumbGuiController(main_controller_data)

import time
print("""
# Try the following if you'd like to see whether Avida runs :

bs.main_controller_data.avida_threaded_driver.doSetContinuous(False)
for i in range(30):
  bs.main_controller_data.avida_threaded_driver.doUpdate()
  bs.time.sleep(0.25)

# To see whether the gui's threaded-driver controller works :

bs.avida_driver_controller.doStartAvidaSlot()
# twiddle your thumbs for a little while...
bs.avida_driver_controller.doPauseAvidaSlot()

""")
