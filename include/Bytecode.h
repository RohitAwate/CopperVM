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

#include "Value.h"

namespace Copper {
	
	enum OpCode {
		/*
			Load specified constant from constant pool
			onto the VM stack.

			Operand:
			(1) - constant offset in bytecode's constant pool
		*/
		LDC,

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

		RET
	};

	typedef unsigned char byte;

	class Bytecode {
		friend class VM;
		friend class Disassembler;
	public:
		Bytecode(std::string source) : m_source(source) {}
		void emit(byte);
		void emitConstant(const Value);
	private:
		std::string m_source;
		std::vector<byte> m_blob;
		std::vector<Value> m_constants;
	};

} // namespace Copper
