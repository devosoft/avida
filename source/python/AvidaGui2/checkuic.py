import os, time, re
py_dict = {}
dir_listing = os.listdir('.')
for file in dir_listing:
  if file.endswith('View.py'):
    py_dict['./' + file] = 0
for file in dir_listing:
  if file.endswith('.ui'):
    if re.compile('junk').search(file,1):
      print "============== " + file + " =============="
    ui_name = './' + file
    ui_stat = os.stat(ui_name)
    py_name = ui_name.replace(".ui",".py")
    if py_dict.has_key(py_name):
      py_dict[py_name] = 1

      # run pyuic and check the existing .py file against the newly
      # created .py file

      # print "******** " + py_name + " *******"
      # os.system("pyuic " + ui_name + " > pyuic.tmp")
      # os.system("diff -w " + py_name + " pyuic.tmp > diff.tmp")
      # diff_file = open("diff.tmp")
      # lines = diff_file.readlines()
      # diff_file.close()
      # for line in lines:
        # showline = True
        # if re.compile("# Created:").search(line, 1):
          # showline = False
        # if re.compile("# Form implementation generated").search(line, 1):
          # showline = False
        # if re.compile("The PyQt User Interface Compiler").search(line, 1):
          # showline = False
        # if showline:
          # print line

      # see if the .ui file is new than the .py file and
      # offer to run pyuic

      py_stat = os.stat(py_name)
      time_diff = ui_stat.st_mtime - py_stat.st_mtime
      if time_diff > 0:
        print ui_name + ' ' + time.ctime(ui_stat.st_mtime)
        print py_name + ' ' + time.ctime(py_stat.st_mtime)
        command =  "./pyuic " + ui_name + " > " + py_name
        response = raw_input(command + "? (y/n)  ")
        response = response.upper()
        response = response.strip()
        if response.startswith('Y'):
          os.system(command)
    else:
      print "No file " + py_name + " exists"
      command =  "./pyuic " + ui_name + " > " + py_name
      response = raw_input(command + "? (y/n)  ")
      response = response.upper()
      response = response.strip()
      if response.startswith('Y'):
        os.system(command)

for py_name in py_dict.keys():
  if py_dict[py_name] == 0:
    ui_name = py_name.replace(".py", ".ui")
    print ui_name + " not found"
