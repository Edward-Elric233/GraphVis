cmake_minimum_required(VERSION 3.21)
project(GraphVis)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


find_package(Qt5 COMPONENTS
        Core
        Widgets
        REQUIRED)

set(ogdfPath /home/edward/Software/OGDF/OGDF)
include_directories(${ogdfPath}/include)

aux_source_directory (./ SRC_LIST)
add_executable(GraphVis ${SRC_LIST} GraphicsViewZoom.cpp GraphicsViewZoom.h)
target_link_libraries(GraphVis
        Qt5::Core
        Qt5::Widgets
        ${ogdfPath}/libOGDF.a
        ${ogdfPath}/libCOIN.a
        pthread
        )
