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

#include <string>

namespace Copper {
	enum class TokenType;
	
	class Token {
	public:
		Token(std::string lexeme, TokenType type) :
			m_lexeme(lexeme), m_type(type) {}
	private:
		std::string m_lexeme;
		TokenType m_type;
	};
	
	enum class TokenType {
		// Keywords
		LET, CONST,
		IF, ELSE, SWITCH, CASE,
		FOR, WHILE, DO, OF, IN,
		BREAK, CONTINUE, 
		FUNCTION, RETURN,
		TRUE, FALSE,
		NULL_TYPE, UNDEFINED,
		CLASS, THIS, NEW, STATIC, EXTENDS, SUPER,
		TYPEOF, INSTANCEOF,
		TRY, CATCH, THROW,
		IMPORT, EXPORT, DEFAULT,

		// Literals
		IDENTIFIER, STRING, NUMBER,
		
		// Single character tokens
		PLUS, MINUS, MULTIPLY, DIVIDE, MODULO,
		GREATER_THAN, LESS_THAN,
		OPEN_BRACE, CLOSE_BRACE,
		OPEN_PAREN, CLOSE_PAREN,
		OPEN_BRACKET, CLOSE_BRACKET,
		NEGATION, ASSIGNMENT,
		SEMICOLON,

		// Multiple character tokens
		PLUS_PLUS, MINUS_MINUS,
		DOUBLE_EQUAL, NOT_EQUAL,
		LESS_EQUAL, GREATER_EQUAL,
		AND, OR,
	};
}
