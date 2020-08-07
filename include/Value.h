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

		StoreAs(ValueType, std::string);
        StoreAs(const double);
		StoreAs(const bool);
	};

	struct Value {
		Value(ValueType type, std::string lexeme);
        Value(const double number);
		Value(const bool boolean);

		const ValueType type;
		const StoreAs as;

		std::string toString() const;

        friend std::ostream& operator<<(std::ostream&, const Value&);
	};

}