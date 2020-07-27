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

namespace Copper {
	
	class Tokenizer {
	public:
		Tokenizer(const std::string filename, std::unique_ptr<std::string> input)
			: m_filename(filename), m_input(std::move(input)) {}

		std::vector<Token> run();
	private:
		std::string m_filename;
		std::unique_ptr<std::string> m_input;

		void emitToken(std::vector<Token>&, const TokenType, int length);
		void emitToken(std::vector<Token> &, const TokenType, std::string lexeme);

		int m_curr   { 0 };
		int m_line   { 1 };
		int m_column { 1 };

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

		void error(const char* msg) const;
		std::string getLine(int) const;
	};

}   // namespace Copper
