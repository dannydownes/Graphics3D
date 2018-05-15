project(GraphicsAssignment)
cmake_minimum_required(VERSION 3.0)


include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
CONAN_BASIC_SETUP()

ADD_EXECUTABLE(myGame src/main.cpp src/load.h src/load.cpp src/input.h src/input.cpp src/shader.cpp src/shader.h src/frag.shader src/vert.shader src/textVert.shader src/textFrag.shader src/lightVert.shader src/lightFrag.shader)
TARGET_LINK_LIBRARIES(myGame ${CONAN_LIBS})

set (CMAKE_CXX_STANDARD 11)
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT "myGame")

FILE(COPY assets/ DESTINATION bin/assets)
