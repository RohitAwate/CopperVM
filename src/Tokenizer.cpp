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

#include "Colors.h"
#include "Tokenizer.h"

#define SINGLE_CHAR_TOKEN(c, TYPE)                                  \
	case c:                                                         \
		emitToken(tokens, TokenType::TYPE);							\
		break;

#define DOUBLE_CHAR_TOKEN(firstChar, SINGLE_TYPE, secondChar, DOUBLE_TYPE)      \
	case firstChar: {                                                           \
		if (matchNext(secondChar)) {                                            \
			emitToken(tokens, TokenType::DOUBLE_TYPE, 2);						\
		} else {                                                                \
			emitToken(tokens, TokenType::SINGLE_TYPE);							\
		}                                                                       \
		break;                                                                  \
	}

namespace Copper {

	void Tokenizer::emitToken(std::vector<Token>& tokens, const TokenType type, int tokenLen = 1) {
		tokens.push_back(Token(type, m_line, m_column));
		m_column += tokenLen;
	}

	std::vector<Token> Tokenizer::run() {
		std::vector<Token> tokens;

		while (!atEOF()) {
			skipWhitespace();

			switch (peek()) {
				SINGLE_CHAR_TOKEN('%', MODULO);
				SINGLE_CHAR_TOKEN('{', OPEN_BRACE);
				SINGLE_CHAR_TOKEN('}', CLOSE_BRACE);
				SINGLE_CHAR_TOKEN('(', OPEN_PAREN);
				SINGLE_CHAR_TOKEN(')', CLOSE_PAREN);
				SINGLE_CHAR_TOKEN('[', OPEN_SQUARE_BRACKET);
				SINGLE_CHAR_TOKEN(']', CLOSE_SQUARE_BRACKET);
				SINGLE_CHAR_TOKEN(';', SEMICOLON);
			
				case '+': {
					switch (peekNext()) {
						case '+':
							emitToken(tokens, TokenType::PLUS_PLUS, 2);
							advance();
							break;
						case '=':
							emitToken(tokens, TokenType::PLUS_ASSIGNMENT, 2);
							advance();
							break;
						default:
							emitToken(tokens, TokenType::PLUS);
					}

					break;
				}

				case '-': {
					switch (peekNext()) {
						case '-':
							emitToken(tokens, TokenType::MINUS_MINUS, 2);
							advance();
							break;
						case '=':
							emitToken(tokens, TokenType::MINUS_ASSIGNMENT, 2);
							advance();
							break;
						default:
							emitToken(tokens, TokenType::MINUS);
					}

					break;
				}

				case '=': {
					switch (peekNext()) {
						case '=':
							emitToken(tokens, TokenType::EQUAL_EQUAL, 2);
							advance();
							break;
						case '>':
							emitToken(tokens, TokenType::ARROW, 2);
							advance();
							break;
						default:
							emitToken(tokens, TokenType::ASSIGNMENT);
					}

					break;
				}

				DOUBLE_CHAR_TOKEN('*', MULTIPLY, '=', MULTIPLY_ASSIGNMENT);
				DOUBLE_CHAR_TOKEN('/', DIVIDE, '=', DIVIDE_ASSIGNMENT);
				DOUBLE_CHAR_TOKEN('>', GREATER_THAN, '=', GREATER_EQUAL);
				DOUBLE_CHAR_TOKEN('<', LESS_THAN, '=', LESS_EQUAL);
				DOUBLE_CHAR_TOKEN('!', NEGATION, '=', NOT_EQUAL);
				
				case '&': {
					if (peekNext() == '&') {
						emitToken(tokens, TokenType::AND, 2);
						advance();
					} else {
						error("Invalid or unexpected token: '&'");
					}

					break;
				}

				case '|': {
					if (peekNext() == '|') {
						emitToken(tokens, TokenType::OR, 2);
						advance();
					} else {
						error("Invalid or unexpected token: '|'");
					}

					break;
				}

				default:
					m_column++;
			}

			advance();
		}

		return tokens;
	}

	void Tokenizer::skipWhitespace() {
		for (;;) {
			switch (peek()) {
				case ' ':
				case '\t':
				case '\r':
					advance();
					m_column++;
					break;
				case '\n':
					advance();
					m_line++;
					m_column = 1;
					break;
				default:
					return;
			}
		}
	}

	char Tokenizer::peek() const {
		if (atEOF()) {
			return '\0';
		}

		return m_input.get()->at(m_curr);
	}

	char Tokenizer::peekNext() const {
		if (m_curr + 1 >= m_input.get()->size()) {
			return '\0';
		}

		return m_input.get()->at(m_curr + 1);
	}

	bool Tokenizer::matchNext(char expected) {
		if (atEOF()) return false;
		if (peekNext() != expected) return false;

		m_curr++;
		return true;
	}

	void Tokenizer::advance() {
		m_curr++;
	}

	bool Tokenizer::atEOF() const {
		return m_curr >= m_input.get()->size();
	}

	void Tokenizer::error(const char *msg) const {
		std::cerr << ANSICodes::RED << ANSICodes::BOLD << "error: " << ANSICodes::RESET;
		std::cerr << ANSICodes::BOLD << m_filename << ANSICodes::RESET << " ";
		std::cerr << "(line " << m_line << "): ";
		std::cerr << msg << std::endl;
		std::cerr << "\t" << getLine(m_line) << std::endl;
		std::cerr << "\t" << std::string(m_column - 1, ' ');
		
		std::cerr << "^" << std::endl;
	}

	std::string Tokenizer::getLine(int line) const {
		size_t start = 0, end = 0;

		int currentLine = 1;
		for (size_t i = 0; i < m_input.get()->size(); i++) {
			if (m_input.get()->at(i) == '\n') {
				currentLine++;

				if (currentLine == line) {
					start = i + 1;
				} else if (currentLine == line + 1) {
					end = i - 1;
				}
			}
		}

		return m_input.get()->substr(start, end - start + 1);
	}

} // namespace Copper
