cmake_minimum_required(VERSION 3.10.2)
project(CopperVM VERSION 0.1
				DESCRIPTION "A bytecode virtual machine for an opinionated, sensible subset of JavaScript."
				LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_BUILD_TYPE Debug)

if (MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -pedantic)
endif()

set(LIB_SRC
	src/Bytecode.cpp
	src/Disassembler.cpp
	src/Object.cpp
	src/Parser.cpp
	src/Tokenizer.cpp
	src/TranslationUnit.cpp
	src/VM.cpp
)

# libcurt: Library for the Copper (Cu) Runtime
add_library(libcurt SHARED ${LIB_SRC})
target_include_directories(libcurt PUBLIC include)
set_target_properties(libcurt PROPERTIES PREFIX "")

# cu: Because Copper's symbol is Cu
add_executable(cu app/Copper.cpp)
target_link_libraries(cu PUBLIC libcurt)

target_compile_definitions(cu PRIVATE
	COMPILER_NAME="${CMAKE_CXX_COMPILER_ID}"
	COMPILER_VERSION="${CMAKE_CXX_COMPILER_VERSION}"
	PLATFORM="${CMAKE_HOST_SYSTEM_NAME}"
	COPPER_VERSION="${CopperVM_VERSION}"
)
