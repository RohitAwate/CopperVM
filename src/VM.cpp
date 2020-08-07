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
#include "Value.h"
#include "VM.h"

namespace Copper {

    void VM::error(const char* msg) const {
        std::cout << ANSICodes::RED << msg << ANSICodes::RESET << std::endl;
    }

    int VM::run() {

#define BINARY_OP(op)                                          \
    do                                                         \
    {                                                          \
        const Value rightVal = m_stack.top();                  \
        if (rightVal.type != ValueType::NUMBER)              \
        {                                                      \
            error("Operand must be a number.");                \
            return 1;                                          \
        }                                                      \
        m_stack.pop();                                         \
                                                               \
        const Value leftVal = m_stack.top();                   \
        if (leftVal.type != ValueType::NUMBER)               \
        {                                                      \
            error("Operand must be a number.");                \
            return 1;                                          \
        }                                                      \
        m_stack.pop();                                         \
                                                               \
        m_stack.push(leftVal.as.number op rightVal.as.number); \
    } while (false)

#define BINARY_MATH_H(func)                                     \
    do                                                             \
    {                                                              \
        const Value rightVal = m_stack.top();                      \
        if (rightVal.type != ValueType::NUMBER)                  \
        {                                                          \
            error("Operand must be a number.");                    \
            return 1;                                              \
        }                                                          \
        m_stack.pop();                                             \
                                                                   \
        const Value leftVal = m_stack.top();                       \
        if (leftVal.type != ValueType::NUMBER)                   \
        {                                                          \
            error("Operand must be a number.");                    \
            return 1;                                              \
        }                                                          \
        m_stack.pop();                                             \
                                                                   \
        m_stack.push(func(leftVal.as.number, rightVal.as.number)); \
    } while (false)

#define EQUALITY_OP(op)                                            \
	do                                                             \
	{                                                              \
		const Value rightVal = m_stack.top();                      \
		m_stack.pop();                                             \
		const Value leftVal = m_stack.top();                       \
		m_stack.pop();                                             \
                                                                   \
		if (leftVal.type != rightVal.type)                     \
			m_stack.push(false);                                   \
		else                                                       \
			m_stack.push(leftVal.as.number op rightVal.as.number); \
	} while (false)

#define BINARY_LOGICAL_OP(op)                                    \
    do                                                           \
    {                                                            \
        const Value rightVal = m_stack.top();                    \
        if (rightVal.type != ValueType::BOOLEAN)                 \
        {                                                        \
            error("Operand must be a boolean.");                 \
            return 1;                                            \
        }                                                        \
        m_stack.pop();                                           \
                                                                 \
        const Value leftVal = m_stack.top();                     \
        if (leftVal.type != ValueType::BOOLEAN)                  \
        {                                                        \
            error("Operand must be a boolean.");                 \
            return 1;                                            \
        }                                                        \
        m_stack.pop();                                           \
                                                                 \
        m_stack.push(leftVal.as.boolean op rightVal.as.boolean); \
    } while (false)

/*
    The constant is stored in the constant pool of the bytecode.
    We need to fetch the constant at the index indicated by
    the operand of LDC. The instruction pointer is already
    incremented to point to the index in the bytecode execution handler
    for LDC. Hence, we just read from that index.
*/
#define GET_CONST(index) m_code->m_constants[code[index]]

        auto const& code = m_code.get()->m_blob;

        for (m_ip = 0; m_ip < code.size(); m_ip++) {
            switch (code[m_ip]) {
                case LDC:
                    m_stack.push(GET_CONST(++m_ip));
                    break;
                
                // Basic arithmetic
                case NEG: {
                    const Value val = m_stack.top();
                    if (val.type != ValueType::NUMBER) {
                        error("Operand must be a number.");
                        return 1;
                    }

                    m_stack.pop();
                    m_stack.push(val.as.number * -1);
                    break;
                }
                case ADD: BINARY_OP(+); break;
                case SUB: BINARY_OP(-); break;
                case MUL: BINARY_OP(*); break;
                case DIV: BINARY_OP(/); break;
                case MOD: BINARY_MATH_H(std::fmod); break;
                case EXP: BINARY_MATH_H(std::pow); break;
                
                // Arithmetic comparison
                case GRT: BINARY_OP(>); break;
                case LST: BINARY_OP(<); break;
                case GRE: BINARY_OP(>=); break;
                case LSE: BINARY_OP(<=); break;

                // Equality comparison
                case EQU: EQUALITY_OP(==); break;
                case NEQ: EQUALITY_OP(!=); break;

                // Logical
                case AND: BINARY_LOGICAL_OP(&&); break;
                case OR:  BINARY_LOGICAL_OP(||); break;
                case NOT: {
                    const Value val = m_stack.top();
                    if (val.type != ValueType::BOOLEAN) {
                        error("Operand must be a boolean.");
                        return 1;
                    }

                    m_stack.pop();
                    m_stack.push(!val.as.boolean);
                    break;
                }
                
                case RET: {
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
#undef BINARY_MATH_H
#undef EQUALITY_OP
#undef GET_CONST
        return 0;
    }

}