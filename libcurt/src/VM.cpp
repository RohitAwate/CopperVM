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

namespace cu {

    static bool isTruthy(const std::shared_ptr<Object>& obj) {
        switch (obj->type) {
            case ObjectType::BOOLEAN:
                return std::dynamic_pointer_cast<BooleanObject>(obj)->get();
            case ObjectType::NUMBER:
                return std::dynamic_pointer_cast<NumberObject>(obj)->get() != 0;
            case ObjectType::STRING:
                return std::dynamic_pointer_cast<StringObject>(obj)->get().length() != 0;
            case ObjectType::NULL_TYPE:
            case ObjectType::UNDEFINED:
                return false;
        }
    }

    void VM::error(const TranslationUnit& translationUnit, const Bytecode& bytecode, const std::string& msg) const {
        std::cout << ANSICodes::RED << ANSICodes::BOLD << "error: " << ANSICodes::RESET;
        std::cout << ANSICodes::BOLD << translationUnit.filepath << ANSICodes::RESET << " ";
        
        const auto& location = bytecode.getSourceLocation(ip);
        std::cout << "(line " << location.line << "): ";
        std::cout << msg << std::endl;

        const auto& culpritLine = translationUnit.getLine(location.line);
        std::cout << "\t" << culpritLine << std::endl;
        std::cout << "\t" << TranslationUnit::getOffsetString(culpritLine, location.column - 1);
        std::cout << ANSICodes::RED << ANSICodes::BOLD << "↑" << ANSICodes::RESET << std::endl;
    }

    int VM::run(const Bytecode& bytecode, const TranslationUnit& translationUnit) {

#define BINARY_OP(op, ResultObjectType)                                              \
    do                                                                               \
    {                                                                                \
        auto rightVal = stack.top();                                                 \
        if (rightVal->type != ObjectType::NUMBER)                                    \
        {                                                                            \
            error(translationUnit, bytecode, "Operand must be a number.");           \
            return 1;                                                                \
        }                                                                            \
        stack.pop();                                                                 \
                                                                                     \
        auto leftVal = stack.top();                                                  \
        if (leftVal->type != ObjectType::NUMBER)                                     \
        {                                                                            \
            error(translationUnit, bytecode, "Operand must be a number.");           \
            return 1;                                                                \
        }                                                                            \
        stack.pop();                                                                 \
                                                                                     \
        auto left = std::dynamic_pointer_cast<NumberObject>(leftVal);                \
        auto right = std::dynamic_pointer_cast<NumberObject>(rightVal);              \
        stack.push(std::make_shared<ResultObjectType>(left->get() op right->get())); \
    } while (false)

#define EQUALITY_OP(op)                                                                   \
    do                                                                                    \
    {                                                                                     \
        auto rightVal = stack.top();                                                      \
        stack.pop();                                                                      \
        auto leftVal = stack.top();                                                       \
        stack.pop();                                                                      \
                                                                                          \
        if (leftVal->type != rightVal->type)                                              \
            stack.push(std::make_shared<BooleanObject>(leftVal->type op rightVal->type)); \
        else if (leftVal->type == ObjectType::BOOLEAN)                                    \
        {                                                                                 \
            auto left = std::dynamic_pointer_cast<BooleanObject>(leftVal);                \
            auto right = std::dynamic_pointer_cast<BooleanObject>(rightVal);              \
            stack.push(std::make_shared<BooleanObject>(left->get() op right->get()));     \
        }                                                                                 \
        else if (leftVal->type == ObjectType::NUMBER)                                     \
        {                                                                                 \
            auto left = std::dynamic_pointer_cast<NumberObject>(leftVal);                 \
            auto right = std::dynamic_pointer_cast<NumberObject>(rightVal);               \
            stack.push(std::make_shared<BooleanObject>(left->get() op right->get()));     \
        }                                                                                 \
        else if (leftVal->type == ObjectType::STRING)                                     \
        {                                                                                 \
            auto left = std::dynamic_pointer_cast<StringObject>(leftVal);                 \
            auto right = std::dynamic_pointer_cast<StringObject>(rightVal);               \
            stack.push(std::make_shared<BooleanObject>(left->get() op right->get()));     \
        }                                                                                 \
    } while (false)

#define BINARY_LOGICAL_OP(op)                                                    \
    do                                                                           \
    {                                                                            \
        auto rightVal = stack.top();                                             \
        if (rightVal->type != ObjectType::BOOLEAN)                               \
        {                                                                        \
            error(translationUnit, bytecode, "Operand must be a boolean.");      \
            return 1;                                                            \
        }                                                                        \
        stack.pop();                                                             \
                                                                                 \
        auto leftVal = stack.top();                                              \
        if (leftVal->type != ObjectType::BOOLEAN)                                \
        {                                                                        \
            error(translationUnit, bytecode, "Operand must be a boolean.");      \
            return 1;                                                            \
        }                                                                        \
        stack.pop();                                                             \
                                                                                 \
        auto left = std::dynamic_pointer_cast<BooleanObject>(leftVal);           \
        auto right = std::dynamic_pointer_cast<BooleanObject>(rightVal);         \
        stack.push(std::make_shared<BooleanObject>(left->get() op right->get())); \
    } while (false)

/*
    The constant is stored in the constant pool of the bytecode.
    We need to fetch the constant at the index indicated by
    the operand of LDC. Thus, we increment the instruction pointer
    to fetch Object at the constant offset in the pool.
*/
#define GET_CONST() (bytecode.constants[code[++ip]])

#define GET_STRING() std::dynamic_pointer_cast<StringObject>(GET_CONST())

#define READ_OPERAND() code[++ip]

        auto const& code = bytecode.blob;

        for (ip = 0; ip < code.size(); ip++) {

#ifdef TRACE_EXECUTION
    printf("\n%s%s%5zu%s | ", ANSICodes::BOLD, ANSICodes::BLUE, ip, ANSICodes::RESET);

    for (const auto& stackItem : stack) {
        std::cout << stackItem->toString() << ", ";
    }

    std::cout << std::endl;
#endif

            switch (code[ip]) {
                case LDC: {
                    stack.push(GET_CONST());
                    break;
                }

                case POP: {
                    stack.pop();
                    break;
                }

                case POPN: {
                    auto popCount = READ_OPERAND();
                    stack.multipop(popCount);
                    break;
                }

                case LDVAR: {
                    auto stackIndex = READ_OPERAND();
                    stack.push(stack[stackIndex]);
                    break;
                }

                case SETVAR: {
                    auto stackIndex = READ_OPERAND();
                    stack[stackIndex] = stack.top();
                    break;
                }

                case NEWARR: {
                    auto arraySize = READ_OPERAND();

                    auto arrObj = std::make_shared<ArrayObject>();
                    for (size_t i = 0; i < arraySize; i++) {
                        arrObj->push(stack[stack.size() - arraySize + i]);
                    }

                    stack.multipop(arraySize);
                    stack.push(arrObj);

                    break;
                }

                case SETPROP: { 
                    const auto newVal = stack.top();
                    stack.pop();

                    const auto property = stack.top();
                    stack.pop();

                    const auto& object = stack.top();

                    switch (object->type) {
                        case ObjectType::ARRAY: {
                            const auto arr = std::dynamic_pointer_cast<ArrayObject>(object).get();
                            (*arr)[property] = newVal;
                            break;
                        }
                        default:
                            break;
                    }

                    break;
                }

                case LDPROP: {
                    const auto property = stack.top();
                    stack.pop();
                    
                    const auto object = stack.top();
                    stack.pop();

                    switch (object->type) {
                        case ObjectType::ARRAY: {
                            const auto& arr = *std::dynamic_pointer_cast<ArrayObject>(object).get();
                            stack.push(arr[property]);
                            break;
                        }
                        default:
                            stack.push(std::make_shared<EmptyObject>(ObjectType::UNDEFINED));
                    }

                    break;
                }

                case JMP: {
                    // decrementing to offset for the loop increment
                    auto jumpOffset = READ_OPERAND() - 1;
                    ip = jumpOffset;
                    break;
                }

                case JNT: {
                    // decrementing to offset for the loop increment
                    auto jumpOffset = READ_OPERAND() - 1;
                    if (!isTruthy(stack.top())) {
                        ip = jumpOffset;
                    }

                    break;
                }
                
                // Basic arithmetic
                case NEG: {
                    auto obj = stack.top();
                    if (obj->type != ObjectType::NUMBER) {
                        error(translationUnit, bytecode, "Operand must be a number.");
                        return 1;
                    }

                    stack.pop();

                    auto numObj = std::dynamic_pointer_cast<NumberObject>(obj);
                    stack.push(std::make_shared<NumberObject>(-numObj->get()));
                    break;
                }

                case ADD: {
                    auto rightVal = stack.top();
                    stack.pop();
                    auto leftVal = stack.top();
                    stack.pop();

                    if (leftVal->type == ObjectType::STRING || rightVal->type == ObjectType::STRING) {
                        auto concat = leftVal->toString() + rightVal->toString();
                        stack.push(std::make_shared<StringObject>(concat));
                    } else if (leftVal->type == ObjectType::NUMBER && rightVal->type == ObjectType::NUMBER) {
                        auto left = std::dynamic_pointer_cast<NumberObject>(leftVal);
                        auto right = std::dynamic_pointer_cast<NumberObject>(rightVal);
                        stack.push(std::make_shared<NumberObject>(left->get() + right->get()));
                    } else {
                        error(translationUnit, bytecode, "Invalid operand types for operator +");
                        return 1;
                    }

                    break;
                }
                
                case SUB: BINARY_OP(-, NumberObject); break;
                case MUL: BINARY_OP(*, NumberObject); break;
                case DIV: BINARY_OP(/, NumberObject); break;
                case MOD: {
                    auto rightVal = stack.top();
                    if (rightVal->type != ObjectType::NUMBER)
                    {
                        error(translationUnit, bytecode, "Operand must be a number.");
                        return 1;
                    }
                    stack.pop();

                    auto leftVal = stack.top();
                    if (leftVal->type != ObjectType::NUMBER)
                    {
                        error(translationUnit, bytecode, "Operand must be a number.");
                        return 1;
                    }
                    stack.pop();

                    auto left = std::dynamic_pointer_cast<NumberObject>(leftVal);
                    auto right = std::dynamic_pointer_cast<NumberObject>(rightVal);
                    stack.push(std::make_shared<NumberObject>(std::fmod(left->get(), right->get())));
                    break;
                };
                case EXP: {
                    auto rightVal = stack.top();
                    if (rightVal->type != ObjectType::NUMBER) {
                        error(translationUnit, bytecode, "Operand must be a number.");
                        return 1;
                    }
                    stack.pop();

                    auto leftVal = stack.top();
                    if (leftVal->type != ObjectType::NUMBER) {
                        error(translationUnit, bytecode, "Operand must be a number.");
                        return 1;
                    }
                    stack.pop();

                    auto left = std::dynamic_pointer_cast<NumberObject>(leftVal);
                    auto right = std::dynamic_pointer_cast<NumberObject>(rightVal);
                    stack.push(std::make_shared<NumberObject>(std::pow(left->get(), right->get())));  
                    break;
                }

                case INCR: {
                    if (stack.top()->type != ObjectType::NUMBER) {
                        error(translationUnit, bytecode, "Cannot increment non-numeric type");
                        return 1;
                    }

                    std::dynamic_pointer_cast<NumberObject>(stack.top())->increment();
                    break;
                }

                case DECR: {
                    if (stack.top()->type != ObjectType::NUMBER) {
                        error(translationUnit, bytecode, "Cannot decrement non-numeric type");
                        return 1;
                    }

                    std::dynamic_pointer_cast<NumberObject>(stack.top())->decrement();
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
                    auto obj = stack.top();
                    if (obj->type != ObjectType::BOOLEAN) {
                        error(translationUnit, bytecode, "Operand must be a boolean.");
                        return 1;
                    }

                    stack.pop();
                    auto numObj = std::dynamic_pointer_cast<BooleanObject>(obj);
                    stack.push(std::make_shared<BooleanObject>(!numObj->get()));
                    break;
                }

                case PRINT: {
                    std::cout << ANSICodes::WHITE <<
                        stack.top()->toString() << ANSICodes::RESET << std::endl;
                    stack.pop();
                    break;
                }

                default:
                    printf("%sVM Error: Invalid instruction (%li)\n%s", ANSICodes::RED, code[ip], ANSICodes::RESET);
                    return 1;
            }
        }
#ifdef UNWIND_STACK
        // Print stack residue
        while (!stack.empty()) {
            std::cout << ANSICodes::RED << stack.top()->toString() << ANSICodes::RESET << std::endl;
            stack.pop();
        }
#endif

#undef BINARY_OP
#undef BINARY_MATH_H
#undef EQUALITY_OP
#undef GET_CONST
#undef GET_STRING
#undef READ_OPERAND

        return 0;
    }

} // namespace cu