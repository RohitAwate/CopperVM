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

#include <vector>
#include <stack>

#include "Bytecode.h"
#include "Environment.h"
#include "Token.h"
#include "TranslationUnit.h"

namespace Copper {
	
	class Parser {
	public:
		bool parse(TranslationUnit& translationUnit, std::vector<Token>& tokens);
		void reset();
		Bytecode getBytecode() const;
	private:
		TranslationUnit* translationUnit;
		std::vector<Token> tokens;
		size_t curr = 0;

		// TODO: Documentation needed
		struct LoopJumpOffsets {
			byte continueOffset;
			std::vector<byte> breakPatches;

			LoopJumpOffsets(const byte& continueOffset) :
				continueOffset(continueOffset) {}
		};

		std::stack<LoopJumpOffsets> loopStack;

		bool insideLoop() const { return !loopStack.empty(); }

		Bytecode bytecode;
		Environment env;

		const Token& previous() const;
		const Token& peek() const;
		const Token& next();
		void consume();
		bool match(TokenType);
		bool atEOF() const;

		byte emitJump(OpCode);

		void synchronize();

		bool declaration();
		bool declarationList(const bool& isConst);
		bool singleDeclaration(const bool& isConst);
		bool statement();
		bool printStatement();
		bool expressionStatement();
		bool block();
		bool ifStatement();
		bool forStatement();
		bool whileStatement();

		/*
			These Boolean return values form the synchronization
			mechanism in the parser. If an unexpected or invalid token
			is encountered, an error is reported and we unwind the
			call stack by returning false.
		*/
		bool expression();
		bool logicalOR();
		bool logicalAND();
		bool equality();
		bool comparison();
		bool term();
		bool factor();
		bool exponent();
		bool preUnary();
		bool primary();
		bool grouping();
		bool array();
		bool stringTemplate();
		bool identifier();
		bool memberAccess(const Token& identifierToken);
		bool variableReference(const Token& identifierToken);

		void error(const std::string &) const;
	};

} // namespace Copper
