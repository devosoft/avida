
import qt
import pmock
import types


class BuriedAttributeError(AttributeError):
  def __init__(self, msg):
    AttributeError.__init__(self, msg)

def recursiveDeleteChecks(owner_factory, exclusion_accessors, recycler = None):
  """
  Recursively verifies that an object and its attributes are deleted when
  the object is no longer externally referenced. owner_factory should be a
  callable object taking no arguments that returns an object to be tested.
  exclusion_accessors should be a list of strings of the form
  '.attribute_a.attribute_b...' indicating object attributes that are
  expected to be not deleted.  recycler should be callable, taking the
  object as an argument, and should perform any required cleanup of the
  object.
  """

  class DeletionDetector:
    """
    A helper class. DeletionDetector objects are instantiated with an
    endotest from the pmock package, and a DeletionDetector is 'inserted'
    into each attribute of an object under deletion checks.  When the
    attribute is deleted, the DeletionDetector is deleted, and marks the
    delete in the endotest.
    """
    def __init__(self, endotest):
      self.m_endotest = endotest
    def __del__(self):
      self.m_endotest.mark_a_delete()

  def makeDeletionDetector(obj, obj_name, matcher):
    """
    A helper function. makeDeletionDetector inserts a DeletionDetector
    into obj, and returns the pair (endotest, obj_name), where the
    endotest expects obj to be deleted if matcher is pmock.once() or
    pmock.once_no_raise(), and expects obj to be not deleted if matcher
    is pmock.never() or pmock.never_no_raise().
    """
    # ignore objects that are classes (or sip-wrapped classes).
    if type(obj) != types.ClassType and type(obj) != type(qt.QObject):
      endotest=pmock.Mock()
      endotest.expects(matcher).mark_a_delete().id(obj_name)
      try:
        # dictionaries and lists have to have delete detector inserted
        # in special way.
        if type(obj) == types.DictType: obj["verifyDeletes"] = DeletionDetector(endotest)
        elif type(obj) == types.ListType: obj.append(DeletionDetector(endotest))
        else: obj.m_delete_detector = DeletionDetector(endotest)
        return (endotest, obj_name)
      except:
        pass

  def enqueueAndMark(attr, attr_name, id_dict, object_queue):
    id_dict[id(attr)] = attr_name
    object_queue.append(attr)

  # create object to recursively test for deletes.
  owner = owner_factory()
  # queue for breadth-first search of owner's attributes.
  object_queue = [owner]
  # ids of attributes which caller says shouldn't be deleted.
  excluded_id_dict = {}
  # ids of attributes already examined.
  id_dict = {id(owner):''}
  # return list of delete endotests and corresponding names of
  # endotested attributes; caller will verify these endotests.
  endotest_list = []

  # find attributes that caller says shouldn't be deleted.
  for attr_name in exclusion_accessors:
    get_buried_attribute = eval('lambda x: x' + attr_name)
    try: attr = get_buried_attribute(owner)
    except AttributeError: raise BuriedAttributeError(attr_name)
    if not excluded_id_dict.has_key(id(attr)):
      result = makeDeletionDetector(attr, attr_name, pmock.never_no_raise())
      if(result): endotest_list.append(result)
      excluded_id_dict[id(attr)] = attr_name

  # breadth-first search of owner's attributes.
  while len(object_queue):
    o = object_queue.pop(0)
    # ignore objects that are classes (or sip-wrapped classes).
    if type(o) != types.ClassType and type(o) != type(qt.QObject):
      # to make things easier on troubleshooting programmer, when we
      # encounter lists and dictionaries, store some feedback about the
      # list indices and dictionaries keys; later, for each endotest of
      # an object in a list, the corresponding stored name of the object
      # will have the index embedded in it. similarly, embedded keys for
      # undeleted objects in dictionaries.
      if type(o) == types.ListType:
        for i in range(len(o)):
          attr = o[i]
          if not id_dict.has_key(id(attr)):
            enqueueAndMark(attr, id_dict[id(o)] + '[%d]'%i + str(attr), id_dict, object_queue)
      elif type(o) == types.DictType:
        for k in o.keys():
          attr = o[k]
          if not id_dict.has_key(id(attr)):
            enqueueAndMark(attr, id_dict[id(o)] + '[%s]'%str(k) + str(attr), id_dict, object_queue)
      elif hasattr(o, '__dict__'):
        for name in o.__dict__.keys():
          attr = getattr(o,name)
          if not id_dict.has_key(id(attr)):
            if name[:2] == 'm_' and not name == 'm_delete_detector':
              enqueueAndMark(attr, id_dict[id(o)] + '.' + name, id_dict, object_queue)

      if not excluded_id_dict.has_key(id(o)):
        result = makeDeletionDetector(o, id_dict[id(o)], pmock.once_no_raise())
        if(result): endotest_list.append(result)

  # cleanup if caller wants.
  if recycler: recycler(owner)

  return endotest_list
