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
	
	Bytecode Parser::parse() {
		expression();
		return m_bytecode;
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
					m_bytecode.emit(OpCode::OP_ADD);
					break;
				case TokenType::MINUS:
					m_bytecode.emit(OpCode::OP_SUB);
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
					m_bytecode.emit(OpCode::OP_MUL);
					break;
				case TokenType::DIVIDE:
					m_bytecode.emit(OpCode::OP_DIV);
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
				m_bytecode.emitConstant(std::stoi(factorToken.getLexeme()));
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
		std::cout << msg << std::endl;
	}

} // namespace Copper
