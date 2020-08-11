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

#include "Object.h"

namespace Copper {

	enum OpCode {
		/**
		 * Load specified constant from constant pool
		 * onto the VM stack.
		 * 
		 * Operand:
		 * (1) - constant offset in bytecode's constant pool
		 */
		LDC,

		/**
		 * Defines a global object.
		 * 
		 * First, the name of the symbol is loaded from the constants
		 * pool.
		 * 
		 * The value of the symbol is already expected to be loaded
		 * by LDC, which we now pop from the stack and add to the
		 * VM's global symbol table.
		 *  
		 * Operand:
		 * (1) - identifier offset in bytecode's constant pool
		 */
		DEFGL,
		
		/**
		 * Load specified global variable from the VM's global table.
		 * 
		 * First, the name of the symbol is loaded from the constants
		 * pool.
		 * 
		 * The value of the symbol is already expected to be defined
		 * in the VM's global table by DEFGL, which we now push onto
		 * the VM's execution stack.
		 * 
		 * Operand:
		 * (1) - StringObject representing the identifier of the global
		 */
		LDGL,

		// Basic arithmetic
		ADD,
		SUB,
		MUL,
		DIV,
		MOD,
		EXP,
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

		RET
	};

	typedef size_t byte;

	class Bytecode {
		friend class Disassembler;
		friend class VM;
	public:
		Bytecode(std::string source) : m_source(source) {}
		void emit(byte);
		size_t addConstant(const Object*);
		void emit(byte, byte);
		void addIdentifier(const std::string&, const bool isMutable);
	private:
		std::string m_source;
		std::vector<byte> m_blob;
		std::vector<std::shared_ptr<Object>> m_constants;
	};

} // namespace Copper
