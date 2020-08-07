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
#include "Parser.h"

namespace Copper {

	bool Parser::parse() {
		if (atEOF()) return false;

		auto retval = expression();
		m_bytecode.emit(RET);
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

		while (peek().getType() == TokenType::EQU ||
			   peek().getType() == TokenType::NEQ) {
			auto const operatorToken = next();

			if (!comparison()) return false;

			switch (operatorToken.getType()) {
				case TokenType::EQU:
					m_bytecode.emit(OpCode::EQU);
					break;
				case TokenType::NEQ:
					m_bytecode.emit(OpCode::NEQ);
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

		while (peek().getType() == TokenType::GRT 	||
			   peek().getType() == TokenType::LST 	||
			   peek().getType() == TokenType::GRE ||
			   peek().getType() == TokenType::LSE) {
			auto const operatorToken = next();

			if (!term()) return false;

			switch (operatorToken.getType()) {
				case TokenType::GRT:
					m_bytecode.emit(OpCode::GRT);
					break;
				case TokenType::LST:
					m_bytecode.emit(OpCode::LST);
					break;
				case TokenType::GRE:
					m_bytecode.emit(OpCode::GRE);
					break;
				case TokenType::LSE:
					m_bytecode.emit(OpCode::LSE);
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
					m_bytecode.emit(OpCode::ADD);
					break;
				case TokenType::MINUS:
					m_bytecode.emit(OpCode::SUB);
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
					m_bytecode.emit(OpCode::MUL);
					break;
				case TokenType::DIVIDE:
					m_bytecode.emit(OpCode::DIV);
					break;
				case TokenType::MODULO:
					m_bytecode.emit(OpCode::MOD);
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

			m_bytecode.emit(OpCode::EXP);
		}

		return true;
	}

	bool Parser::unary() {
		if (peek().getType() == TokenType::MINUS || peek().getType() == TokenType::NEGATION) {
			// Consume the operator
			const auto& operatorToken = next();

			if (!unary()) return false;

			switch (operatorToken.getType()) {
				case TokenType::MINUS:
					m_bytecode.emit(OpCode::NEG);
					break;
				case TokenType::NEGATION:
					m_bytecode.emit(OpCode::NOT);
					break;
			}
			return true;
		}

		return primary();
	}

	bool Parser::primary() {
		const auto& primaryToken = peek();
		switch (primaryToken.getType()) {
			case TokenType::OPEN_PAREN:
				if (!grouping()) return false;
				break;
			case TokenType::NUMBER:
				m_bytecode.emitConstant(Value(ValueType::NUMBER, primaryToken.getLexeme()));
				next();
				break;
			case TokenType::TRUE:
			case TokenType::FALSE:
				m_bytecode.emitConstant(Value(ValueType::BOOLEAN, primaryToken.getLexeme()));
				next();
				break;
			case TokenType::EOF_TYPE:
				error("Unexpected end-of-file, expect expression");
				return false;
			default:
				error("Invalid or unexpected token");
				return false;
		}

		return true;
	}

	bool Parser::grouping() {
		// We have already checked for the opening parenthesis '(',
		// so directly consume it here.
		consume();

		if (!expression()) return false;

		if (peek().getType() != TokenType::CLOSE_PAREN) {
			if (atEOF()) error("Unexpected end-of-file, expect ')'");
			else error("Expect ')'");
			return false;
		}

		// Consume the closing parenthesis ')'
		consume();
		return true;
	}

	void Parser::error(const char* msg) const {
		const Token& currentToken = peek();

		std::cout << ANSICodes::RED << ANSICodes::BOLD << "error: " << ANSICodes::RESET;
		std::cout << ANSICodes::BOLD << m_translationUnit.m_filepath << ANSICodes::RESET << " ";
		std::cout << "(line " << currentToken.getLine() << "): ";
		std::cout << msg << std::endl;

		std::string culpritLine = m_translationUnit.getLine(currentToken.getLine());
		std::cout << "\t" << culpritLine << std::endl;
		std::cout << "\t" << TranslationUnit::getOffsetString(culpritLine, currentToken.getColumn() - 1);
		std::cout << ANSICodes::RED << ANSICodes::BOLD << "↑" << ANSICodes::RESET << std::endl;
	}

} // namespace Copper
