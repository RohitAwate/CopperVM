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

#include <cmath>
#include <iostream>

#include "Colors.h"
#include "VM.h"

namespace Copper {

	int VM::run() {
#define BINARY_OP(op)                                 \
	do                                                \
	{                                                 \
		const double right = m_stack.top().as.number; \
		m_stack.pop();                                \
		const double left = m_stack.top().as.number;  \
		m_stack.pop();                                \
		m_stack.push(left op right);                  \
		break;                                        \
	} while (false)

#define BINARY_OP_MATH_H(func)                        \
	do                                                \
	{                                                 \
		const double right = m_stack.top().as.number; \
		m_stack.pop();                                \
		const double left = m_stack.top().as.number;  \
		m_stack.pop();                                \
		m_stack.push(func(left, right));              \
		break;                                        \
	} while (false)

#define GET_CONSTANT(index) m_code->m_constants[code[index]]

		auto const& code = m_code.get()->m_blob;

		for (m_ip = 0; m_ip < code.size(); m_ip++) {
			switch (code[m_ip]) {
				case OP_LOAD_CONST:
					m_stack.push(GET_CONSTANT(++m_ip));
					break;
				
				// Basic arithmetic
				case OP_NEG: {
					const double value = m_stack.top().as.number * -1;
					m_stack.pop();
					m_stack.push(value);
					break;
				}
				case OP_ADD: BINARY_OP(+); break;
				case OP_SUB: BINARY_OP(-); break;
				case OP_MUL: BINARY_OP(*); break;
				case OP_DIV: BINARY_OP(/); break;
				case OP_MOD: BINARY_OP_MATH_H(std::fmod); break;
				case OP_EXP: BINARY_OP_MATH_H(std::pow); break;
				
				// Arithmetic comparison
				case OP_GREATER_THAN: 	BINARY_OP(>); break;
				case OP_LESSER_THAN: 	BINARY_OP(<); break;
				case OP_GREATER_EQUAL: 	BINARY_OP(>=); break;
				case OP_LESSER_EQUAL: 	BINARY_OP(<=); break;

				// Equality comparison
				case OP_EQUAL_EQUAL: 	BINARY_OP(==); break;
				case OP_NOT_EQUAL: 		BINARY_OP(!=); break;
				
				case OP_RET: {
					while (!m_stack.empty()) {
						std::cout << ANSICodes::RED << ANSICodes::BOLD <<
							m_stack.top()
						<< ANSICodes::RESET << std::endl;
						m_stack.pop();
					}
				}
			}
		}
#undef BINARY_OP
		return 0;
	}

}