# Copyright (c) 2011-2014 Stefan Eilemann <eile@eyescale.ch>

#include_directories(
#  ${CMAKE_CURRENT_SOURCE_DIR})

#set(SPLOTCH_ROOT "/home/nachbaur/dev/viz.stable/splotch")
#set(SPLOTCH_LIBRARIES "${SPLOTCH_ROOT}/Splotch6-SuperMuc")

#include_directories(${SPLOTCH_ROOT})

stringify_shaders(FBO_Passthrough.frag FBO_Passthrough.vert FBO_ToneMap.frag FBO_ToneMap.vert PP_FBO.frag PP_FBO.geom PP_FBO.vert)
set(SEQSPLOTCH_HEADERS application.h renderer.h ../eqPly/frameData.h)
set(SEQSPLOTCH_SOURCES application.cpp renderer.cpp main.cpp ../eqPly/frameData.cpp ${SHADER_SOURCES})
set(SEQSPLOTCH_LINK_LIBRARIES Sequel splotch)
common_application(seqSplotch)
target_include_directories(seqSplotch PRIVATE ${PROJECT_SOURCE_DIR}/examples/eqPly ${CMAKE_CURRENT_BINARY_DIR})

#eq_add_example(seqPly
#  HEADERS
#    application.h
#    renderer.h
#  SOURCES
#    application.cpp
#    main.cpp
#    renderer.cpp
#    ../eqPly/frameData.h
#    ../eqPly/frameData.cpp
#  LINK_LIBRARIES
#    Sequel triply
#  )
