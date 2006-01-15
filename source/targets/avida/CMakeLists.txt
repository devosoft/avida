INCLUDE_DIRECTORIES(${ALL_INC_DIRS})

ADD_EXECUTABLE(avida primitive.cc)
TARGET_LINK_LIBRARIES(avida main classification cpu event analyze drivers analyze cpu main tools)
LINK_DIRECTORIES(${ALL_LIB_DIRS})
INSTALL_TARGETS(/work avida)
