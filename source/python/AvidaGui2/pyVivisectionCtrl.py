# -*- coding: utf-8 -*-

from qt import *
from pyVivisectionView import pyVivisectionView


class pyVivisectionCtrl(pyVivisectionView):

  def __init__(self,parent = None,name = None,fl = 0):
    pyVivisectionView.__init__(self,parent,name,fl)
