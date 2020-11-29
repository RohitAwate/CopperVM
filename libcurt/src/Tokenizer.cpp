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
		emitToken(TokenType::TYPE);							\
		break;

#define DOUBLE_CHAR_TOKEN(firstChar, SINGLE_TYPE, secondChar, DOUBLE_TYPE)      \
	case firstChar: {                                                           \
		if (matchNext(secondChar)) {                                            \
			emitToken(TokenType::DOUBLE_TYPE, 2);						\
		} else {                                                                \
			emitToken(TokenType::SINGLE_TYPE);							\
		}                                                                       \
		break;                                                                  \
	}

namespace cu {

	void Tokenizer::emitToken(const TokenType type, int tokenLen = 1) {
		tokens.push_back(Token(type, line, column));
		column += tokenLen;
	}

	void Tokenizer::emitToken(const TokenType type, std::string lexeme) {
		tokens.push_back(Token(lexeme, type, line, column));
		column += lexeme.size();
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
		{"print", TokenType::PRINT},
	};

	bool Tokenizer::tokenize() {
		run();
		return !hadError;
	}

	std::vector<Token>& Tokenizer::getTokens() {
		return tokens;
	}

	void Tokenizer::run() {
		/*
			This keeps track of the number of open curly braces
			encountered so far. This is required for tokenizing
			template string literals properly.

			stringTemplate() calls run() in a mutually recursive
			manner to scan the expressions within ${}. Thus, in the
			recursive call, we must return on encountering a closing
			brace i.e. }

			However, the expression may contain other syntactic
			elements such as functions and objects which also
			contain braces. Thus, we need to keep track of the
			balance between opening and closing braces to correctly
			identify the closing brace of the interpolated string.
		*/
		size_t bracesOpened = 0;

		while (!atEOF()) {
			skipWhitespace();

			if (isDigit(peek())) {
				const auto num = number();
				emitToken(TokenType::NUMBER, num);
				continue;
			}

			if (isAlpha(peek())) {
				const auto id = identifier();
				const auto iterator = KEYWORDS.find(id);
				
				if (iterator != KEYWORDS.end()) {
					emitToken(iterator->second, id);
				} else {
					emitToken(TokenType::IDENTIFIER, id);
				}
				
				continue;
			}

			switch (peek()) {
				SINGLE_CHAR_TOKEN('%', MODULO);
				SINGLE_CHAR_TOKEN('(', OPEN_PAREN);
				SINGLE_CHAR_TOKEN(')', CLOSE_PAREN);
				SINGLE_CHAR_TOKEN('[', OPEN_SQUARE_BRACKET);
				SINGLE_CHAR_TOKEN(']', CLOSE_SQUARE_BRACKET);
				SINGLE_CHAR_TOKEN(':', COLON);
				SINGLE_CHAR_TOKEN(';', SEMICOLON);
				SINGLE_CHAR_TOKEN('.', DOT);
				SINGLE_CHAR_TOKEN(',', COMMA);

				case '{':
					bracesOpened++;
					emitToken(TokenType::OPEN_BRACE);
					break;
				
				case '}': {
					/*
						If we're inside an interpolated string,
						we are in run() to parse the expression
						inside ${}. Thus, a } would indicate the end
						of such an expression. We don't need to emit
						the token for } but simply consume it and
						return back to interpolatedString() for 
						scanning the rest of the string.
					*/
					emitToken(TokenType::CLOSE_BRACE);
					if (interpolationDepth > 0 && bracesOpened == 0) {
						advance();
						return;
					} else {
						bracesOpened--;
					}

					break;
				}

				case '+': {
					switch (peekNext()) {
						case '+':
							emitToken(TokenType::PLUS_PLUS, 2);
							advance();
							break;
						case '=':
							emitToken(TokenType::PLUS_ASSIGNMENT, 2);
							advance();
							break;
						default:
							emitToken(TokenType::PLUS);
					}

					break;
				}

				case '-': {
					switch (peekNext()) {
						case '-':
							emitToken(TokenType::MINUS_MINUS, 2);
							advance();
							break;
						case '=':
							emitToken(TokenType::MINUS_ASSIGNMENT, 2);
							advance();
							break;
						default:
							emitToken(TokenType::MINUS);
					}

					break;
				}

				case '=': {
					switch (peekNext()) {
						case '=':
							emitToken(TokenType::EQU, 2);
							advance();
							break;
						case '>':
							emitToken(TokenType::ARROW, 2);
							advance();
							break;
						default:
							emitToken(TokenType::ASSIGNMENT);
					}

					break;
				}

				case '*': {
					switch (peekNext()) {
						case '=':
							emitToken(TokenType::MULTIPLY_ASSIGNMENT, 2);
							advance();
							break;
						case '*':
							emitToken(TokenType::EXPONENT, 2);
							advance();
							break;
						default:
							emitToken(TokenType::MULTIPLY);
					}

					break;
				}

				DOUBLE_CHAR_TOKEN('>', GRT, '=', GRE);
				DOUBLE_CHAR_TOKEN('<', LST, '=', LSE);
				DOUBLE_CHAR_TOKEN('!', NEGATION, '=', NEQ);

				case '/': {
					switch (peekNext()) {
						case '=':
							emitToken(TokenType::DIVIDE_ASSIGNMENT, 2);
							advance();
							break;
						case '/':
							// Single line comment
							while (!atEOF() && peek() != '\n') {
								advance();
							}
							
							if (peek() == '\n') {
								line++;
								column = 1;
							}
							break;
						case '*':
							// Multi line comment
							while (!atEOF()) {
								advance();
								column++;

								if (peek() == '\n') {
									line++;
									column = 0;
									continue;
								}

								if (peek() == '*' && peekNext() == '/') {
									break;
								}
							}

							if (peek() == '*' && peekNext() == '/') {
								curr += 2;
								column += 2;
								continue;
							} else {
								if (atEOF()) {
									line--;
								}

								error("Unterminated multi-line comment");
							}
							break;
						default:
							emitToken(TokenType::DIVIDE);
					}

					break;
				}
				
				case '&': {
					if (peekNext() == '&') {
						emitToken(TokenType::AND, 2);
						advance();
					} else {
						error("Invalid or unexpected token: '&'");
					}

					break;
				}

				case '|': {
					if (peekNext() == '|') {
						emitToken(TokenType::OR, 2);
						advance();
					} else {
						error("Invalid or unexpected token: '|'");
					}

					break;
				}

				case '"':
				case '\'':
					emitToken(TokenType::STRING, string());
					
					// adjust for the opening and closing quotes
					column += 2;
					break;

				// Back tick
				case '`':
					stringTemplate();
					break;

				case EOF:
					break;

				default:
					// TODO: This is just disgustingly bad and inefficient
					error("Invalid or unexpected token: '"+ std::string(1, peek()) + "'");
					column++;
			}

			advance();
		}

		emitToken(TokenType::EOF_TYPE, 0);
	}

	void Tokenizer::skipWhitespace() {
		for (;;) {
			switch (peek()) {
				case ' ':
				case '\t':
				case '\r':
					advance();
					column++;
					break;
				case '\n':
					advance();
					line++;
					column = 1;
					break;
				default:
					return;
			}
		}
	}

	char Tokenizer::peek() const {
		if (atEOF()) {
			return EOF;
		}

		return translationUnit.contents.get()->at(curr);
	}

	char Tokenizer::peekNext() const {
		if (curr + 1 >= translationUnit.contents->size()) {
			return '\0';
		}

		return translationUnit.contents->at(curr + 1);
	}

	bool Tokenizer::matchNext(char expected) {
		if (atEOF()) return false;
		if (peekNext() != expected) return false;

		curr++;
		return true;
	}

	void Tokenizer::advance() {
		curr++;
	}

	bool Tokenizer::atEOF() const {
		return curr >= translationUnit.contents->size();
	}

	inline bool Tokenizer::isDigit(char c) {
		return c >= '0' && c <= '9';
	}

	std::string Tokenizer::number() {
		size_t len = 0;
		size_t start = curr;

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

		return translationUnit.contents->substr(start, len);
	}

	inline bool Tokenizer::isAlpha(char c) {
		return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_' || c == '$';
	}

	std::string Tokenizer::identifier() {
		size_t len = 0;

		// consume the current character,
		// which has already been verified to be
		// alpha by the isAlpha() call in run()
		size_t start = curr;
		advance(); len++;

		while (isAlpha(peek()) || isDigit(peek())) {
			advance(); len++;
		}

		return translationUnit.contents->substr(start, len);
	}

	std::string Tokenizer::string() {
		size_t len = 0;

		// string literal may use single or double
		// quotation marks, hence we store which one
		// was used in order to find it's pair
		const char openingQuote = peek();

		// consume the opening quotation mark
		advance();

		size_t start = curr;

		while (!atEOF() && peek() != openingQuote && peek() != '\n') {
			advance(); len++;
		}

		if (atEOF() || peek() == '\n') {
			error("Unterminated string literal");
		}

		return translationUnit.contents->substr(start, len);
	}

	void Tokenizer::stringTemplate() {
		emitToken(TokenType::BACK_TICK);
		advance();	// consume `

		size_t len = 0;
		size_t start = curr;

		while (!atEOF()) {
			switch (peek()) {
				case '$': {
					if (peekNext() == '{') {
						auto const &stringLiteral = translationUnit.contents->substr(start, len);

						// We don't use emitToken() for emitting the string literal since it could
						// be multi-line. emitToken() automatically increments column based on
						// the length of the lexeme to be emitted. This, obviously poses a problem when
						// emitting a multi-line string. Hence, we do it manually.
						tokens.push_back(Token(stringLiteral, TokenType::STRING, line, column - stringLiteral.length()));
						emitToken(TokenType::PLUS, 0);

						emitToken(TokenType::INTERPOLATION_START, 2);
						advance(); advance();	// consume ${
				
						interpolationDepth++;
						run();	// scan expression
						interpolationDepth--;
						emitToken(TokenType::PLUS, 0);

						len = 0;
						start = curr;
					}
					break;
				}
				case '`': {
					auto const &stringLiteral = translationUnit.contents->substr(start, len);
					// Not using emitToken for the same reason as above
					tokens.push_back(Token(stringLiteral, TokenType::STRING, line, column - stringLiteral.length()));
					emitToken(TokenType::BACK_TICK);
					return;
				}

				case '\n':
					advance();
					line++;
					column = 1;
					len++;
					break;

				default:
					advance();
					column++;
					len++;
			}
		}
	}

	void Tokenizer::error(const std::string& msg) {
		hadError = true;

		std::cout << ANSICodes::RED << ANSICodes::BOLD << "error: " << ANSICodes::RESET;
		std::cout << ANSICodes::BOLD << translationUnit.filepath << ANSICodes::RESET << " ";
		std::cout << "(line " << line << "): ";
		std::cout << msg << std::endl;

		std::string culpritLine = translationUnit.getLine(line);
		std::cout << "\t" << culpritLine << std::endl;
		std::cout << "\t" << TranslationUnit::getOffsetString(culpritLine, column - 1);
		std::cout << ANSICodes::RED << ANSICodes::BOLD << "↑" << ANSICodes::RESET << std::endl;
	}

} // namespace cu
