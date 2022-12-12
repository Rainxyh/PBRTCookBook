cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 17)

project(Rattler)

set(CMAKE_BUILD_TYPE  "Release")

link_directories(${CMAKE_SOURCE_DIR}/3rdLib/Assimp-lib/)
link_directories(${CMAKE_SOURCE_DIR}/3rdLib/OpenGL-lib/)

find_package(OpenMP REQUIRED)
if(OpenMP_FOUND)
	message(STATUS "found openmp") 
	set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS}, ${OPENMP_C_FLAGS})
   	# set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS}, ${OPENMP_CXX_FLAGS})
	# set(CMAKE_CXX_FLAGS "-fopenmp -lassimp")
	set(CMAKE_CXX_FLAGS "-fopenmp")
else()
   	message(FATAL_ERROR "openmp not found!")
endif()

if(WIN32)
  set(LIBS glfw3 opengl32 assimp freetype irrKlang)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
elseif(UNIX AND NOT APPLE)
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
  find_package(OpenGL REQUIRED)
  add_definitions(${OPENGL_DEFINITIONS})
  find_package(X11 REQUIRED)
  # note that the order is important for setting the libs
  # use pkg-config --libs $(pkg-config --print-requires --print-requires-private glfw3) in a terminal to confirm
  set(LIBS ${GLFW3_LIBRARY} X11 Xrandr Xinerama Xi Xxf86vm Xcursor GL dl pthread freetype ${ASSIMP_LIBRARY})
  set (CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_CXX_LINK_EXECUTABLE} -ldl")
elseif(APPLE)
  INCLUDE_DIRECTORIES(/System/Library/Frameworks)
  FIND_LIBRARY(COCOA_LIBRARY Cocoa)
  FIND_LIBRARY(OpenGL_LIBRARY OpenGL)
  FIND_LIBRARY(IOKit_LIBRARY IOKit)
  FIND_LIBRARY(CoreVideo_LIBRARY CoreVideo)
  MARK_AS_ADVANCED(COCOA_LIBRARY OpenGL_LIBRARY)
  SET(APPLE_LIBS ${COCOA_LIBRARY} ${IOKit_LIBRARY} ${OpenGL_LIBRARY} ${CoreVideo_LIBRARY})
  SET(APPLE_LIBS ${APPLE_LIBS} ${GLFW3_LIBRARY} ${ASSIMP_LIBRARY} ${FREETYPE_LIBRARIES})
  set(LIBS ${LIBS} ${APPLE_LIBS})
else()
  set(LIBS )
endif(WIN32)



# include files
INCLUDE_DIRECTORIES(
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/3rdLib
)

# Third Party Library files
set(3rdLib
	3rdLib/stb_image.h
	3rdLib/stb_image_write.h
	3rdLib/stb_image_resize.h
	3rdLib/OpenGL/glfw3.h
	3rdLib/OpenGL/glfw3native.h
	3rdLib/OpenGL/khrplatform.h
	3rdLib/OpenGL/glad.h
	3rdLib/OpenGL/glad.c
)
# Make the 3rdLib group
SOURCE_GROUP("3rdLib" FILES ${3rdLib})

# Core files
set(Core
	Core/Common.h
	Core/Geometry.h
	Core/Geometry.cpp
	Core/Transform.h
	Core/Transform.cpp
	Core/Spectrum.h
	Core/Spectrum.cpp
)
# Make the Core group
SOURCE_GROUP("Core" FILES ${Core})

# Main files
set(Main
	Main/Main.cpp
	Main/ModelLoad.h
	Main/ModelLoad.cpp
	Main/File.h
	Main/File.cpp
	Main/ImageProcess.h
	Main/ImageProcess.cpp
)
# Make the Main group
SOURCE_GROUP("Main" FILES ${Main})

# Create executable
add_executable(Rattler
	${3rdLib}
	${Main}
	${Core}
)

FILE(GLOB Icons ${CMAKE_CURRENT_SOURCE_DIR}/Icons/*.png)
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${Icons} DESTINATION ${EXECUTABLE_OUTPUT_PATH}/Icons/)


add_library(GLAD "3rdLib/OpenGL/glad.c")
set(LIBS ${LIBS} GLAD)
message(${LIBS})

target_link_libraries(Rattler ${LIBS})

# 用于包含第三方库
set(AssimpLib ${CMAKE_CURRENT_SOURCE_DIR}/3rdLib/Assimp-lib/)
set(OpenGLLib ${CMAKE_CURRENT_SOURCE_DIR}/3rdLib/OpenGL-lib/)

FILE(GLOB ASSIMPDLL ${AssimpLib}/assimp-vc140-mt.dll)
file(COPY ${ASSIMPDLL} DESTINATION ${EXECUTABLE_OUTPUT_PATH})
FILE(GLOB OpenGLDLL ${OpenGLLib}/glfw3.dll)
file(COPY ${OpenGLDLL} DESTINATION ${EXECUTABLE_OUTPUT_PATH})

target_link_libraries(Rattler 
	${AssimpLib}assimp.lib
	${OpenGLLib}glfw3.lib
	${OpenGLLib}glfw3_mt.lib
	${OpenGLLib}glfw3dll.lib
)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Rattler)

get_target_property(prj_libs ${PROJECT_NAME} LINK_LIBRARIES)
foreach(lib ${prj_libs})
    message( STATUS "libs_list: ${lib}" )
endforeach()
