/*
 * Copyright 2020 Rohit Awate
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <iostream>

#include "Compiler.h"
#include "Disassembler.h"
#include "Parser.h"
#include "Tokenizer.h"

#define DISASSEMBLE
#define TOKENIZE

namespace Copper {

	Bytecode Compiler::compile(const TranslationUnit& unit) {
		Tokenizer tokenizer(unit);
		auto tokens = tokenizer.tokenize();

#ifdef TOKENIZE
		for (const auto &token : tokens)
			std::cout << toString(token.getType()) << " " << token.getLexeme() << " [" << token.getLine() << ":" << token.getColumn() << "]" << std::endl;
#endif

		Parser parser(unit, tokens);
		if (parser.parse()) {
			auto bytecode = parser.getBytecode();
#ifdef DISASSEMBLE
			Copper::Disassembler disassembler(bytecode);
			disassembler.run();
#endif
			return bytecode;
		}

		std::cout << "Compilation failed." << std::endl;
		std::exit(1);
	}

} // namespace Copper