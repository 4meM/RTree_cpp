cmake_minimum_required(VERSION 3.29)
project(RTree LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

include(FetchContent)
FetchContent_Declare(SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 2.6.x
        GIT_SHALLOW ON
        EXCLUDE_FROM_ALL
        SYSTEM)
FetchContent_MakeAvailable(SFML)

add_executable(RTree main.cpp
        rtree/Global.hpp
        rtree/Point.hpp
        rtree/GeometryTraits.hpp
        rtree/aabb.hpp
        rtree/Iterator.hpp
        rtree/StaticNode.hpp
        rtree/StaticVector.hpp
        rtree/RStarSplit.hpp
        rtree/QuadraticSplit.hpp
        rtree/RTree.hpp)

target_link_libraries(RTree PRIVATE sfml-graphics)
target_compile_features(RTree PRIVATE cxx_std_17)

if(WIN32)
    add_custom_command(
            TARGET RTree
            COMMENT "Copy OpenAL DLL"
            PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll $<TARGET_FILE_DIR:RTree>
            VERBATIM)

endif()