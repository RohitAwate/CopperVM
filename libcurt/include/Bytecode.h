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
		 * Load Variable
		 * 
		 * DESCRIPTION:
		 * Loads the value of a variable on the stack.
		 * 
		 * PRE-CONDITIONS:
		 * - The value must already be loaded by LDC.
		 * - The stack offset of the value must be known.
		 * 
		 * OPERATION:
		 * - The value of the variable is read from the stack offset.
		 * - The read value is pushed onto the stack.
		 * 
		 * OPERANDS:
		 * (1) - stack offset to the variable
		 */
		LDVAR,

		/**
		 * NAME:
		 * Set Variable
		 * 
		 * DESCRIPTION:
		 * Sets the value of a variable on the stack.
		 * 
		 * PRE-CONDITIONS:
		 * - The new value must already be loaded by LDC on the top of the stack.
		 * - The stack offset of the target variable must be known.
		 * 
		 * OPERATION:
		 * - The value on the top of the stack is assigned to the variable
		 * at the given stack offset.
		 * 
		 * OPERANDS:
		 * (1) - stack offset to the variable
		 */
		SETVAR,

		/**
		 * NAME:
		 * Create New Array
		 * 
		 * DESCRIPTION:
		 * Creates a new array of the specified initial size.
		 * 
		 * PRE-CONDITIONS:
		 * - The specified number of values must be loaded on the stack.
		 * 
		 * OPERATION:
		 * - Pushes the top n elements into the array, with the
		 * bottom-most one going in first.
		 * - Pops the n elements from the stack.
		 * 
		 * OPERANDS:
		 * (1) - initial size of array
		 */
		ARRNEW,

		/**
		 * NAME:
		 * Load Object Property
		 * 
		 * DESCRIPTION:
		 * Loads a certain object property onto the stack.
		 * 
		 * PRE-CONDITIONS:
		 * - The object and the property identifier must be loaded on the stack.
		 * 
		 * OPERATION:
		 * - Pops the property identifier and the object from the stack.
		 * - If object is not an array or a JavaScript Object, undefined is pushed
		 * onto the stack.
		 * - Else, an attempt to load the property from the object is made.
		 * 
		 * OPERANDS:
		 * None. Required operands are read from the stack.
		 */
		LDPROP,

		/**
		 * NAME:
		 * Jump to Offset
		 * 
		 * DESCRIPTION:
		 * Jumps to the offset indicated by the operand.
		 * 
		 * PRE-CONDITIONS:
		 * None.
		 * 
		 * OPERATION:
		 * - IP is set to the bytecode offset indicated by the operand.
		 * 
		 * OPERANDS:
		 * (1) - bytecode offset to jump to
		 */
		JMP,

		/**
		 * NAME:
		 * Jump If Not True
		 * 
		 * DESCRIPTION:
		 * Checks the stack top and if it is truthy, jumps to the offset
		 * indicated by the operand.
		 * 
		 * PRE-CONDITIONS:
		 * - The stack must have at least one value.
		 * 
		 * OPERATION:
		 * - The value at the top of the stack is checked.
		 * - If truthy, IP is set to the bytecode offset indicated by the operand.
		 * - Truthiness for various Object types is evaluated as follows:
		 *    - Boolean: The underlying Boolean value is checked.
		 *    - Number: False if zero, true otherwise.
		 *    - String: False if length is zero, true otherwise.
		 *    - Null and Undefined: Always false.
		 * 
		 * OPERANDS:
		 * (1) - bytecode offset to jump to
		 */
		JNT,

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
		 * INCREMENT & DECREMENT
		 * 
		 * DESCRIPTION:
		 * Increment/Decrement the value of the stack top.
		 * 
		 * PRE-CONDITIONS:
		 * - The stack must have at least one value.
		 * 
		 * OPERATION:
		 * - The object type is checked, error if non-numeric.
		 * - The increment/decrement operation is performed.
		 * 
		 * OPERANDS:
		 * None. Required operands are popped from the stack.
		 */
		INCR,
		DECR,

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
		std::pair<unsigned int, unsigned int> getSourceLocation(byte opcodeIndex) const;
		size_t size() const { return blob.size(); }
		void patch(const size_t offset, const byte b);
	private:
		std::vector<byte> blob;
		std::map<unsigned int, std::vector<unsigned int>> locations;
		std::vector<std::shared_ptr<Object>> constants;

		void addInstructionLocation(const unsigned int &line, const unsigned int &column);

	};

} // namespace Copper
