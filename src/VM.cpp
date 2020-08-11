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

#include <iostream>
#include <memory>

#include "Colors.h"
#include "Object.h"
#include "VM.h"

namespace Copper {

    void VM::error(const std::string& msg) const {
        std::cout << ANSICodes::RED << msg << ANSICodes::RESET << std::endl;
    }

    int VM::run(const Bytecode& bytecode) {

#define BINARY_OP(op, ResultObjectType)                                      \
    do                                                                       \
    {                                                                        \
        auto rightVal = m_stack.top();                                       \
        if (rightVal->type != ObjectType::NUMBER)                            \
        {                                                                    \
            error("Operand must be a number.");                              \
            return 1;                                                        \
        }                                                                    \
        m_stack.pop();                                                       \
                                                                             \
        auto leftVal = m_stack.top();                                        \
        if (leftVal->type != ObjectType::NUMBER)                             \
        {                                                                    \
            error("Operand must be a number.");                              \
            return 1;                                                        \
        }                                                                    \
        m_stack.pop();                                                       \
                                                                             \
        auto left = std::dynamic_pointer_cast<NumberObject>(leftVal);        \
        auto right = std::dynamic_pointer_cast<NumberObject>(rightVal);      \
        m_stack.push(std::make_shared<ResultObjectType>((*left)op(*right))); \
    } while (false)

#define EQUALITY_OP(op)                                                       \
    do                                                                        \
    {                                                                         \
        auto rightVal = m_stack.top();                                        \
        m_stack.pop();                                                        \
        auto leftVal = m_stack.top();                                         \
        m_stack.pop();                                                        \
                                                                              \
        if (leftVal->type != rightVal->type)                                  \
            m_stack.push(std::make_shared<BooleanObject>(false, false));      \
        else if (leftVal->type == ObjectType::BOOLEAN)                        \
        {                                                                     \
            auto left = std::dynamic_pointer_cast<BooleanObject>(leftVal);    \
            auto right = std::dynamic_pointer_cast<BooleanObject>(rightVal);  \
            m_stack.push(std::make_shared<BooleanObject>((*left)op(*right))); \
        }                                                                     \
        else if (leftVal->type == ObjectType::NUMBER)                         \
        {                                                                     \
            auto left = std::dynamic_pointer_cast<NumberObject>(leftVal);     \
            auto right = std::dynamic_pointer_cast<NumberObject>(rightVal);   \
            m_stack.push(std::make_shared<BooleanObject>((*left)op(*right))); \
        }                                                                     \
    } while (false)

#define BINARY_LOGICAL_OP(op)                                             \
    do                                                                    \
    {                                                                     \
        auto rightVal = m_stack.top();                                    \
        if (rightVal->type != ObjectType::BOOLEAN)                        \
        {                                                                 \
            error("Operand must be a boolean.");                          \
            return 1;                                                     \
        }                                                                 \
        m_stack.pop();                                                    \
                                                                          \
        auto leftVal = m_stack.top();                                     \
        if (leftVal->type != ObjectType::BOOLEAN)                         \
        {                                                                 \
            error("Operand must be a boolean.");                          \
            return 1;                                                     \
        }                                                                 \
        m_stack.pop();                                                    \
                                                                          \
        auto left = std::dynamic_pointer_cast<BooleanObject>(leftVal);    \
        auto right = std::dynamic_pointer_cast<BooleanObject>(rightVal);  \
        m_stack.push(std::make_shared<BooleanObject>((*left)op(*right))); \
    } while (false)

/*
    The constant is stored in the constant pool of the bytecode.
    We need to fetch the constant at the index indicated by
    the operand of LDC. Thus, we increment the instruction pointer
    to fetch Object at the constant offset in the pool.
*/
#define GET_CONST() (bytecode.m_constants[code[++m_ip]])

#define GET_STRING() std::dynamic_pointer_cast<StringObject>(GET_CONST())

        auto const& code = bytecode.m_blob;

        for (m_ip = 0; m_ip < code.size(); m_ip++) {
            switch (code[m_ip]) {
                case LDC:
                    m_stack.push(GET_CONST());
                    break;

                case DEFGL: {
                    auto identifier = GET_STRING();
                    m_globals[identifier->get()] = m_stack.top();
                    m_stack.pop();
                    break;
                }

                case LDGL: {
                    auto identifier = GET_STRING();
                    if (m_globals.find(identifier->get()) == m_globals.end()) {
                        error("Undefined variable: " + identifier->get());
                        return 1;
                    }

                    m_stack.push(m_globals[identifier->get()]);
                    break;
                }
                
                // Basic arithmetic
                case NEG: {
                    auto obj = m_stack.top();
                    if (obj->type != ObjectType::NUMBER) {
                        error("Operand must be a number.");
                        return 1;
                    }

                    m_stack.pop();

                    auto numObj = std::dynamic_pointer_cast<NumberObject>(obj);
                    m_stack.push(std::make_shared<NumberObject>(-*numObj));
                    break;
                }
                case ADD: {
                    auto rightVal = m_stack.top();
                    m_stack.pop();
                    auto leftVal = m_stack.top();
                    m_stack.pop();

                    if (leftVal->type == ObjectType::STRING || rightVal->type == ObjectType::STRING) {
                        auto concat = leftVal->toString() + rightVal->toString();
                        m_stack.push(std::make_shared<StringObject>(concat));
                    }
                    else if (leftVal->type == ObjectType::NUMBER && rightVal->type == ObjectType::NUMBER) {
                        auto left = std::dynamic_pointer_cast<NumberObject>(leftVal);
                        auto right = std::dynamic_pointer_cast<NumberObject>(rightVal);
                        m_stack.push(std::make_shared<NumberObject>((*left) + (*right)));
                    }
                    else {
                        error("Invalid operand types for operator +");
                        return 1;
                    }

                    break;
                }
                case SUB: BINARY_OP(-, NumberObject); break;
                case MUL: BINARY_OP(*, NumberObject); break;
                case DIV: BINARY_OP(/, NumberObject); break;
                case MOD: BINARY_OP(%, NumberObject); break;
                case EXP: {
                    auto rightVal = m_stack.top();
                    if (rightVal->type != ObjectType::NUMBER) {
                        error("Operand must be a number.");
                        return 1;
                    }
                    m_stack.pop();

                    auto leftVal = m_stack.top();
                    if (leftVal->type != ObjectType::NUMBER) {
                        error("Operand must be a number.");
                        return 1;
                    }
                    m_stack.pop();

                    auto left = std::dynamic_pointer_cast<NumberObject>(leftVal);
                    auto right = std::dynamic_pointer_cast<NumberObject>(rightVal);
                    m_stack.push(std::make_shared<NumberObject>(left->toPower(*right)));  
                    break;
                }

                // Arithmetic comparison
                case GRT: BINARY_OP(>, BooleanObject); break;
                case LST: BINARY_OP(<, BooleanObject); break;
                case GRE: BINARY_OP(>=, BooleanObject); break;
                case LSE: BINARY_OP(<=, BooleanObject); break;

                // Equality comparison
                case EQU: EQUALITY_OP(==); break;
                case NEQ: EQUALITY_OP(!=); break;

                // Logical
                case AND: BINARY_LOGICAL_OP(&&); break;
                case OR:  BINARY_LOGICAL_OP(||); break;
                case NOT: {
                    auto obj = m_stack.top();
                    if (obj->type != ObjectType::BOOLEAN) {
                        error("Operand must be a boolean.");
                        return 1;
                    }

                    m_stack.pop();
                    auto numObj = std::dynamic_pointer_cast<BooleanObject>(obj);
                    m_stack.push(std::make_shared<BooleanObject>(!*numObj));
                    break;
                }
                
                case RET: {
                    while (!m_stack.empty()) {
                        std::cout << ANSICodes::RED << ANSICodes::BOLD <<
                            m_stack.top()->toString()
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
#undef GET_STRING
        return 0;
    }

} // namespace Copper