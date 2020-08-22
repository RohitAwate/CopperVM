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

#include <memory>
#include <vector>

#include "Token.h"
#include "TranslationUnit.h"

namespace Copper {
	
	class Tokenizer {
	public:
		Tokenizer(const TranslationUnit translationUnit)
			: translationUnit(translationUnit) {}

		bool tokenize();
		std::vector<Token>& getTokens();
	private:
		const TranslationUnit translationUnit;

		void emitToken(const TokenType, int length);
		void emitToken(const TokenType, std::string lexeme);

		size_t curr   			= 0;
		size_t line   			= 1;
		size_t column 			= 1;
		size_t interpolationDepth	= 0;

		bool hadError = false;

		std::vector<Token> tokens;
		
		void run();

		void skipWhitespace();
		char peek() const;
		char peekNext() const;
		bool matchNext(char);
		void advance();
		bool atEOF() const;

		static bool isDigit(char);
		std::string number();

		static bool isAlpha(char);
		std::string identifier();

		std::string string();
		void stringTemplate();

		void error(const std::string& msg);
	};

}   // namespace Copper
