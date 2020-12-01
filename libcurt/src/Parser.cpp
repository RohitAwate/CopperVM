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

namespace cu {
	
	bool Parser::parse(TranslationUnit& translationUnit, std::vector<Token>& tokens) {
		this->translationUnit = &translationUnit;
		this->tokens.swap(tokens);

		bool success = true;

		env.beginScope();
		while (!atEOF()) {
			if (!declaration()) {
				success = false;
				synchronize();
			}
		}

		return success;
	}

	void Parser::reset() {
		curr = 0;

		while (!loopStack.empty())
			loopStack.pop();

		bytecode.clear();
		env.clear();
	}

	Bytecode Parser::getBytecode() const {
		return bytecode;
	}

	const Token& Parser::previous() const {
		if (curr > 0) return tokens[curr - 1];
	}

	const Token& Parser::peek() const {
		return tokens[curr];
	}

	const Token& Parser::next() {
		if (atEOF()) {
			return peek();
		}

		curr++;
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

	byte Parser::emitJump(OpCode op) {
		bytecode.emit(op, 0, peek().getLocation());
		return bytecode.size() - 1;
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
				case TokenType::PRINT:
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
		}

		return statement();
	}

	bool Parser::declarationList(const bool& isConst) {
		bool identifierFound = false;

		while (peek().getType() == TokenType::IDENTIFIER) {
			identifierFound = true;

			if (!singleDeclaration(isConst)) return false;
			if (!match(TokenType::COMMA)) break; 
		}

		// This might be a crutch.
		if (!identifierFound) {
			error("Unexpected token");
			return false;
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
			
			auto const &constOffset = bytecode.addConstant(new EmptyObject(ObjectType::UNDEFINED));
			bytecode.emit(OpCode::LDC, constOffset, peek().getLocation());
		}

		if (!env.newVariable(identifierToken.getLexeme(), isConst)) {
			error("Redeclaration of variable: " + identifierToken.getLexeme());
			return false;
		}

		return true;
	}

	bool Parser::statement() {
		if (match(TokenType::OPEN_BRACE)) {
			return block();
		} else if (match(TokenType::PRINT)) {
			return printStatement();
		} else if (match(TokenType::IF)) {
			return ifStatement();
		} else if (match(TokenType::FOR)) {
			return forStatement();
		}  else if (match(TokenType::WHILE)) {
			return whileStatement();
		}
 
		return expressionStatement();
	}

	bool Parser::printStatement() {
		const auto& printToken = previous();

		if(!expression()) return false;
		bytecode.emit(OpCode::PRINT, printToken.getLocation());
		
		if (!match(TokenType::SEMICOLON)) {
			error("Expect ';' after statement");
			return false;
		}

		return true;
	}

	bool Parser::expressionStatement() {
		if (!expression()) return false;

		if (match(TokenType::SEMICOLON)) {
			bytecode.emit(OpCode::POP, previous().getLocation());
			return true;
		}

		error("Expect ';' after expression");
		return false;
	}

	bool Parser::block() {
		env.beginScope();

		while (!atEOF() && peek().getType() != TokenType::CLOSE_BRACE) {
			if (!declaration()) return false;
		}

		if (!match(TokenType::CLOSE_BRACE)) {
			error("Expect '}' after block");
			return false;
		}

		auto popCount = env.closeScope();
		bytecode.emit(OpCode::POPN, popCount, previous().getLocation());
		return true;
	}

	bool Parser::ifStatement() {
		if (!match(TokenType::OPEN_PAREN)) {
			error("Expect '(' before if condition");
			return false;
		}

		auto expressionStartToken = peek();
		if (!expression()) return false;

		bytecode.emit(OpCode::JNT, 0, expressionStartToken.getLocation());
		auto jntOffset = bytecode.size() - 1;
		bytecode.emit(OpCode::POP, expressionStartToken.getLocation());

		if (!match(TokenType::CLOSE_PAREN)) {
			error("Expect ')' after if condition");
			return false;
		}

		if (!statement()) return false;

		bytecode.emit(OpCode::JMP, 0, expressionStartToken.getLocation());
		auto jmpOffset = bytecode.size() - 1;

		auto afterIfBodyOffset = bytecode.size();
		bytecode.patch(jntOffset, afterIfBodyOffset);

		bytecode.emit(OpCode::POP, expressionStartToken.getLocation());
		if (match(TokenType::ELSE)) {
			if (!statement()) return false;
		}

		auto afterElseBodyOffset = bytecode.size();
		bytecode.patch(jmpOffset, afterElseBodyOffset);

		return true;
	}

	bool Parser::forStatement() {
		env.beginScope();
		if (!match(TokenType::OPEN_PAREN)) {
			error("Expect '(' before for initializer");
			return false;
		}

		/*
			Initializer may be:
			- empty
			- let or const declaration
			- assignment
		*/
		if (match(TokenType::SEMICOLON)) {
			// Empty initializer
		} else if (match(TokenType::LET) || match(TokenType::CONST)) {
			if (!declarationList(previous().getType() == TokenType::CONST)) return false;
		} else {
			if (!expressionStatement()) return false;
		}

		byte nextIteration = bytecode.size();
		int toEndOfLoop = -1;

		// Exit condition is optional
		if (!match(TokenType::SEMICOLON)) {
			if (!expression()) return false;

			if (!match(TokenType::SEMICOLON)) {
				error("Expect ';' after for exit condition");
				return false;
			}

			toEndOfLoop = emitJump(OpCode::JNT);
			bytecode.emit(OpCode::POP, peek().getLocation());
		}

		byte toIncrement = nextIteration;
		int toBody = -1;

		// Increment expression is optional
		if (!match(TokenType::CLOSE_PAREN)) {
			toBody = emitJump(OpCode::JMP);
			toIncrement = bytecode.size();

			if (!expression()) return false;

			if (!match(TokenType::CLOSE_PAREN)) {
				error("Expect ')' after for declaration");
				return false;
			}

			bytecode.emit(OpCode::POP, peek().getLocation());
			bytecode.emit(OpCode::JMP, nextIteration, peek().getLocation());
		}

		if (toBody != -1) {
			bytecode.patch(toBody, bytecode.size());
		}

		loopStack.push(LoopJumpOffsets(toIncrement));
		if (!statement()) return false;

		bytecode.emit(OpCode::JMP, toIncrement, peek().getLocation());

		if (toEndOfLoop != -1) {
			bytecode.patch(toEndOfLoop, bytecode.size());
			bytecode.emit(OpCode::POP, peek().getLocation());
		}

		for (const auto& breakPatch : loopStack.top().breakPatches) {
			bytecode.patch(breakPatch, bytecode.size());
		}
		loopStack.pop();

		auto popCount = env.closeScope();
		bytecode.emit(OpCode::POPN, popCount, previous().getLocation());
		return true;
	}

	bool Parser::whileStatement() {
		if (!match(TokenType::OPEN_PAREN)) {
			error("Expect '(' before while condition");
			return false;
		}

		auto nextIteration = bytecode.size();

		auto expressionStartToken = peek();
		if (!expression()) return false;

		byte toEndOfLoop = emitJump(OpCode::JNT);
		bytecode.emit(OpCode::POP, expressionStartToken.getLocation());

		if (!match(TokenType::CLOSE_PAREN)) {
			error("Expect ')' after while condition");
			return false;
		}

		loopStack.push(LoopJumpOffsets(nextIteration));
		if (!statement()) return false;

		bytecode.emit(OpCode::JMP, nextIteration, expressionStartToken.getLocation());
		
		bytecode.patch(toEndOfLoop, bytecode.size());
		bytecode.emit(OpCode::POP, expressionStartToken.getLocation());

		for (const auto& breakPatch : loopStack.top().breakPatches) {
			bytecode.patch(breakPatch, bytecode.size());
		}
		loopStack.pop();

		return true;
	}

	bool Parser::expression() {
		return logicalOR();
	}

	bool Parser::logicalOR() {
		if (!logicalAND()) return false;

		while (match(TokenType::OR)) {
			const auto& orToken = previous();
			if (!logicalAND()) return false;

			bytecode.emit(OpCode::OR, orToken.getLocation());
		}

		return true;
	}

	bool Parser::logicalAND() {
		if (!equality()) return false;

		while (match(TokenType::AND)) {
			const auto &andToken = previous();
			if (!equality()) return false;

			bytecode.emit(OpCode::AND, andToken.getLocation());
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
					bytecode.emit(OpCode::EQU, operatorToken.getLocation());
					break;
				case TokenType::NEQ:
					bytecode.emit(OpCode::NEQ, operatorToken.getLocation());
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
					bytecode.emit(OpCode::GRT, operatorToken.getLocation());
					break;
				case TokenType::LST:
					bytecode.emit(OpCode::LST, operatorToken.getLocation());
					break;
				case TokenType::GRE:
					bytecode.emit(OpCode::GRE, operatorToken.getLocation());
					break;
				case TokenType::LSE:
					bytecode.emit(OpCode::LSE, operatorToken.getLocation());
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
					bytecode.emit(OpCode::ADD, operatorToken.getLocation());
					break;
				case TokenType::MINUS:
					bytecode.emit(OpCode::SUB, operatorToken.getLocation());
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
					bytecode.emit(OpCode::MUL, operatorToken.getLocation());
					break;
				case TokenType::DIVIDE:
					bytecode.emit(OpCode::DIV, operatorToken.getLocation());
					break;
				case TokenType::MODULO:
					bytecode.emit(OpCode::MOD, operatorToken.getLocation());
					break;
				default:
					error("Invalid or unexpected token");
					return false;
			}
		}

		return true;
	}

	bool Parser::exponent() {
		if (!preUnary()) return false;

		if (match(TokenType::EXPONENT)) {
			const auto& operatorToken = previous();

			// Process RHS of expression
			if (!exponent()) return false;

			bytecode.emit(OpCode::EXP, operatorToken.getLocation());
		}

		return true;
	}

	bool Parser::preUnary() {
		switch (peek().getType()) {
			case TokenType::PLUS_PLUS:
			case TokenType::MINUS_MINUS: {
				OpCode op = next().getType() == TokenType::PLUS_PLUS ? OpCode::INCR : OpCode::DECR;

				const auto& identifierToken = next();
				auto stackIndex = env.resolveVariable(identifierToken.getLexeme());

				if (stackIndex == -1) {
					error("Undefined variable: " + identifierToken.getLexeme());
					return false;
				}

				if (env.isVariableConst(stackIndex)) {
					error("Assignment to const variable: " + identifierToken.getLexeme());
					return false;
				}

				bytecode.emit(OpCode::LDVAR, stackIndex, identifierToken.getLocation());
				bytecode.emit(op, identifierToken.getLocation());
				bytecode.emit(OpCode::SETVAR, stackIndex, identifierToken.getLocation());

				return true;
			}

			case TokenType::MINUS:
			case TokenType::NEGATION: {
				const auto& operatorToken = next();
				OpCode op = operatorToken.getType() == TokenType::MINUS ? OpCode::NEG : OpCode::NOT;

				if (!preUnary()) return false;

				bytecode.emit(op, operatorToken.getLocation());
			
				return true;
			}
		}

		return primary();
	}

	bool Parser::primary() {
		const auto& primaryToken = peek();
		switch (primaryToken.getType()) {
			case TokenType::OPEN_PAREN:
				if (!grouping()) return false;
				break;
			case TokenType::OPEN_SQUARE_BRACKET:
				if (!array()) return false;
				break;
			case TokenType::NUMBER: {
				auto const &constOffset = bytecode.addConstant(new NumberObject(primaryToken.getLexeme()));
				bytecode.emit(OpCode::LDC, constOffset, primaryToken.getLocation());
				next();
				break;
			}
			case TokenType::TRUE:
			case TokenType::FALSE: {
				auto const &constOffset = bytecode.addConstant(new BooleanObject(primaryToken.getLexeme()));
				bytecode.emit(OpCode::LDC, constOffset, primaryToken.getLocation());
				next();
				break;
			}
			case TokenType::STRING: {
				auto const &constOffset = bytecode.addConstant(new StringObject(primaryToken.getLexeme()));
				bytecode.emit(OpCode::LDC, constOffset, primaryToken.getLocation());
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
			case TokenType::IDENTIFIER:
				return identifier();
			case TokenType::NULL_TYPE: {
				auto const &constOffset = bytecode.addConstant(new EmptyObject(ObjectType::NULL_TYPE));
				bytecode.emit(OpCode::LDC, constOffset, primaryToken.getLocation());
				consume();
				break;
			}
			case TokenType::UNDEFINED: {
				auto const &constOffset = bytecode.addConstant(new EmptyObject(ObjectType::UNDEFINED));
				bytecode.emit(OpCode::LDC, constOffset, primaryToken.getLocation());
				consume();
				break;
			}
			case TokenType::BREAK:
				if (!insideLoop()) {
					error("Illegal break statement");
					return false;
				}

				loopStack.top().breakPatches.push_back(emitJump(OpCode::JMP));
				consume();
				break;
			case TokenType::CONTINUE:
				if (!insideLoop()) {
					error("Illegal continue statement, no enclosing iteration statement");
					return false;
				}

				bytecode.emit(OpCode::JMP, loopStack.top().continueOffset, peek().getLocation());
				consume();
				break;
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

	bool Parser::array() {
		// We have already checked for the opening sqauare bracker '[',
		// so directly consume it here.
		consume();

		byte arraySize = 0;

		while (!atEOF() && !match(TokenType::CLOSE_SQUARE_BRACKET)) {
			if (!expression()) return false;
			arraySize++;
			if (!match(TokenType::COMMA)) {
				if (!match(TokenType::CLOSE_SQUARE_BRACKET)) {
					error("Expect ',' between array members");
					return false;
				}

				break;
			}
		}

		if (previous().getType() == TokenType::CLOSE_SQUARE_BRACKET) {
			bytecode.emit(OpCode::NEWARR, arraySize, peek().getLocation());
			return true;
		} else if (atEOF())
			error("Unexpected end-of-file, expect ']'");
		else
			error("Expect ']' after array declaration");

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

	bool Parser::identifier() {
		const auto& identifierToken = next();
		auto stackIndex = env.resolveVariable(identifierToken.getLexeme());

		if (stackIndex == -1) {
			error("Undefined variable: " + identifierToken.getLexeme());
			return false;
		}

		// TODO: Also pass the stack index of the identifierToken
		// since it is recomputed in the functions called by
		// both branches.
		if (peek().getType() == TokenType::OPEN_SQUARE_BRACKET) {
			return memberAccess(identifierToken);
		} else {
			return variableReference(identifierToken);
		}
	}

	bool Parser::memberAccess(const Token& identifierToken) {
		auto stackIndex = env.resolveVariable(identifierToken.getLexeme());
		bytecode.emit(OpCode::LDVAR, stackIndex, identifierToken.getLocation());

		while (match(TokenType::OPEN_SQUARE_BRACKET)) {
			if (!expression()) return false;
			
			if (!match(TokenType::CLOSE_SQUARE_BRACKET)) {
				error("Expect ']' after member access");
				return false;
			}

			if (!match(TokenType::ASSIGNMENT)) {
				bytecode.emit(OpCode::LDPROP, previous().getLocation());
			}
		}
		
		if (previous().getType() == TokenType::ASSIGNMENT) {
			if (!expression()) return false;
			bytecode.emit(OpCode::SETPROP, peek().getLocation());
		}

		return true;
	}

	bool Parser::variableReference(const Token& identifierToken) {
		auto stackIndex = env.resolveVariable(identifierToken.getLexeme());
		if (match(TokenType::ASSIGNMENT)) {
			if (env.isVariableConst(stackIndex)) {
				error("Assignment to const variable: " + identifierToken.getLexeme());
				return false;
			}

			if (!expression()) return false;
			bytecode.emit(OpCode::SETVAR, stackIndex, identifierToken.getLocation());
		} else if (match(TokenType::PLUS_PLUS) || match(TokenType::MINUS_MINUS)) {
			return postUnary(identifierToken);
		} else {
			bytecode.emit(OpCode::LDVAR, stackIndex, identifierToken.getLocation());
		}

		return true;
	}

	bool Parser::postUnary(const Token& identifierToken) {
		auto stackIndex = env.resolveVariable(identifierToken.getLexeme());
		if (env.isVariableConst(stackIndex)) {
			error("Assignment to const variable: " + identifierToken.getLexeme());
			return false;
		}

		/*
			Pushing the value twice on the stack because we need to make two states of this variable
			- pre-increment value
			- prepare the new incremented/decremented value that would then be assigned to the original
				via SETVAR and then be popped off
		*/
		bytecode.emit(OpCode::LDVAR, stackIndex, identifierToken.getLocation());
		bytecode.emit(OpCode::LDVAR, stackIndex, identifierToken.getLocation());

		/*
			We don't use the INCR/DECR instructions because they operate on the object at the top of
			the stack. When we push the target value on the stack using LDVAR as above, it is
			a reference to the original value. Thus, any increment/decrement will affect all such
			references, including the pre-increment value which should preserve the original value.

			Thus, we need to produce a new value on the stack and not just increment this copy. Thus,
			we push 1 on the stack, perform ADD/SUB which then pushes a new result value onto the stack,
			without affecting the pre-increment copy.

			We then set this new value to the original using SETVAR and pop it off so that the pre-increment
			value remains at the top of the stack as would be expected as the result of this expression.
		*/
		auto constOffset = bytecode.addConstant(new NumberObject(1, true));
		bytecode.emit(OpCode::LDC, constOffset, previous().getLocation());
		OpCode op = previous().getType() == TokenType::PLUS_PLUS ? OpCode::ADD : OpCode::SUB;
		bytecode.emit(op, previous().getLocation());

		bytecode.emit(OpCode::SETVAR, stackIndex, previous().getLocation());
		bytecode.emit(OpCode::POP, previous().getLocation());

		return true;
	}

	void Parser::error(const std::string& msg) const {
		const Token& currentToken = peek();

		std::cout << ANSICodes::RED << ANSICodes::BOLD << "error: " << ANSICodes::RESET;
		std::cout << ANSICodes::BOLD << translationUnit->filepath << ANSICodes::RESET << " ";
		std::cout << "(line " << currentToken.getLocation().line << "): ";
		std::cout << msg << std::endl;

		std::string culpritLine = translationUnit->getLine(currentToken.getLocation().line);
		std::cout << "\t" << culpritLine << std::endl;
		std::cout << "\t" << TranslationUnit::getOffsetString(culpritLine, currentToken.getLocation().column - 1);
		std::cout << ANSICodes::RED << ANSICodes::BOLD << "↑" << ANSICodes::RESET << std::endl;
	}

} // namespace cu
