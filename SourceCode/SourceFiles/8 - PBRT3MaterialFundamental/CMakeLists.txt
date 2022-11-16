cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 11)

# set Qt moc rcc uic
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

project(Feimos)

set(CMAKE_BUILD_TYPE  "Debug")

set(QT_PATH "D:/DevTools/QT5/5.7/msvc2015_64" CACHE PATH "qt5 cmake dir")

set(CMAKE_PREFIX_PATH ${QT_PATH})

find_package(Qt5 COMPONENTS 
	Widgets 
	Gui 
REQUIRED)

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
	# 场景
	Core/Scene.h
	Core/Scene.cpp
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

# Camera files
set(Camera
	Camera/Camera.h
	Camera/Camera.cpp
	Camera/orthographic.h
	Camera/orthographic.cpp
	Camera/Perspective.h
	Camera/perspective.cpp
)
# Make the Camera group
SOURCE_GROUP("Camera" FILES ${Camera})

# Sampler files
set(Sampler
	Sampler/TimeClockRandom.h
	Sampler/Sampling.h
	Sampler/Sampling.cpp
	Sampler/RNG.h
	Sampler/LowDiscrepancy.h
	Sampler/LowDiscrepancy.cpp
	Sampler/SobolMatrices.h
	Sampler/SobolMatrices.cpp
	Sampler/Sampler.h
	Sampler/Sampler.cpp
	Sampler/halton.h
	Sampler/halton.cpp
	Sampler/clockRand.h
	Sampler/clockRand.cpp
)
# Make the Sampler group
SOURCE_GROUP("Sampler" FILES ${Sampler})


# Integrator files
set(Integrator
	Integrator/Integrator.h
	Integrator/Integrator.cpp
)
# Make the Integrator group
SOURCE_GROUP("Integrator" FILES ${Integrator})


# Material files
set(Material
	Material/Reflection.h
	Material/Reflection.cpp
	Material/Material.h
	Material/Material.cpp
	Material/MatteMaterial.h
	Material/MatteMaterial.cpp
)
# Make the Material group
SOURCE_GROUP("Material" FILES ${Material})


# Texture files
set(Texture
	Texture/Texture.h
	Texture/Texture.cpp
	Texture/ConstantTexture.h
	Texture/ConstantTexture.cpp
)
# Make the Texture group
SOURCE_GROUP("Texture" FILES ${Texture})


# Create executable
add_executable(Feimos
	WIN32
	${MainGUI}
	${Core}
	${Shape}
	${Accelerator}
	${Camera}
	${Sampler}
	${Integrator}
	${Material}
	${Texture}
)

FILE(GLOB Icons ${CMAKE_CURRENT_SOURCE_DIR}/Icons/*.png)
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${Icons} DESTINATION ${EXECUTABLE_OUTPUT_PATH}/Icons/)

target_link_libraries(Feimos 
	Qt5::Widgets
	Qt5::Gui
)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Feimos)




