set(LIB_SRC
	src/Bytecode.cpp
	src/Compiler.cpp
	src/Disassembler.cpp
	src/Environment.cpp
	src/Object.cpp
	src/Parser.cpp
	src/Tokenizer.cpp
	src/TranslationUnit.cpp
	src/VM.cpp
)

add_library(curt SHARED ${LIB_SRC})
target_include_directories(curt PUBLIC include)