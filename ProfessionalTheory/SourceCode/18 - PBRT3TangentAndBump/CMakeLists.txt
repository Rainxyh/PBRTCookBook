cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "-fopenmp")

# set Qt moc rcc uic
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

project(Feimos)

set(CMAKE_BUILD_TYPE  "Release")

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
	# set(CMAKE_CXX_FLAGS "-fopenmp -lassimp")
	set(CMAKE_CXX_FLAGS "-fopenmp")
else()
   	message(FATAL_ERROR "openmp not found!")
endif()

find_package(assimp REQUIRED)
if (assimp_FOUND)
    message(STATUS "found assimp") 
    set(ASSIMP_LIBRARY "assimp")
    add_library(${ASSIMP_LIBRARY} SHARED IMPORTED)
    set_target_properties(${ASSIMP_LIBRARY} PROPERTIES IMPORTED_LOCATION "${ASSIMP_LIBRARY_DIRS}/libassimp.so")
else()
   	message(FATAL_ERROR "assimp not found!")
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
	Core/Geometry.h
	Core/Geometry.cpp
	Core/interaction.h
	Core/interaction.cpp
	Core/Primitive.h
	Core/Primitive.cpp
	Core/Transform.h
	Core/Transform.cpp
	Core/Spectrum.h
	Core/Spectrum.cpp
	Core/Memory.h
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
	Shape/ModelLoad.h
	Shape/ModelLoad.cpp
	Shape/RattlerLoad.h
	Shape/RattlerLoad.cpp
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
	Camera/Orthographic.h
	Camera/Orthographic.cpp
	Camera/Perspective.h
	Camera/Perspective.cpp
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
	Sampler/Halton.h
	Sampler/Halton.cpp
	Sampler/ClockRand.h
	Sampler/ClockRand.cpp
)
# Make the Sampler group
SOURCE_GROUP("Sampler" FILES ${Sampler})


# Integrator files
set(Integrator
	Integrator/Integrator.h
	Integrator/Integrator.cpp
	Integrator/WhittedIntegrator.h
	Integrator/WhittedIntegrator.cpp
	Integrator/DirectLightingIntegrator.h
	Integrator/DirectLightingIntegrator.cpp
	Integrator/PathIntegrator.h
	Integrator/PathIntegrator.cpp
	Integrator/VolPathIntegrator.h
	Integrator/VolPathIntegrator.cpp
)
# Make the Integrator group
SOURCE_GROUP("Integrator" FILES ${Integrator})


# Material files
set(Material
	Material/Reflection.h
	Material/Reflection.cpp
	Material/Microfacet.h
	Material/Microfacet.cpp
	Material/Material.h
	Material/Material.cpp
	Material/MatteMaterial.h
	Material/MatteMaterial.cpp
	Material/Fresnel.h
	Material/Fresnel.cpp
	Material/Mirror.h
	Material/Mirror.cpp
	Material/PlasticMaterial.h
	Material/PlasticMaterial.cpp
	Material/MetalMaterial.h
	Material/MetalMaterial.cpp
	Material/GlassMaterial.h
	Material/GlassMaterial.cpp
)
# Make the Material group
SOURCE_GROUP("Material" FILES ${Material})


# Texture files
set(Texture
	Texture/Texture.h
	Texture/Texture.cpp
	Texture/ConstantTexture.h
	Texture/ConstantTexture.cpp
	Texture/MIPMap.h
	Texture/MIPMap.cpp
	Texture/ImageTexture.h
	Texture/ImageTexture.cpp
)
# Make the Texture group
SOURCE_GROUP("Texture" FILES ${Texture})


# Light files
set(Light
	Light/Light.h
	Light/Light.cpp
	Light/PointLight.h
	Light/PointLight.cpp
	Light/DiffuseLight.h
	Light/DiffuseLight.cpp
	Light/SkyBoxLight.h
	Light/SkyBoxLight.cpp
	Light/LightDistrib.h
	Light/LightDistrib.cpp
	Light/InfiniteAreaLight.h
	Light/InfiniteAreaLight.cpp
)
# Make the Light group
SOURCE_GROUP("Light" FILES ${Light})

# Media files
set(Media
	Media/Medium.h
	Media/Medium.cpp
	Media/HomogeneousMedium.h
	Media/HomogeneousMedium.cpp
	Media/GridDensityMedium.h
	Media/GridDensityMedium.cpp
)
# Make the Media group
SOURCE_GROUP("Media" FILES ${Media})



# Create executable
add_executable(Feimos
	WIN32
	${3rdLib}
	${MainGUI}
	${Core}
	${Shape}
	${Accelerator}
	${Camera}
	${Sampler}
	${Integrator}
	${Material}
	${Texture}
	${Light}
	${Media}
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

target_link_libraries(Feimos 
	Qt5::Widgets
	Qt5::Gui
	${ASSIMP_LIBRARY}
)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Feimos)

