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

add_subdirectory(libcurt)

SET(APP_SRC
    cu/Copper.cpp
)

add_executable(cu ${APP_SRC})

target_include_directories(cu PUBLIC libcurt/include)
target_link_libraries(cu PUBLIC curt)

target_compile_definitions(cu PRIVATE
	COMPILER_NAME="${CMAKE_CXX_COMPILER_ID}"
	COMPILER_VERSION="${CMAKE_CXX_COMPILER_VERSION}"
	PLATFORM="${CMAKE_HOST_SYSTEM_NAME}"
	COPPER_VERSION="${CopperVM_VERSION}"
)

