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
#include <unordered_map>

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

	void Tokenizer::emitToken(std::vector<Token>& tokens, const TokenType type, std::string lexeme) {
		tokens.push_back(Token(lexeme, type, m_line, m_column));
		m_column += lexeme.size();
	}

	static const std::unordered_map<std::string, TokenType> KEYWORDS = {
		{"let", TokenType::LET},
		{"const", TokenType::CONST},
		{"if", TokenType::IF},
		{"else", TokenType::ELSE},
		{"switch", TokenType::SWITCH},
		{"case", TokenType::CASE},
		{"for", TokenType::FOR},
		{"while", TokenType::WHILE},
		{"do", TokenType::DO},
		{"of", TokenType::OF},
		{"in", TokenType::IN},
		{"break", TokenType::BREAK},
		{"continue", TokenType::CONTINUE},
		{"function", TokenType::FUNCTION},
		{"return", TokenType::RETURN},
		{"true", TokenType::TRUE},
		{"false", TokenType::FALSE},
		{"null", TokenType::NULL_TYPE},
		{"undefined", TokenType::UNDEFINED},
		{"class", TokenType::CLASS},
		{"this", TokenType::THIS},
		{"new", TokenType::NEW},
		{"static", TokenType::STATIC},
		{"extends", TokenType::EXTENDS},
		{"super", TokenType::SUPER},
		{"typeof", TokenType::TYPEOF},
		{"instanceof", TokenType::INSTANCEOF},
		{"try", TokenType::TRY},
		{"catch", TokenType::CATCH},
		{"throw", TokenType::THROW},
		{"import", TokenType::IMPORT},
		{"export", TokenType::EXPORT},
		{"default", TokenType::DEFAULT},
		{"async", TokenType::ASYNC},
		{"await", TokenType::AWAIT},
	};

	std::vector<Token> Tokenizer::run() {
		std::vector<Token> tokens;

		while (!atEOF()) {
			skipWhitespace();

			if (isDigit(peek())) {
				const auto num = number();
				emitToken(tokens, TokenType::NUMBER, num);
				continue;
			}

			if (isAlpha(peek())) {
				const auto id = identifier();
				const auto iterator = KEYWORDS.find(id);
				if (iterator != KEYWORDS.end()) {
					emitToken(tokens, iterator->second, id);
					continue;
				}

				emitToken(tokens, TokenType::IDENTIFIER, id);
				continue;
			}

			switch (peek()) {
				SINGLE_CHAR_TOKEN('%', MODULO);
				SINGLE_CHAR_TOKEN('{', OPEN_BRACE);
				SINGLE_CHAR_TOKEN('}', CLOSE_BRACE);
				SINGLE_CHAR_TOKEN('(', OPEN_PAREN);
				SINGLE_CHAR_TOKEN(')', CLOSE_PAREN);
				SINGLE_CHAR_TOKEN('[', OPEN_SQUARE_BRACKET);
				SINGLE_CHAR_TOKEN(']', CLOSE_SQUARE_BRACKET);
				SINGLE_CHAR_TOKEN(':', COLON);
				SINGLE_CHAR_TOKEN(';', SEMICOLON);
				SINGLE_CHAR_TOKEN('.', DOT);
				SINGLE_CHAR_TOKEN(',', COMMA);

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
				DOUBLE_CHAR_TOKEN('>', GREATER_THAN, '=', GREATER_EQUAL);
				DOUBLE_CHAR_TOKEN('<', LESS_THAN, '=', LESS_EQUAL);
				DOUBLE_CHAR_TOKEN('!', NEGATION, '=', NOT_EQUAL);

				case '/': {
					switch (peekNext()) {
						case '=':
							emitToken(tokens, TokenType::DIVIDE_ASSIGNMENT, 2);
							advance();
							break;
						case '/':
							// Single line comment
							while (!atEOF() && peek() != '\n') {
								advance();
							}
							
							if (peek() == '\n') {
								m_line++;
								m_column = 1;
							}
							break;
						default:
							emitToken(tokens, TokenType::DIVIDE);
					}

					break;
				}
				
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

				case '"':
				case '\'':
					emitToken(tokens, TokenType::STRING, string());
					break;

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

	inline bool Tokenizer::isDigit(char c) {
		return c >= '0' && c <= '9';
	}

	std::string Tokenizer::number() {
		int len = 0;
		int start = m_curr;
		while (isDigit(peek())) {
			advance();
			len++;
		}

		if (peek() == '.' && isDigit(peekNext())) {
			advance();
			len++;

			while (isDigit(peek())) {
				advance();
				len++;
			}	
		}

		return m_input.get()->substr(start, len);
	}

	inline bool Tokenizer::isAlpha(char c) {
		return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
	}

	std::string Tokenizer::identifier() {
		int len = 0;

		// consume the current character,
		// which has already been verified to be
		// alpha by the isAlpha() call in run()
		int start = m_curr;
		advance(); len++;

		while (isAlpha(peek()) || isDigit(peek())) {
			advance(); len++;
		}

		return m_input.get()->substr(start, len);
	}

	std::string Tokenizer::string() {
		int len = 0;

		// string literal may use single or double
		// quotation marks, hence we store which one
		// was used in order to find it's pair
		const char openingQuote = peek();

		// consume the opening quotation mark
		advance();
		int start = m_curr;

		while (!atEOF() && peek() != openingQuote && peek() != '\n') {
			advance(); len++;
		}

		if (atEOF() || peek() == '\n') {
			error("Unterminated string literal");
		}

		return m_input.get()->substr(start, len);
	}

	static std::string getOffsetString(const std::string& line, size_t offset) {
		std::string offsetString;

		for (size_t i = 0; i < offset; i++) {
			if (line.at(i) == '\t') {
				offsetString.append("\t");
			} else {
				offsetString.append(" ");
			}
		}

		return offsetString;
	}

	void Tokenizer::error(const char *msg) const {
		std::cout << ANSICodes::RED << ANSICodes::BOLD << "error: " << ANSICodes::RESET;
		std::cout << ANSICodes::BOLD << m_filename << ANSICodes::RESET << " ";
		std::cout << "(line " << m_line << "): ";
		std::cout << msg << std::endl;

		std::string culpritLine = getLine(m_line);
		std::cout << "\t" << culpritLine << std::endl;
		std::cout << "\t" << getOffsetString(culpritLine, m_column - 1);
		std::cout << ANSICodes::RED << ANSICodes::BOLD << "↑" << ANSICodes::RESET << std::endl;
	}

	std::string Tokenizer::getLine(int line) const {
		size_t start = 0, end = m_input.get()->size();

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
