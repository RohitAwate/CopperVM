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
#include "Tokenizer.h"

namespace cu {

	static void error() {
		std::cout << "Compilation failed." << std::endl;
	}

	bool Compiler::compile(TranslationUnit& translationUnit) {
		Tokenizer tokenizer(translationUnit);
		if (!tokenizer.tokenize()) {
			error();
			return false;
		}

		auto tokens = tokenizer.getTokens();

#ifdef TOKENIZE
		for (const auto &token : tokens) {
			std::cout << toString(token.getType()) << " ";
			std::cout << (token.getType() == TokenType::STRING ? "'" : "");
			std::cout << token.getLexeme();
			std::cout << (token.getType() == TokenType::STRING ? "'" : "");
			std::cout << " [" << token.getLine() << ":" << token.getColumn() << "]" << std::endl;
		}
#endif

		parser.reset();
		if (!parser.parse(translationUnit, tokens)) {
			error();
			return false;
		}

#ifdef DISASSEMBLE
		cu::Disassembler disassembler;
		disassembler.disassemble(parser.getBytecode(), translationUnit);
#endif

		return true;
	}

} // namespace cu