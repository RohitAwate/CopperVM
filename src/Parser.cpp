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

#include "Parser.h"

#define Log(x) std::cout << (x); std::cout.flush();

namespace Copper {

	const Token Parser::peek() const {
		return m_tokens[m_curr];
	}

	const Token Parser::next() {
		if (atEOF()) {
			return peek();
		}

		m_curr++;
		return m_tokens.at(m_curr - 1);
	}
	
	void Parser::parse() {
		expression();
		std::cout << std::endl;
	}

	bool Parser::atEOF() const {
		return m_curr >= m_tokens.size() - 1;
	}

	bool Parser::expression() {
		if (!term()) return false;

		while (peek().getType() == TokenType::PLUS ||
			   peek().getType() == TokenType::MINUS) {
			auto const operatorToken = next();
			
			if (!term()) return false;

			switch (operatorToken.getType()) {
				case TokenType::PLUS:
					Log(" + ");
					break;
				case TokenType::MINUS:
					Log(" - ");
					break;
				default:
					error("Invalid or unexpected token");
					return false;
			}
		}

		return true;
	}

	bool Parser::term() {
		if (!factor()) return false;

		while (peek().getType() == TokenType::MULTIPLY ||
			   peek().getType() == TokenType::DIVIDE) {
			auto const operatorToken = next();

			if (!factor()) return false;

			switch (operatorToken.getType()) {
				case TokenType::MULTIPLY:
					Log(" * ");
					break;
				case TokenType::DIVIDE:
					Log(" / ");
					break;
				default:
					error("Invalid or unexpected token");
					return false;
			}
		}

		return true;
	}

	bool Parser::factor() {
		const auto& factorToken = next();
		switch (factorToken.getType()) {
			case TokenType::OPEN_PAREN:
				grouping();
				break;
			case TokenType::NUMBER:
				Log(" " + factorToken.getLexeme() + " ");
				break;
			case TokenType::IDENTIFIER:
				Log(" " + factorToken.getLexeme() + " ");
				break;
			default:
				error("Invalid or unexpected token");
				return false;
		}

		return true;
	}

	bool Parser::grouping() {
		// We've already checked that OPEN_PAREN exists in Parser::factor(),
		// hence we directly parse the expression

		if (!expression()) return false;

		if (next().getType() != TokenType::CLOSE_PAREN) {
			error("Expect ')'");
			return false;
		}

		return true;
	}

	void Parser::error(const char* msg) const {
		Log(msg);
	}

} // namespace Copper
