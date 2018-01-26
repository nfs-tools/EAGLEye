cmake_minimum_required(VERSION 3.8)
project(EAGLEye)

set(CMAKE_CXX_STANDARD 17)

find_package(Boost COMPONENTS system filesystem program_options regex REQUIRED)

include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIR})

set(SOURCE_FILES main.cpp eaglutils.cpp eaglutils.h chunkbase.h chunkbase.cpp GameSupport/MW.cpp GameSupport/MW.h chunkstructures.h DataContext.cpp DataContext.h)
add_executable(EAGLEye ${SOURCE_FILES})
target_link_libraries(EAGLEye ${Boost_LIBRARIES})