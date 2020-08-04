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

#include "Token.h"

namespace Copper
{
	
	class Parser {
	public:
		Parser(std::vector<Token> tokens) : m_tokens(tokens) {}

		void parse();
	private:
		std::vector<Token> m_tokens;
		size_t m_curr { 0 }; 

		const Token peek() const;
		const Token next();
		bool atEOF() const;

		/*
			These Boolean return values form the synchronization
			mechanism in the parser. If an unexpected or invalid token
			is encountered, an error is reported and we unwind the
			call stack by returning false.
		*/
		bool expression();
		bool term();
		bool factor();
		bool grouping();

		void error(const char* msg) const;
	};

} // namespace Copper
