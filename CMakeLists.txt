cmake_minimum_required(VERSION 3.10)
project(hcore C)


set(CMAKE_INSTALL_PREFIX "/home/huashi/code/")
set(CMAKE_BUILD_TYPE Debug)



include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
)
link_directories(${CMAKE_SOURCE_DIR}/lib)

file(GLOB HC_SRCS "src/*.c")
add_library(hcore SHARED ${HC_SRCS} )
set(HC_3RD libzlog.a libiniparser.a)
target_link_libraries(hcore ${HC_3RD})

# install
# file(GLOB HC_INCS "*.h")
install(
    DIRECTORY ${CMAKE_SOURCE_DIR}/include/
    DESTINATION include/hcore
)
# install(
#     DIRECTORY ${CMAKE_SOURCE_DIR}/lib
#     DESTINATION .
# )
install(TARGETS
    hcore
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)



add_subdirectory(test)
