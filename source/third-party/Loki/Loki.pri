
loki {
  HEADERS += \
    $$LOKI_HH/AbstractFactory.h \
    $$LOKI_HH/AssocVector.h \
    $$LOKI_HH/EmptyType.h \
    $$LOKI_HH/Factory.h \
    $$LOKI_HH/Functor.h \
    $$LOKI_HH/HierarchyGenerators.h \
    $$LOKI_HH/MultiMethods.h \
    $$LOKI_HH/NullType.h \
    $$LOKI_HH/Singleton.h \
    $$LOKI_HH/SmallObj.h \
    $$LOKI_HH/SmartPtr.h \
    $$LOKI_HH/static_check.h \
    $$LOKI_HH/Threads.h \
    $$LOKI_HH/Tuple.h \
    $$LOKI_HH/TypeInfo.h \
    $$LOKI_HH/Typelist.h \
    $$LOKI_HH/TypeManip.h \
    $$LOKI_HH/TypeTraits.h \
    $$LOKI_HH/Visitor.h

  SOURCES += \
    $$LOKI_CC/Singleton.cpp \
    $$LOKI_CC/SmallObj.cpp
}
