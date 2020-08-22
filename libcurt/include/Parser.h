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

#pragma once

#include <vector>

#include "Bytecode.h"
#include "Environment.h"
#include "Token.h"
#include "TranslationUnit.h"

namespace Copper {
	
	class Parser {
	public:
		Parser(const TranslationUnit translationUnit, std::vector<Token> tokens) :
			m_translationUnit(translationUnit), m_tokens(tokens) {}

		bool parse();
		Bytecode getBytecode() const;
	private:
		const TranslationUnit m_translationUnit;
		const std::vector<Token> m_tokens;
		size_t m_curr { 0 }; 

		Bytecode m_bytecode;
		Environment m_env;

		const Token& previous() const;
		const Token& peek() const;
		const Token& next();
		void consume();
		bool match(TokenType);
		bool atEOF() const;

		void synchronize();

		bool declaration();
		bool declarationList(const bool& isConst);
		bool singleDeclaration(const bool& isConst);
		bool statement();
		bool printStatement();
		bool expressionStatement();
		bool block();

		/*
			These Boolean return values form the synchronization
			mechanism in the parser. If an unexpected or invalid token
			is encountered, an error is reported and we unwind the
			call stack by returning false.
		*/
		bool expression();
		bool logicalOR();
		bool logicalAND();
		bool equality();
		bool comparison();
		bool term();
		bool factor();
		bool exponent();
		bool unary();
		bool primary();
		bool grouping();
		bool stringTemplate();

		void error(const std::string&) const;
	};

} // namespace Copper