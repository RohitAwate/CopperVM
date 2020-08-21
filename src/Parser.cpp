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

	// TODO: parse always returns true
	// return actual return value from declaration
	bool Parser::parse() {
		while (!atEOF()) {
			if (!declaration()) {
				synchronize();
			}
		}

		return true;
	}

	Bytecode Parser::getBytecode() const {
		return m_bytecode;
	}

	const Token& Parser::previous() const {
		if (m_curr > 0) return m_tokens[m_curr - 1];
	}

	const Token& Parser::peek() const {
		return m_tokens[m_curr];
	}

	const Token& Parser::next() {
		if (atEOF()) {
			return peek();
		}

		m_curr++;
		return previous();
	}

	void Parser::consume() {
		next();
	}

	bool Parser::match(TokenType expectedType) {
		if (peek().getType() == expectedType) {
			consume();
			return true;
		}

		return false;
	}

	bool Parser::atEOF() const {
		return peek().getType() == TokenType::EOF_TYPE;
	}

	void Parser::synchronize() {
		while (!atEOF()) {
			switch (peek().getType()) {
				case TokenType::CLASS:
				case TokenType::FUNCTION:
				case TokenType::LET:
				case TokenType::CONST:
				case TokenType::FOR:
				case TokenType::IF:
				case TokenType::WHILE:
				case TokenType::DO:
				case TokenType::TRY:
					return;
				case TokenType::SEMICOLON:
					consume();
					return;
				default:
					consume();
			}
		}
	}

	bool Parser::declaration() {
		if (match(TokenType::LET)) {
			return declarationList(false);
		} else if (match(TokenType::CONST)) {
			return declarationList(true);
		} else if (match(TokenType::PRINT)) {
			return printStatement();
		}

		return statement();
	}

	bool Parser::declarationList(const bool& isConst) {
		while (peek().getType() == TokenType::IDENTIFIER) {
			if (!singleDeclaration(isConst)) return false;

			if (!match(TokenType::COMMA)) break; 
		}

		if (!match(TokenType::SEMICOLON)) {
			error("Expect ';' after declaration");
			return false;
		}

		return true;
	}


	bool Parser::singleDeclaration(const bool& isConst) {
		const auto& identifierToken = peek();
		consume();

		if (match(TokenType::ASSIGNMENT)) {
			if (!expression()) return false;
		} else {
			if (isConst) {
				error("Missing initializer in const declaration");
				return false;
			}
			
			auto const &constOffset = m_bytecode.addConstant(new EmptyObject(ObjectType::UNDEFINED));
			m_bytecode.emit(OpCode::LDC, constOffset, peek().getLine(), peek().getColumn());
		}

		m_bytecode.addIdentifier(identifierToken.getLexeme(), isConst, identifierToken.getLine(), identifierToken.getColumn());

		return true;
	}

	bool Parser::statement() {
		return expressionStatement();
	}

	bool Parser::printStatement() {
		const auto& printToken = previous();

		if(!expression()) return false;
		m_bytecode.emit(OpCode::PRINT, printToken.getLine(), printToken.getColumn());
		
		if (!match(TokenType::SEMICOLON)) {
			error("Expect ';' after statement");
			return false;
		}

		return true;
	}

	bool Parser::expressionStatement() {
		if (!expression()) return false;

		if (match(TokenType::SEMICOLON)) {
			m_bytecode.emit(OpCode::RET, previous().getLine(), previous().getColumn());
			return true;
		}

		error("Expect ';' after expression");
		return false;
	}


	bool Parser::expression() {
		return logicalOR();
	}

	bool Parser::logicalOR() {
		if (!logicalAND()) return false;

		while (match(TokenType::OR)) {
			const auto& orToken = previous();
			if (!logicalAND()) return false;

			m_bytecode.emit(OpCode::OR, orToken.getLine(), orToken.getColumn());
		}

		return true;
	}

	bool Parser::logicalAND() {
		if (!equality()) return false;

		while (match(TokenType::AND)) {
			const auto &andToken = previous();
			if (!equality()) return false;

			m_bytecode.emit(OpCode::AND, andToken.getLine(), andToken.getColumn());
		}

		return true;
	}

	bool Parser::equality() {
		if (!comparison()) return false;

		while (peek().getType() == TokenType::EQU ||
			   peek().getType() == TokenType::NEQ) {
			auto const operatorToken = next();

			if (!comparison()) return false;

			switch (operatorToken.getType()) {
				case TokenType::EQU:
					m_bytecode.emit(OpCode::EQU, operatorToken.getLine(), operatorToken.getColumn());
					break;
				case TokenType::NEQ:
					m_bytecode.emit(OpCode::NEQ, operatorToken.getLine(), operatorToken.getColumn());
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
					m_bytecode.emit(OpCode::GRT, operatorToken.getLine(), operatorToken.getColumn());
					break;
				case TokenType::LST:
					m_bytecode.emit(OpCode::LST, operatorToken.getLine(), operatorToken.getColumn());
					break;
				case TokenType::GRE:
					m_bytecode.emit(OpCode::GRE, operatorToken.getLine(), operatorToken.getColumn());
					break;
				case TokenType::LSE:
					m_bytecode.emit(OpCode::LSE, operatorToken.getLine(), operatorToken.getColumn());
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
					m_bytecode.emit(OpCode::ADD, operatorToken.getLine(), operatorToken.getColumn());
					break;
				case TokenType::MINUS:
					m_bytecode.emit(OpCode::SUB, operatorToken.getLine(), operatorToken.getColumn());
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
					m_bytecode.emit(OpCode::MUL, operatorToken.getLine(), operatorToken.getColumn());
					break;
				case TokenType::DIVIDE:
					m_bytecode.emit(OpCode::DIV, operatorToken.getLine(), operatorToken.getColumn());
					break;
				case TokenType::MODULO:
					m_bytecode.emit(OpCode::MOD, operatorToken.getLine(), operatorToken.getColumn());
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

		if (match(TokenType::EXPONENT)) {
			const auto& operatorToken = previous();

			// Process RHS of expression
			if (!exponent()) return false;

			m_bytecode.emit(OpCode::EXP, operatorToken.getLine(), operatorToken.getColumn());
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
					m_bytecode.emit(OpCode::NEG, operatorToken.getLine(), operatorToken.getColumn());
					break;
				case TokenType::NEGATION:
					m_bytecode.emit(OpCode::NOT, operatorToken.getLine(), operatorToken.getColumn());
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
			case TokenType::NUMBER: {
				auto const &constOffset = m_bytecode.addConstant(new NumberObject(primaryToken.getLexeme()));
				m_bytecode.emit(OpCode::LDC, constOffset, primaryToken.getLine(), primaryToken.getColumn());
				next();
				break;
			}
			case TokenType::TRUE:
			case TokenType::FALSE: {
				auto const &constOffset = m_bytecode.addConstant(new BooleanObject(primaryToken.getLexeme()));
				m_bytecode.emit(OpCode::LDC, constOffset, primaryToken.getLine(), primaryToken.getColumn());
				next();
				break;
			}
			case TokenType::STRING: {
				auto const &constOffset = m_bytecode.addConstant(new StringObject(primaryToken.getLexeme()));
				m_bytecode.emit(OpCode::LDC, constOffset, primaryToken.getLine(), primaryToken.getColumn());
				next();
				break;
			}
			case TokenType::INTERPOLATION_START: {
				consume();
				if (!expression()) return false;
				if (!match(TokenType::CLOSE_BRACE)) {
					error("Expect '}' after template expression");
					return false;
				}
				break;
			}
			// String Template Literal aka String Interpolation
			case TokenType::BACK_TICK:
				return stringTemplate();
			case TokenType::IDENTIFIER: {
				auto const &constOffset = m_bytecode.addConstant(new StringObject(primaryToken.getLexeme()));
				consume();

				if (match(TokenType::ASSIGNMENT)) {
					if (!expression()) return false;
					m_bytecode.emit(OpCode::SETGL, constOffset, primaryToken.getLine(), primaryToken.getColumn());
				} else {
					m_bytecode.emit(OpCode::LDGL, constOffset, primaryToken.getLine(), primaryToken.getColumn());
				}

				break;
			}
			case TokenType::NULL_TYPE: {
				auto const &constOffset = m_bytecode.addConstant(new EmptyObject(ObjectType::NULL_TYPE));
				m_bytecode.emit(OpCode::LDC, constOffset, primaryToken.getLine(), primaryToken.getColumn());
				next();
				break;
			}
			case TokenType::UNDEFINED: {
				auto const &constOffset = m_bytecode.addConstant(new EmptyObject(ObjectType::UNDEFINED));
				m_bytecode.emit(OpCode::LDC, constOffset, primaryToken.getLine(), primaryToken.getColumn());
				next();
				break;
			}
			case TokenType::EOF_TYPE:
				error("Unexpected end-of-file, expect expression");
				return false;
			default:
				error("Expect expression");
				return false;
		}

		return true;
	}

	bool Parser::grouping() {
		// We have already checked for the opening parenthesis '(',
		// so directly consume it here.
		consume();

		if (!expression()) return false;

		if (match(TokenType::CLOSE_PAREN)) return true;

		if (atEOF())
			error("Unexpected end-of-file, expect ')'");
		else
			error("Expect ')'");
		
		return false;
	}

	bool Parser::stringTemplate() {
		consume();	// the back tick `

		while (peek().getType() != TokenType::BACK_TICK) {
			switch (peek().getType()) {
				case TokenType::STRING: {
					if (!expression()) return false;
					break;
				}
				case TokenType::EOF_TYPE:
					error("Unexpected end-of-file, unterminated string template literal");
					return false;
				default:
					error("Unexpected token");
					return false;
			}
		}
		
		consume();	// the back tick `
		return true;
	}

	void Parser::error(const std::string& msg) const {
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
