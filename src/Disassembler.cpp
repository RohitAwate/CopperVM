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

#include <sstream>

#include "Colors.h"
#include "Disassembler.h"

namespace Copper {

	void Disassembler::run() const {

#define GET_CONST(index) m_bytecode.m_constants[m_bytecode.m_blob[index]]

		// Printing the source of the bytecode
		std::cout << ANSICodes::BOLD << ANSICodes::WHITE;
		std::cout << m_bytecode.m_source << ANSICodes::RESET << std::endl;

		for (int i = 0; i < m_bytecode.m_blob.size(); i++) {
			switch (m_bytecode.m_blob[i]) {
				case OpCode::LDC: {
					const Value val = GET_CONST(++i);
					printInstruction("LDC", std::to_string((int) m_bytecode.m_blob[i]), val.toString());
					break;
				}

				// Arithmetic
				case OpCode::ADD: printInstruction("ADD"); break;
				case OpCode::SUB: printInstruction("SUB"); break;
				case OpCode::MUL: printInstruction("MUL"); break;
				case OpCode::DIV: printInstruction("DIV"); break;
				case OpCode::EXP: printInstruction("EXP"); break;
				case OpCode::NEG: printInstruction("NEG"); break;

				// Comparison
				case OpCode::GRT: printInstruction("GRT"); break;
				case OpCode::LST: printInstruction("LST"); break;
				case OpCode::GRE: printInstruction("GRE"); break;
				case OpCode::LSE: printInstruction("LSE"); break;
				case OpCode::EQU: printInstruction("EQU"); break;
				case OpCode::NEQ: printInstruction("NEQ"); break;

				// Logical
				case OpCode::NOT: printInstruction("NOT"); break;

				case OpCode::RET: printInstruction("RET"); break;
			}
		}

#undef GET_CONST
	}

	void Disassembler::printInstruction(const std::string& opcode,
			const std::string& operands, const std::string& comment) {
		std::cout << ANSICodes::BOLD << ANSICodes::GREEN;
		printf("%-5s", opcode.c_str());
		std::cout << ANSICodes::RESET;

		
		if (operands != "") {
			printf(" %-4s", operands.c_str());
		}

		if (comment != "") {
			std::cout << ANSICodes::BLUE;
			printf(" # %-30s", comment.c_str());
			std::cout << ANSICodes::RESET;
		}

		std::cout << std::endl;
	}

}