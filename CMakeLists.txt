cmake_minimum_required(VERSION 3.10)
project(hbase C)


set(CMAKE_INSTALL_PREFIX "/home/huashi/code/app")
set(CMAKE_BUILD_TYPE Release)



include_directories(${CMAKE_SOURCE_DIR}/include)
link_directories(${CMAKE_SOURCE_DIR}/lib)

file(GLOB HBASE_SRCS "src/*.c")
add_library(hbase SHARED ${HBASE_SRCS})
target_link_libraries(hbase PUBLIC zlog)

# install
# file(GLOB HBASE_INCS "*.h")
install(
    DIRECTORY ${CMAKE_SOURCE_DIR}/include
    DESTINATION .
)
install(
    DIRECTORY ${CMAKE_SOURCE_DIR}/lib
    DESTINATION .
)
install(TARGETS
    hbase
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)



add_subdirectory(test)
