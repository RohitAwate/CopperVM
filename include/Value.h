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

#include <iostream>
#include <string>

namespace Copper {
    enum class ValueType {
		BOOLEAN,
		NUMBER,
	};

	union StoreAs {
		bool boolean;
		double number;

		StoreAs(ValueType type, std::string lexeme) {
			switch (type) {
				case ValueType::BOOLEAN:
					boolean = lexeme == "true";
					break;
				case ValueType::NUMBER:
					number = std::stod(lexeme);
					break;
			}
		}

        StoreAs(const double num) {
            number = num;
		}
	};

	struct Value {
		Value(ValueType type, std::string lexeme) :
			m_type(type), as(StoreAs(type, lexeme)) {}

        Value(const double number) :
            m_type(ValueType::NUMBER), as(StoreAs(number)) {}
	
		const ValueType m_type;
		const StoreAs as;

        friend std::ostream& operator<<(std::ostream&, const Value&);
	};
}