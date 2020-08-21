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

#include <memory>
#include <vector>
#include <map>

#include "Object.h"

namespace Copper {

	enum OpCode {

		/**
		 * NAME:
		 * Load Constant
		 * 
		 * DESCRIPTION:
		 * Loads a constant value onto the stack.
		 * 
		 * PRE-CONDITIONS:
		 * - The constant must be defined in the bytecode's constant pool.
		 * 
		 * OPERATION:
		 * - The value at the operand index is read.
		 * - The read value is loaded onto the VM's execution stack.
		 *  
		 * OPERANDS:
		 * (1) - offset into the bytecode's constant pool
		 */
		LDC,

		/**
		 * NAME:
		 * Pop Stack
		 * 
		 * DESCRIPTION:
		 * Pops the top value from the stack.
		 *
		 * PRE-CONDITIONS:
		 * None.
		 * 
		 * OPERATION:
		 * - Topmost element of the stack is discarded.
		 * 
		 * OPERANDS:
		 * None.
		 */
		POP,

		/**
		 * NAME:
		 * Multi-Pop Stack
		 * 
		 * DESCRIPTION:
		 * Pops the N-topmost values from the stack.
		 *
		 * PRE-CONDITIONS:
		 * None.
		 * 
		 * OPERATION:
		 * - The N-topmost elements on the stack are discarded.
		 * 
		 * OPERANDS:
		 * (1) - number of elements to pop
		 */
		POPN,

		/**
		 * NAME:
		 * Define Global
		 * 
		 * DESCRIPTION:
		 * Defines a new global variable.
		 * 
		 * PRE-CONDITIONS:
		 * - The identifier string must already be loaded by LDC.
		 * - The variable's value must already be loaded by LDC.
		 * 
		 * OPERATION:
		 * - The identifier string is loaded from the constants pool.
		 * - Checks if identifier has been already defined.
		 * - Adds identifier and corresponding value to the VM's global
		 *   symbol table along with the constness of the variable.
		 * - We now pop the value from the stack.
		 *  
		 * OPERANDS:
		 * (1) - identifier offset into the bytecode's constant pool
		 */
		DEFGL,

		/**
		 * NAME:
		 * Load Global
		 * 
		 * DESCRIPTION:
		 * Loads the value of a global variable on the stack.
		 * 
		 * PRE-CONDITIONS:
		 * - The variable must already be defined.
		 * - The identifier string must already be loaded by LDC.
		 * 
		 * OPERATION:
		 * - The identifier string is loaded from the constants pool.
		 * - Checks if identifier has actually been defined.
		 * - Pushes the value of the variable onto the stack.
		 * - We now pop the value from the stack.
		 *  
		 * OPERANDS:
		 * (1) - identifier offset into the bytecode's constant pool
		 */
		LDGL,

		/**
		 * NAME:
		 * Set Global
		 * 
		 * DESCRIPTION:
		 * Sets the value of a global variable.
		 * 
		 * PRE-CONDITIONS:
		 * - The variable must already be defined.
		 * - The identifier string must already be loaded by LDC.
		 * - The variable's value must already be loaded by LDC.
		 * 
		 * OPERATION:
		 * - The identifier string is loaded from the constants pool.
		 * - Checks if variable has actually been defined.
		 * - Checks if the variable is non-const.
		 * - New value is assigned to the variable.
		 * - We now pop the value from the stack.
		 *  
		 * OPERANDS:
		 * (1) - identifier offset into the bytecode's constant pool
		 */
		SETGL,

		/**
		 * ARITHMETIC ADDITION & STRING CONCATENATION
		 * 
		 * DESCRIPTION:
		 * Performs numeric addition when both operands are numeric,
		 * or a string conversion of both followed by a concatenation
		 * if even one of them is string.
		 * 
		 * PRE-CONDITIONS:
		 * - The two operands must already be loaded by LDC.
		 * 
		 * OPERATION:
		 * - The two operand values are popped from the stack.
		 * - Checked if any of the two are string. In this case,
		 *   a string concatenation is produced as result.
		 * - If both are numeric, addition is performed resulting in a sum.
		 * - Result is pushed onto the stack.
		 * 
		 * OPERANDS:
		 * None. Required operands are popped from the stack.
		 */
		ADD,

		/**
		 * BASIC ARITHMETIC OPERATIONS
		 * Applies to SUB -> EXP
		 * 
		 * DESCRIPTION:
		 * Performs the specified arithmetic operation.
		 * 
		 * PRE-CONDITIONS:
		 * - The two operands must already be loaded by LDC.
		 * 
		 * OPERATION:
		 * - The two operand values are popped from the stack.
		 * - Check for numeric types is performed.
		 * - The arithmetic operation is performed.
		 * - Result is pushed onto the stack.
		 * 
		 * OPERANDS:
		 * None. Required operands are popped from the stack.
		 */
		SUB,
		MUL,
		DIV,
		MOD,
		EXP,

		/**
		 * DESCRIPTION:
		 * Negates a numeric value.
		 * 
		 * PRE-CONDITIONS:
		 * - The operand must already be loaded by LDC.
		 * 
		 * OPERATION:
		 * - The operand value is popped from the stack.
		 * - Check for numeric type is performed.
		 * - Value is negated.
		 * - Result is pushed onto the stack.
		 * 
		 * OPERANDS:
		 * None. Required operand is popped from the stack.
		 */
		NEG,

		// Comparison
		GRT,
		LST,
		GRE,
		LSE,
		EQU,
		NEQ,

		// Logical
		AND,
		OR,
		NOT,

		PRINT,
		RET
	};

	typedef size_t byte;

	class Bytecode {
		friend class Disassembler;
		friend class VM;
	public:
		void emit(byte opcode, unsigned int line, unsigned int column);
		void emit(byte b1, byte b2, unsigned int line, unsigned int column);
		size_t addConstant(const Object *);
		void addIdentifier(const std::string &, const bool isConst, unsigned int line, unsigned int column);
		std::pair<unsigned int, unsigned int> getSourceLocation(byte opcodeIndex) const;
	private:
		std::vector<byte> m_blob;
		std::map<unsigned int, std::vector<unsigned int>> m_locations;
		std::vector<std::shared_ptr<Object>> m_constants;

		void addInstructionLocation(const unsigned int &line, const unsigned int &column);

	};

} // namespace Copper
