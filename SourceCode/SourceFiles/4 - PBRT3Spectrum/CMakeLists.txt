cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 14)

# set Qt moc rcc uic
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

project(Feimos)

set(CMAKE_BUILD_TYPE  "Debug")

set(QT_PATH "~/Qt5.14.2/5.14.2/gcc_64" CACHE PATH "qt5 cmake dir")

set(CMAKE_PREFIX_PATH ${QT_PATH})

find_package(Qt5 COMPONENTS 
	Widgets 
	Gui 
REQUIRED)

find_package(OpenMP REQUIRED)
if(OpenMP_FOUND)
	message(STATUS "found openmp") 
	set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS}, ${OPENMP_C_FLAGS})
   	# set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS}, ${OPENMP_CXX_FLAGS})
	set(CMAKE_CXX_FLAGS "-fopenmp")
else()
   	message(FATAL_ERROR "openmp not found!")
endif()

# include files
INCLUDE_DIRECTORIES(
	${QT_INCLUDE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}
)

# Main GUI files
set(MainGUI
	MainGUI/main.cpp
	MainGUI/MainWindow.h
	MainGUI/MainWindow.cpp
	MainGUI/DebugText.hpp
	MainGUI/DebugText.cpp
	MainGUI/DisplayWidget.h
	MainGUI/DisplayWidget.cpp
	MainGUI/IMAGraphicsView.h
	MainGUI/IMAGraphicsView.cpp
	MainGUI/RenderThread.h
	MainGUI/RenderThread.cpp
	MainGUI/InteractionDockWidget.h
	MainGUI/InteractionDockWidget.cpp
	MainGUI/DataTreeWidget.h
	MainGUI/DataTreeWidget.cpp
	MainGUI/RenderStatus.h
	MainGUI/RenderStatus.cpp
)
# Make the MainGUI group
SOURCE_GROUP("MainGUI" FILES ${MainGUI})

# Core files
set(Core
	Core/FeimosRender.h
	# 数据存储
	Core/FrameBuffer.h
	Core/FrameBuffer.cpp
	# 基本数据类型
	Core/Geometry.hpp
	Core/Geometry.cpp
	Core/interaction.h
	Core/interaction.cpp
	Core/primitive.h
	Core/primitive.cpp
	Core/Transform.h
	Core/Transform.cpp
	Core/Spectrum.h
	Core/Spectrum.cpp
)
# Make the Core group
SOURCE_GROUP("Core" FILES ${Core})

# Shape files
set(Shape
	Shape/plyRead.h
	Shape/Shape.h
	Shape/Shape.cpp
	Shape/Sphere.h
	Shape/Sphere.cpp
	Shape/Triangle.h
	Shape/Triangle.cpp
)
# Make the Shape group
SOURCE_GROUP("Shape" FILES ${Shape})

# Accelerator files
set(Accelerator
	Accelerator/BVHAccel.h
	Accelerator/BVHAccel.cpp
)
# Make the Accelerator group
SOURCE_GROUP("Accelerator" FILES ${Accelerator})

# Create executable
add_executable(Feimos
	WIN32
	${MainGUI}
	${Core}
	${Shape}
	${Accelerator}
)

FILE(GLOB Icons ${CMAKE_CURRENT_SOURCE_DIR}/Icons/*.png)
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${Icons} DESTINATION ${EXECUTABLE_OUTPUT_PATH}/Icons/)

target_link_libraries(Feimos 
	Qt5::Widgets
	Qt5::Gui
)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Feimos)




