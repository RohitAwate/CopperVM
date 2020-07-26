cmake_minimum_required(VERSION 3.10.2)
project(CopperVM VERSION 0.1
				DESCRIPTION A bytecode virtual machine for a sensible subset of JavaScript."
				LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_BUILD_TYPE Debug)

set(LIB_SRC
	src/Tokenizer.cpp
)

add_library(libcopper ${LIB_SRC})
target_include_directories(libcopper PUBLIC include)
set_target_properties(libcopper PROPERTIES PREFIX "")

add_executable(copper app/copper.cpp)
target_link_libraries(copper PUBLIC libcopper)
