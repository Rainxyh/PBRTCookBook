cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 11)

# set Qt moc rcc uic
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

project(PhotonMap)

set(CMAKE_BUILD_TYPE  "Release")

set(QT_PATH "D:/DevTools/QT5/5.7/msvc2015_64" CACHE PATH "qt5 cmake dir")

set(CMAKE_PREFIX_PATH ${QT_PATH})

find_package(Qt5 COMPONENTS 
	Widgets 
	Gui 
REQUIRED)

find_package(OpenMP REQUIRED)
if(OpenMP_FOUND)
	message(STATUS "found openmp") 
	set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS}, ${OPENMP_C_FLAGS})
   	set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS}, ${OPENMP_CXX_FLAGS})
else()
   	message(FATAL_ERROR "openmp not found!")
endif()


# include files
INCLUDE_DIRECTORIES(
	${QT_INCLUDE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/3rdLib
)

# Third Party Library files
set(3rdLib
	3rdLib/stb_image.h
	3rdLib/stb_image_write.h
	3rdLib/stb_image_resize.h
)
# Make the 3rdLib group
SOURCE_GROUP("3rdLib" FILES ${3rdLib})

# Main GUI files
set(MainGUI
	MainGUI/main.cpp
	MainGUI/MainWindow.h
	MainGUI/mainWindow.cpp
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
	Core/TimeClockRandom.h
	# 基本功能
	Core/change.h
	Core/Vector.h
	Core/ONB.h
	Core/HitableList.h
	Core/Hitable.h
	Core/Constant_medium.h
	Core/Constant_medium.cpp
	Core/Ray.h
	Core/PDF.h
	# 相机
	Core/Camera.h
	Core/Camera.cpp
	# 加速器
	Core/BoundingBox.h
	Core/BoundingBox.cpp
	Core/bvhTree.h
	Core/bvhTree.cpp
	# 形状
	Core/Triangle.h
	Core/Triangle.cpp
	Core/Rectangle.h
	Core/Box.h
	# 组件
	Core/Constant_medium.h
	Core/Constant_medium.cpp
	Core/Perlin.h
	Core/Perlin.cpp
	Core/texture.h
)
# Make the Core group
SOURCE_GROUP("Core" FILES ${Core})

# Create executable
add_executable(PhotonMap
	WIN32
	${3rdLib}
	${MainGUI}
	${Core}
)

FILE(GLOB Icons ${CMAKE_CURRENT_SOURCE_DIR}/Icons/*.png)
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${Icons} DESTINATION ${EXECUTABLE_OUTPUT_PATH}/Icons/)
# 用于包含第三方库
set(AssimpLib ${CMAKE_CURRENT_SOURCE_DIR}/3rdLib/Assimp-lib/)
#add_library(Feimos STATIC IMPORTED)
#set_target_properties(Feimos PROPERTIES
#IMPORTED_LOCATION_RELEASE ${AssimpLib}/assimp.lib
#IMPORTED_LOCATION_DEBUG ${AssimpLib}/assimp.lib
#IMPORTED_LINK_INTERFACE_LIBRARIES Feimos)
#add_library(assimpFeimos SHARED IMPORTED)
#set_property(TARGET assimpFeimos PROPERTY IMPORTED_LOCATION ${AssimpLib}assimp-vc140-mt.dll)
#set_property(TARGET assimpFeimos PROPERTY IMPORTED_IMPLIB ${AssimpLib}assimp.lib)
#set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
#set_target_properties(Feimos PROPERTIES INSTALL_RPATH ${AssimpLib})
FILE(GLOB ASSIMPDLL ${AssimpLib}/assimp-vc140-mt.dll)
file(COPY ${ASSIMPDLL} DESTINATION ${EXECUTABLE_OUTPUT_PATH})

target_link_libraries(PhotonMap 
	Qt5::Widgets
	Qt5::Gui
	${AssimpLib}assimp.lib
)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT PhotonMap)







