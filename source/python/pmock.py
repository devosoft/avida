"""
Python mock object framework, providing support for creating mock
objects for use in unit testing.

The api is modelled on the jmock mock object framework.

Usage::

    import pmock
    import unittest

    class PowerStation(object):
        def start_up(self, reactor):
            try:
                reactor.activate('core')
            except Exception, err:
                reactor.shutdown()

    class PowerStationTestCase(unittest.TestCase):
        def test_successful_activation(self):
            mock = pmock.Mock()
            mock.expects(pmock.once()).activate(pmock.eq('core'))
            PowerStation().start_up(mock)
            mock.verify()
        def test_problematic_activation(self):
            mock = pmock.Mock()
            mock.expects(pmock.once()).activate(pmock.eq('core')).will(
                pmock.throw_exception(RuntimeError('overheating')))
            mock.expects(pmock.once()).shutdown()
            PowerStation().start_up(mock)
            mock.verify()

    if __name__ == '__main__':
        unittest.main()

Further information is available in the bundled documentation, and from
http://pmock.sourceforge.net/

Copyright (c) 2004, Graham Carlyle

This module is free software, and you may redistribute it and/or modify
it under the same terms as Python itself, so long as this copyright message
and disclaimer are retained in their original form.
 
IN NO EVENT SHALL THE AUTHOR BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT,
SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OF
THIS CODE, EVEN IF THE AUTHOR HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH
DAMAGE.
 
THE AUTHOR SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
PARTICULAR PURPOSE.  THE CODE PROVIDED HEREUNDER IS ON AN "AS IS" BASIS,
AND THERE IS NO OBLIGATION WHATSOEVER TO PROVIDE MAINTENANCE,
SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
"""

__author__ = "Graham Carlyle"
__email__ = "grahamcarlyle at users dot sourceforge dot net"
__version__ = "0.3"

import unittest

##############################################################################
# Exported classes and functions
##############################################################################

__all__ = ["Mock", "MockTestCase",
           "once", "once_no_raise", "at_least_once", "never", "never_no_raise",
           "eq", "same", "string_contains", "functor",
           "return_value", "raise_exception"]


##############################################################################
# Mock objects framework
##############################################################################

class Error(AssertionError):

    def __init__(self, msg):
        AssertionError.__init__(self, msg)
        self.msg = msg

    def _mockers_str(cls, mockers):
        mockers_strs = [str(mocker) for mocker in mockers]
        return ", ".join(mockers_strs)

    _mockers_str = classmethod(_mockers_str)


class VerificationError(Error):
    """An expectation have failed verification."""
    
    def create_error(cls, msg, verified_invokable):
        err_msg = "%s: %s" % (msg, verified_invokable)
        return VerificationError(err_msg)

    create_error = classmethod(create_error)


class MatchError(Error):
    """Method call unexpected."""
    
    def create_error(cls, msg, invocation, mock):
        err_msg = "%s\ninvoked %s" % (msg, invocation)
        invokables_str = mock.invokables_str()
        if invokables_str != "":
            err_msg += "\nin:\n" + invokables_str
        return MatchError(err_msg)

    create_error = classmethod(create_error)
    

class DefinitionError(Error):
    """Expectation definition isn't valid."""

    def create_unregistered_id_error(cls, unregistered_id):
        msg = "reference to undefined id: %s" % unregistered_id
        return DefinitionError(msg)

    create_unregistered_id_error = classmethod(
        create_unregistered_id_error)

    def create_duplicate_id_error(cls, builder_id):
        msg = "id: %s is already defined" % builder_id
        return DefinitionError(msg)

    create_duplicate_id_error = classmethod(create_duplicate_id_error)


class InvocationMocker(object):
    
    def __init__(self, invocation_matcher):
        self._matchers = []
        self._invocation_matcher = invocation_matcher
        self._matchers.append(invocation_matcher)
        self._stub = None
        self._id = None

    def __str__(self):
        strs = ["%s: " % str(self._invocation_matcher)]
        for matcher in self._matchers[1:]:
            strs.append(str(matcher))
        if self._stub is not None:
            strs.append(", %s" % self._stub)
        if self._id is not None:
            strs.append(" [%s]" % self._id)
        return "".join(strs)

    def add_matcher(self, matcher):
        self._matchers.append(matcher)

    def set_stub(self, stub):
        self._stub = stub

    def invoke(self, invocation):
        for matcher in self._matchers:
            matcher.invoked(invocation)
        if self._stub is not None:
            return self._stub.invoke(invocation)

    def matches(self, invocation):
        for matcher in self._matchers:
            if not matcher.matches(invocation):
                return False
        return True

    def set_id(self, mocker_id):
        self._id = mocker_id

    def verify(self):
        try:
            for matcher in self._matchers:
                matcher.verify()
        except AssertionError, err:
            raise VerificationError.create_error(str(err), self)


class AbstractArgumentsMatcher(object):

    def __init__(self, arg_constraints=(), kwarg_constraints={}):
        self._arg_constraints = arg_constraints
        self._kwarg_constraints = kwarg_constraints

    def _arg_strs(self):
        arg_strs = [str(c) for c in self._arg_constraints]
        keywords = self._kwarg_constraints.keys()
        keywords.sort()
        for kw in keywords:
            constraint = self._kwarg_constraints[kw]
            arg_strs.append("%s=%s" % (kw, str(constraint)))
        return arg_strs

    def _matches_args(self, invocation):
        for i, constraint in enumerate(self._arg_constraints):
            if not constraint.eval(invocation.args[i]):
                return False
        return True

    def _matches_kwargs(self, invocation):
        for kw, constraint in self._kwarg_constraints.iteritems():
            if (not invocation.kwargs.has_key(kw) or
                not constraint.eval(invocation.kwargs[kw])):
                return False
        return True

    def matches(self, invocation):
        return (self._matches_args(invocation) and
                self._matches_kwargs(invocation))

    def invoked(self, invocation):
        pass

    def verify(self):
        pass


class LeastArgumentsMatcher(AbstractArgumentsMatcher):

    def __str__(self):
        arg_strs = AbstractArgumentsMatcher._arg_strs(self)
        arg_strs.append("...")
        return "(%s)" % ", ".join(arg_strs)

    def _matches_args(self, invocation):
        if len(self._arg_constraints) > len(invocation.args):
            return False
        return AbstractArgumentsMatcher._matches_args(self, invocation)


ANY_ARGS_MATCHER = LeastArgumentsMatcher()


class AllArgumentsMatcher(AbstractArgumentsMatcher):

    def __str__(self):
        return "(%s)" % ", ".join(AbstractArgumentsMatcher._arg_strs(self))
        
    def _matches_args(self, invocation):
        if len(self._arg_constraints) != len(invocation.args):
            return False
        return AbstractArgumentsMatcher._matches_args(self, invocation)

    def _matches_kwargs(self, invocation):
        for invocation_kw in invocation.kwargs.iterkeys():
            if invocation_kw not in self._kwarg_constraints:
                return False
        return AbstractArgumentsMatcher._matches_kwargs(self, invocation)


NO_ARGS_MATCHER = AllArgumentsMatcher()


class MethodMatcher(object):

    def __init__(self, name):
        self._name = name

    def __str__(self):
         return self._name

    def matches(self, invocation):
        return invocation.name == self._name

    def invoked(self, invocation):
        pass

    def verify(self):
        pass


class InvokedAfterMatcher(object):

    def __init__(self, invocation_recorder, description):
        self._invocation_recorder = invocation_recorder
        self._description = description
        
    def __str__(self):
        return ".after(%s)" % self._description

    def matches(self, invocation):
        return self._invocation_recorder.has_been_invoked()

    def invoked(self, invocation):
        pass

    def verify(self):
        pass


class InvocationMockerBuilder(object):

    def __init__(self, mocker, builder_namespace):
        self._mocker = mocker
        self._builder_namespace = builder_namespace

    def __call__(self, *arg_constraints, **kwarg_constraints):
        self._mocker.add_matcher(AllArgumentsMatcher(arg_constraints,
                                                     kwarg_constraints))
        return self

    def __getattr__(self, name):
        """Define method name directly."""
        self._mocker.add_matcher(MethodMatcher(name))
        self._builder_namespace.register_method_name(name, self)
        return self

    def method(self, name):
        """Define method name."""
        self._mocker.add_matcher(MethodMatcher(name))
        self._builder_namespace.register_method_name(name, self)
        return self

    def with(self, *arg_constraints, **kwarg_constraints):
        """Fully specify the method's arguments."""
        self._mocker.add_matcher(AllArgumentsMatcher(arg_constraints,
                                                     kwarg_constraints))
        return self

    def with_at_least(self, *arg_constraints, **kwarg_constraints):
        """Specify the method's minimum required arguments."""
        self._mocker.add_matcher(LeastArgumentsMatcher(arg_constraints,
                                                       kwarg_constraints))
        return self

    def any_args(self):
        """Method takes any arguments."""
        self._mocker.add_matcher(ANY_ARGS_MATCHER)
        return self

    def no_args(self):
        """Method takes no arguments."""
        self._mocker.add_matcher(NO_ARGS_MATCHER)
        return self

    def will(self, stub):
        """Set stub when method is called."""
        self._mocker.set_stub(stub)
        return self

    def id(self, id_str):
        """Define a id for use in other mock's L{after} method."""
        self._mocker.set_id(id_str)
        self._builder_namespace.register_unique_id(id_str, self)
        return self

    def after(self, id_str, other_mock=None):
        """Expected to be called after the method with supplied id."""
        if other_mock is not None:
            builder_namespace = other_mock
            description = "%s on mock %s" % (repr(id_str),
                                             repr(other_mock.get_name()))
        else:
            builder_namespace = self._builder_namespace
            description = repr(id_str)
        builder = builder_namespace.lookup_id(id_str)
        if builder is None:
            raise DefinitionError.create_unregistered_id_error(id_str)
        invocation_recorder = InvokedRecorderMatcher()
        builder.match(invocation_recorder)
        matcher = InvokedAfterMatcher(invocation_recorder, description)
        self._mocker.add_matcher(matcher)
        return self

    def match(self, matcher):
        self._mocker.add_matcher(matcher)
        return self


class Invocation(object):

    def __init__(self, name, args, kwargs):
        self.name = name
        self.args = args
        self.kwargs = kwargs

    def __str__(self):
        arg_strs = [repr(arg) for arg in self.args]
        keywords = self.kwargs.keys()
        keywords.sort()
        for kw in keywords:
            arg_strs.append("%s=%s" % (kw, repr(self.kwargs[kw])))
        return "%s(%s)" % (self.name, ", ".join(arg_strs))


def extractAttributes(obj, names):
    return [object.__getattribute__(obj, name) for name in names]

def insertAttributes(obj, attrdict):
    for name in attrdict.keys(): object.__setattr__(obj, name, attrdict[name])


class BoundAttribute(object):

    def __init__(self, name, mock):
        insertAttributes(self, {"_name" : name, "_mock" : mock})

    def __call__(self, *args, **kwargs):
        (mock, name) = extractAttributes(self, ("_mock", "_name"))
        return mock.invoke(Invocation(name, args, kwargs))

    def __getattr__(self, attr_name):
        (mock, name) = extractAttributes(self, ("_mock", "_name"))
        return getattr(mock.invoke(Invocation("__getattr__", (name,), {})), attr_name)

    def __repr__(self):
        (mock, name) = extractAttributes(self, ("_mock", "_name"))
        return repr(mock.invoke(Invocation("__getattr__", (name,), {})))

    def __setattr__(self, attr_name, value):
        (mock, name) = extractAttributes(self, ("_mock", "_name"))
        setattr(mock.invoke(Invocation("__getattr__", (name,), {})), attr_name, value)

    def __str__(self):
        (mock, name) = extractAttributes(self, ("_mock", "_name"))
        return str(mock.invoke(Invocation("__getattr__", (name,), {})))


class DefaultStub(object):

    def invoke(self, invocation):
        raise AssertionError("no match found")

_DEFAULT_STUB = DefaultStub()


def _special(method_name):
    def mocked_special(self, *args, **kwargs):
        _invoke_special = object.__getattribute__(self, "_invoke_special")
        return _invoke_special(Invocation(method_name, args, kwargs))
    return mocked_special

class SpecialsMock(object):

    __call__ = _special("__call__")
    __cmp__ = _special("__cmp__")
    # assume no good reason to mock __del__
    __delattr__ = _special("__delattr__")
    # __eq__, __ne__, etc. comparison operators covered by __cmp__
    # __getattr__ & __getattribute__ needed for implementation
    __hash__ = _special("__hash__")
    # __init__ & __new__ needed for implementation
    __nonzero__ = _special("__nonzero__")
    __repr__ = _special("__repr__")
    __setattr__ = _special("__setattr__")
    __str__ = _special("__str__")
    # __unicode__ available if __str__ defined


class Proxy(SpecialsMock):
    """A proxy for a mock object."""
    
    def __init__(self, mock):
        object.__setattr__(self, "_mock", mock)

    def __getattr__(self, attr_name):
        mock = object.__getattribute__(self, "_mock")
        return BoundAttribute(attr_name, mock)

    def _invoke_special(self, invocation):
        mock = object.__getattribute__(self, "_mock")
        return mock._invoke_special(invocation)



def mock_str(mock):
    return "<pmock.Mock id=%s>" % id(mock)


class Mock(SpecialsMock):
    """A mock object."""

    def __init__(self, name=None):
        insertAttributes(self, {
            "_name" : name,
            "_invokables" : [],
            "_proxy" : Proxy(self),
            "_default_stub" : _DEFAULT_STUB,
            "_id_table" : {}
        })

    def __getattr__(self, attr_name):
        return BoundAttribute(attr_name, self)

    def get_name(self):
        if self._name is not None:
            return self._name
        else:
            return mock_str(self)

    def _get_match_order_invokables(self):
        return self._invokables[::-1] # LIFO

    def lookup_id(self, builder_id):
        return self._id_table.get(builder_id, None)

    def register_unique_id(self, builder_id, builder):
        if self._id_table.has_key(builder_id):
            raise DefinitionError.create_duplicate_id_error(builder_id)
        self._id_table[builder_id] = builder

    def register_method_name(self, builder_id, builder):
        self._id_table[builder_id] = builder
        
    def invoke(self, invocation):
        try:
            matching_invokable = None
            for invokable in self._get_match_order_invokables():
                if invokable.matches(invocation):
                    return invokable.invoke(invocation)
            return self._default_stub.invoke(invocation)
        except AssertionError, err:
            raise MatchError.create_error(str(err), invocation, self)

    def _invoke_special(self, invocation):
        return self.invoke(invocation)
            
    def add_invokable(self, invokable):
        self._invokables.append(invokable)

    def invokables_str(self):
        invokable_strs = [str(invokable) for invokable in self._invokables]
        return ",\n".join(invokable_strs)

    def expects(self, invocation_matcher):
        """Define an expectation for a method.

        @return: L{InvocationMocker}
        """
        mocker = InvocationMocker(invocation_matcher)
        self.add_invokable(mocker)
        return InvocationMockerBuilder(mocker, self)

    def stubs(self):
        """Define a method that may or may not be called.

        @return: L{InvocationMocker}
        """
        mocker = InvocationMocker(_STUB_MATCHER_INSTANCE)
        self.add_invokable(mocker)
        return InvocationMockerBuilder(mocker, self)

    def set_default_stub(self, stub):
        """Set the default behaviour of undefined methods."""
        object.__setattr__(self, "_default_stub", stub)
        
    def proxy(self):
        """Return a proxy to the mock object.

        Proxies only have the mocked methods which may be useful if the
        mock's builder methods are in the way.
        """ 
        return self._proxy
    
    def verify(self):
        """Check that the mock object has been called as expected."""
        for invokable in self._get_match_order_invokables():
            invokable.verify()


class MockTestCase(unittest.TestCase):

    def __init__(self, methodName='runTest'):
        unittest.TestCase.__init__(self, methodName)
        self._test_method_name = methodName
        self._mocks = []
        
    def _auto_verified_test(self):
        self._real_test_method()
        for mock in self._mocks:
            mock.verify()
    
    def __call__(self, result=None):
        self._mocks = []
        self._real_test_method = getattr(self, self._test_method_name)
        setattr(self, self._test_method_name, self._auto_verified_test)
        unittest.TestCase.__call__(self, result)
        setattr(self, self._test_method_name, self._real_test_method)

    def mock(self):
        """Create a mock object that will be automatically verified
        after the test is run.
        """
        mock = Mock()
        self._mocks.append(mock)
        return mock


##############################################################################
# Mocked method stubs
############################################################################## 

class ReturnValueStub(object):
    
    def __init__(self, value):
        self._value = value

    def __str__(self):
        return "returns %s" % repr(self._value)

    def invoke(self, invocation):
        return self._value


def return_value(value):
    """Stub that returns the supplied value.    

    Convenience function for creating a L{ReturnValueStub} instance.
    """
    return ReturnValueStub(value)


class RaiseExceptionStub(object):

    def __init__(self, exception):
        self._exception = exception

    def __str__(self):
        return "raises %s" % self._exception

    def invoke(self, invocation):
        raise self._exception


def raise_exception(exception):
    """Stub that raises the supplied exception.    

    Convenience function for creating a L{RaiseExceptionStub} instance.
    """
    return RaiseExceptionStub(exception)


##############################################################################
# Invocation matchers
############################################################################## 

class InvokedRecorderMatcher(object):

    def __init__(self):
        self._invoked = False

    def has_been_invoked(self):
        return self._invoked
    
    def matches(self, invocation):
        return True

    def invoked(self, invocation):
        self._invoked = True

    def verify(self):
        pass

    
class OnceInvocationMatcher(InvokedRecorderMatcher):

    def __str__(self):
        if self.has_been_invoked():
            return "expected once and has been invoked"
        else:
            return "expected once"
    
    def matches(self, invocation):
        return not self.has_been_invoked()

    def verify(self):
        if not self.has_been_invoked():
            raise AssertionError("expected method was not invoked")


def once():
    """Method will be called only once.

    Convenience function for creating a L{OnceInvocationMatcher} instance.
    """
    return OnceInvocationMatcher()


class OnceNoRaiseInvocationMatcher(InvokedRecorderMatcher):

    def __init__(self):
        self._invoke_ct = 0

    def __str__(self):
        if self.has_been_invoked():
            return "expected once and has been invoked"
        else:
            return "expected once"
    
    def has_been_invoked(self):
        if self._invoke_ct > 1: return True
        else: return False
    
    def matches(self, invocation):
        return not self.has_been_invoked()

    def invoked(self, invocation):
        self._invoke_ct += 1

    def verify(self):
        if self._invoke_ct == 0:
            raise AssertionError("expected method was not invoked")
        if self._invoke_ct > 1:
            raise AssertionError("expected method to be invoked only once")


def once_no_raise():
    """Method will be called only once.

    Convenience function for creating a L{OnceInvocationMatcher} instance.
    """
    return OnceNoRaiseInvocationMatcher()


class AtLeastOnceInvocationMatcher(InvokedRecorderMatcher):

    def __str__(self):
        if self.has_been_invoked():
            return "expected at least once and has been invoked"
        else:
            return "expected at least once"

    def matches(self, invocation):
        return True

    def verify(self):
        if not self.has_been_invoked():
            raise AssertionError("expected method was not invoked")


def at_least_once():
    """Method will be called at least once.

    Convenience function for creating a L{AtLeastOnceInvocationMatcher}
    instance.
    """
    return AtLeastOnceInvocationMatcher()


class NotCalledInvocationMatcher(object):

    def __str__(self):
        return "expected not to be called"

    def invoked(self, invocation):
        raise AssertionError("expected method to be never invoked")

    def matches(self, invocation):
        return True

    def verify(self):
        pass


_NOT_CALLED_MATCHER_INSTANCE = NotCalledInvocationMatcher()


def never():
    """Method will not be called.

    Convenience function for getting a L{NotCalledInvocationMatcher} instance.
    """
    return _NOT_CALLED_MATCHER_INSTANCE


class NotCalledNoRaiseInvocationMatcher(InvokedRecorderMatcher):

    def __str__(self):
        return "expected not to be called"

    def invoked(self, invocation):
        InvokedRecorderMatcher.invoked(self, invocation)

    def matches(self, invocation):
        return True

    def verify(self):
        if self.has_been_invoked():
            raise AssertionError("method expected to be not invoked")


def never_no_raise():
    """Method will not be called.

    Convenience function for getting a L{NotCalledInvocationMatcher} instance.
    """
    return NotCalledNoRaiseInvocationMatcher()


class StubInvocationMatcher(object):

    def __str__(self):
        return "stub"
    
    def invoked(self, invocation):
        pass

    def matches(self, invocation):
        return True

    def verify(self):
        pass


_STUB_MATCHER_INSTANCE = StubInvocationMatcher()


##############################################################################
# Argument constraints
############################################################################## 

class EqConstraint(object):

    def __init__(self, expected):
        self._expected = expected

    def __repr__(self):
        return "%s.eq(%s)" % (__name__, repr(self._expected))

    def eval(self, arg):
        return self._expected == arg


def eq(expected):
    """Argument will be equal to supplied value.

    Convenience function for creating a L{EqConstraint} instance.
    """
    return EqConstraint(expected)


class SameConstraint(object):

    def __init__(self, expected):
        self._expected = expected

    def __repr__(self):
        return "%s.same(%s)" % (__name__, repr(self._expected))

    def eval(self, arg):
        return self._expected is arg


def same(expected):
    """Argument will be the same as the supplied reference.

    Convenience function for creating a L{SameConstraint} instance.
    """
    return SameConstraint(expected)


class StringContainsConstraint(object):

    def __init__(self, expected):
        self._expected = expected

    def __repr__(self):
        return "%s.string_contains(%s)" % (__name__, repr(self._expected))

    def eval(self, arg):
        return (arg is not None) and (arg.find(self._expected) != -1)


def string_contains(expected):
    """Argument contains the supplied substring.

    Convenience function for creating a L{StringContainsConstraint} instance.
    """
    return StringContainsConstraint(expected)


class FunctorConstraint(object):

    def __init__(self, boolean_functor):
        self._boolean_functor = boolean_functor

    def __repr__(self):
        return "%s.functor(%s)" % (__name__, repr(self._boolean_functor))

    def eval(self, arg):
        return self._boolean_functor(arg)


def functor(boolean_functor):
    """Supplied unary function evaluates to True when called with argument.

    Convenience function for creating a L{FunctorConstraint} instance.
    """
    return FunctorConstraint(boolean_functor)
