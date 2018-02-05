cmake_minimum_required(VERSION 3.8)
project(EAGLEye)

set(CMAKE_CXX_STANDARD 17)

find_package(Boost COMPONENTS system filesystem program_options regex REQUIRED)
find_package(PkgConfig REQUIRED)

pkg_search_module(GLM REQUIRED glm)

include_directories(${Boost_INCLUDE_DIR} ${GLM_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIR})

set(SOURCE_FILES
        main.cpp
        eaglutils.cpp
        eaglutils.h
        eagldefines.h
        eaglenums.h
        eagldata.h
        eagltypes.h
        Container.h
        containers/carbon/CarbonChunkyContainer.h
        containers/carbon/CarbonChunkyContainer.cpp
        containers/mw/MWChunkyContainer.h
        containers/mw/MWChunkyContainer.cpp
        containers/carbon/CarbonTPKContainer.cpp containers/carbon/CarbonTPKContainer.h data/tpk/TexturePack.cpp data/tpk/TexturePack.h)
add_executable(EAGLEye ${SOURCE_FILES})
target_link_libraries(EAGLEye ${Boost_LIBRARIES} ${GLM_LIBRARIES})