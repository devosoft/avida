INCLUDE_DIRECTORIES(${ALL_INC_DIRS})

ADD_EXECUTABLE(avida primitive.cc)

SET(avida_target_link_libs
  main classification cpu event analyze drivers analyze cpu tools actions
)

IF(NOT MSVC)
  SET(avida_target_link_libs
    ${avida_target_link_libs} pthread
  )
ENDIF(NOT MSVC)

IF(AVD_ENABLE_TCMALLOC)
  SET(avida_target_link_libs
    ${avida_target_link_libs} tcmalloc
  )
ENDIF(AVD_ENABLE_TCMALLOC)

IF(AVD_SERIALIZATION)
  SET(avida_target_link_libs
    ${avida_target_link_libs} archive boost_serialization
  )
ENDIF(AVD_SERIALIZATION)

SET(avida_target_link_libs
  ${avida_target_link_libs} main
)

TARGET_LINK_LIBRARIES(avida ${avida_target_link_libs})
LINK_DIRECTORIES(${ALL_LIB_DIRS})
INSTALL_TARGETS(/work avida)
