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
		OP_LOAD_CONST,

		// Basic arithmetic
		OP_ADD,
		OP_SUB,
		OP_MUL,
		OP_DIV,
		OP_MOD,
		OP_EXP,
		OP_NEG,

		// Comparison
		OP_GREATER_THAN,
		OP_LESSER_THAN,
		OP_GREATER_EQUAL,
		OP_LESSER_EQUAL,
		OP_EQUAL_EQUAL,
		OP_NOT_EQUAL,

		OP_RET
	};

	typedef unsigned char byte;

	class Bytecode {
		friend class VM;
	public:
		void emit(byte);
		void emitConstant(const Value);
	private:
		std::vector<byte> m_blob;
		std::vector<Value> m_constants;
	};

} // namespace Copper
