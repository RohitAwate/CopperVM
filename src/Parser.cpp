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

	bool Parser::parse() {
		if (atEOF()) return false;

		auto retval = expression();
		m_bytecode.emit(OP_RET);
		return retval;
	}

	Bytecode Parser::getBytecode() const {
		return m_bytecode;
	}

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

	void Parser::consume() {
		next();
	}

	bool Parser::atEOF() const {
		return peek().getType() == TokenType::EOF_TYPE;
	}

	bool Parser::expression() {
		return equality();
	}

	bool Parser::equality() {
		if (!comparison()) return false;

		while (peek().getType() == TokenType::EQUAL_EQUAL ||
			   peek().getType() == TokenType::NOT_EQUAL) {
			auto const operatorToken = next();

			if (!comparison()) return false;

			switch (operatorToken.getType()) {
				case TokenType::EQUAL_EQUAL:
					m_bytecode.emit(OpCode::OP_EQUAL_EQUAL);
					break;
				case TokenType::NOT_EQUAL:
					m_bytecode.emit(OpCode::OP_NOT_EQUAL);
					break;
				default:
					error("Invalid or unexpected token");
					return false;
			}
		}

		return true;
	}

	bool Parser::comparison() {
		if (!term()) return false;

		while (peek().getType() == TokenType::GREATER_THAN 	||
			   peek().getType() == TokenType::LESSER_THAN 	||
			   peek().getType() == TokenType::GREATER_EQUAL ||
			   peek().getType() == TokenType::LESSER_EQUAL) {
			auto const operatorToken = next();

			if (!term()) return false;

			switch (operatorToken.getType()) {
				case TokenType::GREATER_THAN:
					m_bytecode.emit(OpCode::OP_GREATER_THAN);
					break;
				case TokenType::LESSER_THAN:
					m_bytecode.emit(OpCode::OP_LESSER_THAN);
					break;
				case TokenType::GREATER_EQUAL:
					m_bytecode.emit(OpCode::OP_GREATER_EQUAL);
					break;
				case TokenType::LESSER_EQUAL:
					m_bytecode.emit(OpCode::OP_LESSER_EQUAL);
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

		while (peek().getType() == TokenType::PLUS ||
			   peek().getType() == TokenType::MINUS) {
			auto const operatorToken = next();
			
			if (!factor()) return false;

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

	bool Parser::factor() {
		if (!exponent()) return false;

		while (peek().getType() == TokenType::MULTIPLY 	||
			   peek().getType() == TokenType::DIVIDE 	||
			   peek().getType() == TokenType::MODULO) {
			auto const operatorToken = next();

			if (!exponent()) return false;

			switch (operatorToken.getType()) {
				case TokenType::MULTIPLY:
					m_bytecode.emit(OpCode::OP_MUL);
					break;
				case TokenType::DIVIDE:
					m_bytecode.emit(OpCode::OP_DIV);
					break;
				case TokenType::MODULO:
					m_bytecode.emit(OpCode::OP_MOD);
					break;
				default:
					error("Invalid or unexpected token");
					return false;
			}
		}

		return true;
	}

	bool Parser::exponent() {
		if (!unary()) return false;

		if (peek().getType() == TokenType::EXPONENT) {
			// Consume the ** operator
			consume();
			
			// Process RHS of expression
			if (!exponent()) return false;

			m_bytecode.emit(OpCode::OP_EXP);
		}

		return true;
	}

	bool Parser::unary() {
		if (peek().getType() == TokenType::MINUS) {
			// Consume the - operator
			consume();

			if (!unary()) return false;

			m_bytecode.emit(OpCode::OP_NEG);
			return true;
		}

		return primary();
	}

	bool Parser::primary() {
		const auto& primaryToken = next();
		switch (primaryToken.getType()) {
			case TokenType::OPEN_PAREN:
				if (!grouping()) return false;
				break;
			case TokenType::NUMBER:
				m_bytecode.emitConstant(Value(ValueType::NUMBER, primaryToken.getLexeme()));
				break;
			case TokenType::TRUE:
			case TokenType::FALSE:
				m_bytecode.emitConstant(Value(ValueType::BOOLEAN, primaryToken.getLexeme()));
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
