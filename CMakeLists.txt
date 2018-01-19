
#
# Параметры сборки:
#
# CMAKE_BUILD_TYPE=Debug или CMAKE_BUILD_TYPE=Release (по умолчанию)
#

cmake_minimum_required(VERSION 3.0)

project(rinex)

find_package(GDAL REQUIRED)
find_package(Eigen3 3.3 REQUIRED NO_MODULE)

set(CXX_FLAGS "-Wall -pipe -fPIC -std=c++17")
string(REPLACE ";" " " CXX_FLAGS " ${CXX_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX_FLAGS}" CACHE string "" FORCE)

include_directories(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR} ${GDAL_INCLUDE_DIR})

add_library(rinex SHARED rinex.cpp lib.cpp data_source.cpp layer.cpp obs/obs.cpp obs/epoch.cpp obs/sat.cpp nav/nav.cpp nav/sat.cpp)
target_link_libraries(rinex ${GDAL_LIBRARY} Eigen3::Eigen)

configure_file(rinex.pc.in rinex.pc)

install(TARGETS rinex DESTINATION lib)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/rinex.hpp DESTINATION include)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/rinex.pc DESTINATION lib/pkgconfig)

#############################################################################
# Testing

add_executable(test_1 test/base.cpp test/1.cpp)
target_link_libraries(test_1 rinex ${GDAL_LIBRARY})

