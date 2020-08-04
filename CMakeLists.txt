cmake_minimum_required(VERSION 3.10.2)
project(CopperVM VERSION 0.1
				DESCRIPTION "A bytecode virtual machine for an opinionated, sensible subset of JavaScript."
				LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_BUILD_TYPE Debug)

set(LIB_SRC
	src/Parser.cpp
	src/Tokenizer.cpp
)

# libcurt: Library for the Copper (Cu) Runtime
add_library(libcurt SHARED ${LIB_SRC})
target_include_directories(libcurt PUBLIC include)
set_target_properties(libcurt PROPERTIES PREFIX "")

# cu: Because Copper's symbol is Cu
add_executable(cu app/Copper.cpp)
target_link_libraries(cu PUBLIC libcurt)
