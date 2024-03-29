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

#include <string>

#include "LocationInfo.h"

#define TOKEN_TYPES                  \
    /* Keywords */                   \
    TYPE_MACRO(LET)                  \
    TYPE_MACRO(CONST)                \
    TYPE_MACRO(IF)                   \
    TYPE_MACRO(ELSE)                 \
    TYPE_MACRO(SWITCH)               \
    TYPE_MACRO(CASE)                 \
    TYPE_MACRO(FOR)                  \
    TYPE_MACRO(WHILE)                \
    TYPE_MACRO(DO)                   \
    TYPE_MACRO(OF)                   \
    TYPE_MACRO(IN)                   \
    TYPE_MACRO(BREAK)                \
    TYPE_MACRO(CONTINUE)             \
    TYPE_MACRO(FUNCTION)             \
    TYPE_MACRO(RETURN)               \
    TYPE_MACRO(TRUE)                 \
    TYPE_MACRO(FALSE)                \
    TYPE_MACRO(NULL_TYPE)            \
    TYPE_MACRO(UNDEFINED)            \
    TYPE_MACRO(CLASS)                \
    TYPE_MACRO(THIS)                 \
    TYPE_MACRO(NEW)                  \
    TYPE_MACRO(STATIC)               \
    TYPE_MACRO(EXTENDS)              \
    TYPE_MACRO(SUPER)                \
    TYPE_MACRO(TYPEOF)               \
    TYPE_MACRO(INSTANCEOF)           \
    TYPE_MACRO(TRY)                  \
    TYPE_MACRO(CATCH)                \
    TYPE_MACRO(THROW)                \
    TYPE_MACRO(IMPORT)               \
    TYPE_MACRO(EXPORT)               \
    TYPE_MACRO(DEFAULT)              \
    TYPE_MACRO(ASYNC)                \
    TYPE_MACRO(AWAIT)                \
    TYPE_MACRO(PRINT)                \
    /* Identifiers */                \
    TYPE_MACRO(IDENTIFIER)           \
    TYPE_MACRO(STRING)               \
    TYPE_MACRO(NUMBER)               \
    /* Single character tokens */    \
    TYPE_MACRO(PLUS)                 \
    TYPE_MACRO(MINUS)                \
    TYPE_MACRO(MULTIPLY)             \
    TYPE_MACRO(DIVIDE)               \
    TYPE_MACRO(MODULO)               \
    TYPE_MACRO(EXPONENT)             \
    TYPE_MACRO(GRT)                  \
    TYPE_MACRO(LST)                  \
    TYPE_MACRO(OPEN_BRACE)           \
    TYPE_MACRO(CLOSE_BRACE)          \
    TYPE_MACRO(OPEN_PAREN)           \
    TYPE_MACRO(CLOSE_PAREN)          \
    TYPE_MACRO(OPEN_SQUARE_BRACKET)  \
    TYPE_MACRO(CLOSE_SQUARE_BRACKET) \
    TYPE_MACRO(NEGATION)             \
    TYPE_MACRO(ASSIGNMENT)           \
    TYPE_MACRO(SEMICOLON)            \
    TYPE_MACRO(COLON)                \
    TYPE_MACRO(DOT)                  \
    TYPE_MACRO(COMMA)                \
    TYPE_MACRO(BACK_TICK)            \
    /* Multi character tokens */     \
    TYPE_MACRO(PLUS_ASSIGNMENT)      \
    TYPE_MACRO(MINUS_ASSIGNMENT)     \
    TYPE_MACRO(MULTIPLY_ASSIGNMENT)  \
    TYPE_MACRO(DIVIDE_ASSIGNMENT)    \
    TYPE_MACRO(PLUS_PLUS)            \
    TYPE_MACRO(MINUS_MINUS)          \
    TYPE_MACRO(EQU)                  \
    TYPE_MACRO(ARROW)                \
    TYPE_MACRO(NEQ)                  \
    TYPE_MACRO(LSE)                  \
    TYPE_MACRO(GRE)                  \
    TYPE_MACRO(AND)                  \
    TYPE_MACRO(OR)                   \
    /*  ${ */                        \
    TYPE_MACRO(INTERPOLATION_START)  \
    TYPE_MACRO(EOF_TYPE)

namespace cu {

    enum class TokenType {
#define TYPE_MACRO(type) \
    type,

        TOKEN_TYPES

#undef TYPE_MACRO
    };
    
    class Token {
    public:
        Token(std::string lexeme, TokenType type, unsigned int line, unsigned int column) :
            lexeme(lexeme), type(type), loc({ line, column }) {}
        
        Token(TokenType type, unsigned int line, unsigned int column) :
            type(type), loc({ line, column })  {}

        const std::string& getLexeme() const { return lexeme; }
        const TokenType& getType() const { return type; }
        const Location& getLocation() const { return loc; }
    private:
        const std::string lexeme;
        const TokenType type;

        Location loc;
    };

    static std::string toString(const TokenType& type) {
        switch (type) {
#define TYPE_MACRO(type)  \
    case TokenType::type: \
        return #type;

            TOKEN_TYPES

#undef TYPE_MACRO
        }
    }

} // namespace cu
