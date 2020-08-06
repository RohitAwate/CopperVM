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

#include "Value.h"

namespace Copper {

	StoreAs::StoreAs(ValueType type, std::string lexeme) {
		switch (type) {
			case ValueType::BOOLEAN:
				boolean = lexeme == "true";
				break;
			case ValueType::NUMBER:
				number = std::stod(lexeme);
				break;
		}
	}

	StoreAs::StoreAs(const double n) {
		number = n;
	}

	StoreAs::StoreAs(const bool b) {
		boolean = b;
	}

	Value::Value(ValueType type, std::string lexeme) : m_type(type), as(StoreAs(type, lexeme)) {}

	Value::Value(const double number) : m_type(ValueType::NUMBER), as(StoreAs(number)) {}

	Value::Value(const bool boolean) : m_type(ValueType::BOOLEAN), as(StoreAs(boolean)) {}

	std::ostream& operator<<(std::ostream& stream, const Value& value) {
		switch (value.m_type) {
			case ValueType::NUMBER:
				stream << value.as.number;
				break;
			case ValueType::BOOLEAN:
				stream << std::boolalpha << value.as.boolean << std::noboolalpha;
				break;
		}

		return stream;
	}

}