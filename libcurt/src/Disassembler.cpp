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

	void Disassembler::disassemble(const Bytecode &bytecode, const TranslationUnit &translationUnit) {

#define GET_CONST(index) bytecode.constants[bytecode.blob[index]]

		// Printing the source of the bytecode
		std::cout << ANSICodes::BOLD << ANSICodes::WHITE;
		std::cout << translationUnit.filepath << ANSICodes::RESET << std::endl;

		for (ip = 0; ip < bytecode.blob.size(); ip++) {
			switch (bytecode.blob[ip]) {
				case LDC: {
					Object* val = GET_CONST(++ip).get();
					printInstruction("LDC", std::to_string((int) bytecode.blob[ip]), val->toString());
					break;
				}

				case POP: printInstruction("POP"); break;

				case POPN: {
					printInstruction("POPN", std::to_string((int) bytecode.blob[++ip]));
					break;
				}

				case LDVAR: {
					printInstruction("LDVAR", std::to_string((int)bytecode.blob[++ip]));
					break;
				}

				case SETVAR: {
					printInstruction("SETVAR", std::to_string((int)bytecode.blob[++ip]));
					break;
				}

				case ARRNEW: {
					printInstruction("ARRNEW", std::to_string((int)bytecode.blob[++ip]));
					break;
				}

				case LDPROP: printInstruction("LDPROP"); break;

				case JMP: {
					printInstruction("JMP", std::to_string((int) bytecode.blob[++ip]));
					break;
				}

				case JNT: {
					printInstruction("JNT", std::to_string((int) bytecode.blob[++ip]));
					break;
				}

				// Arithmetic
				case ADD: printInstruction("ADD"); break;
				case SUB: printInstruction("SUB"); break;
				case MUL: printInstruction("MUL"); break;
				case DIV: printInstruction("DIV"); break;
				case MOD: printInstruction("MOD"); break;
				case EXP: printInstruction("EXP"); break;
				case NEG: printInstruction("NEG"); break;

				case INCR: printInstruction("INCR"); break;
				case DECR: printInstruction("DECR"); break;

				// Comparison
				case GRT: printInstruction("GRT"); break;
				case LST: printInstruction("LST"); break;
				case GRE: printInstruction("GRE"); break;
				case LSE: printInstruction("LSE"); break;
				case EQU: printInstruction("EQU"); break;
				case NEQ: printInstruction("NEQ"); break;

				// Logical
				case AND: printInstruction("AND"); break;
				case OR: printInstruction("OR"); break;
				case NOT: printInstruction("NOT"); break;

				case PRINT: printInstruction("PRINT"); break;
				case RET: printInstruction("RET"); break;
			}
		}

#undef GET_CONST
	}

	void Disassembler::printInstruction(const std::string& opcode,
			const std::string& operands, const std::string& comment) {
		printf("%5zu ", ip - 1);
		std::cout << ANSICodes::BOLD << ANSICodes::GREEN;
		printf("%-10s", opcode.c_str());
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

} // namespace Copper