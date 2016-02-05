# Copyright (c) 2016 Daniel.Nachbaur@epfl.ch

stringify_shaders(
  FBO_Passthrough.frag
  FBO_Passthrough.vert
  FBO_ToneMap.frag
  FBO_ToneMap.vert
  PP_FBO.frag
  PP_FBO.geom
  PP_FBO.vert
)

set(SEQSPLOTCH_HEADERS
  application.h
  renderer.h
  ../eqPly/frameData.h
)

set(SEQSPLOTCH_SOURCES
  application.cpp
  renderer.cpp
  main.cpp
  ../eqPly/frameData.cpp ${SHADER_SOURCES}
)

set(SEQSPLOTCH_LINK_LIBRARIES Sequel splotch)
common_application(seqSplotch)
target_include_directories(seqSplotch
  PRIVATE ${PROJECT_SOURCE_DIR}/examples/eqPly ${CMAKE_CURRENT_BINARY_DIR})
