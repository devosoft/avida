
from AvidaCore import cTestCase

import inspect

class pyTestCase(cTestCase):
  def _extract_frame_info(self):
    frame = inspect.stack()[2]
    (file_name, line_number, function_name) = frame[1:4]
    condition_string = frame[4][frame[5]]
    return (condition_string.strip(), file_name, self.__class__.__name__ + '.' + function_name, line_number)
  def runTest(self):
    cTestCase.runTest(self)
    return self
  def test_non_error(self, condition, msg=""):
    (condition_string, file_name, function_name, line_number) = self._extract_frame_info()
    self.testNonErrorStub(
      condition, condition_string, file_name, function_name, line_number, msg)
  def test_is_true(self, condition, msg=""):
    (condition_string, file_name, function_name, line_number) = self._extract_frame_info()
    self.testIsTrueStub(
      condition, condition_string, file_name, function_name, line_number, msg)
  def test_int_is_equal(self, actual_value, expected_value, msg=""):
    (condition_string, file_name, function_name, line_number) = self._extract_frame_info()
    self.testIntIsEqualStub(
      actual_value, expected_value, condition_string, file_name, function_name, line_number, msg)
  def test_double_is_equal(self, actual_value, expected_value, msg=""):
    (condition_string, file_name, function_name, line_number) = self._extract_frame_info()
    self.testDoubleIsEqualStub(
      actual_value, expected_value, condition_string, file_name, function_name, line_number, 0.005, msg)
  def double_is_within_tolerance(self, actual_value, expected_value, tolerance, msg=""):
    (condition_string, file_name, function_name, line_number) = self._extract_frame_info()
    self.testDoubleIsEqualStub(
      actual_value, expected_value, condition_string, file_name, function_name, line_number, tolerance, msg)
  def test_string_is_equal(self, actual_value, expected_value, msg=""):
    (condition_string, file_name, function_name, line_number) = self._extract_frame_info()
    self.testStringIsEqualStub(
      actual_value, expected_value, condition_string, file_name, function_name, line_number, msg)
  def test_is_equal(self, actual_value, expected_value, msg=""):
    (condition_string, file_name, function_name, line_number) = self._extract_frame_info()
    self.testIsEqualStub(
      actual_value, expected_value, condition_string, file_name, function_name, line_number, msg)
