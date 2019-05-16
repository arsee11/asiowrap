cmake_minimum_required(VERSION 2.8)

set(BOOST_DIR /home/arsee/libs/boost_1_70_0)

add_compile_options(-g -std=c++11)

include_directories(${BOOST_DIR})
aux_source_directory(. ASIOW_TARGET_SRCS)
add_library(asiow ${ASIOW_TARGET_SRCS} )
