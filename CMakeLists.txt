cmake_minimum_required(VERSION 3.29)
project(RTree)

set(CMAKE_CXX_STANDARD 20)

set(SFML_DIR "C:/SFML-2.5.1/lib/cmake/SFML")  # Adjust the path as necessary

find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

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

target_link_libraries(RTree sfml-graphics sfml-window sfml-system)
