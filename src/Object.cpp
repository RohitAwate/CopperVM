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

#include "Object.h"

namespace Copper {

	std::string EmptyObject::toString() const {
		switch (type) {
			case ObjectType::NULL_TYPE:
				return "null";
			case ObjectType::UNDEFINED:
				return "undefined";
		}
	}

	std::string BooleanObject::toString() const {
		return val == true ? "true" : "false";
	}

	std::string NumberObject::toString() const {
		// Check if value is integral, convert to long
		// so that we don't see the fractional part.
		double temp;
		if (std::modf(val, &temp) == 0.0) {
			return std::to_string((long) val);
		}

		/*
			If not integral, call to std::to_string
			produces trailing zeroes in the string.
			For example, 61.6 appears as 61.600000.

			To avoid this, we erase the trailing zeroes.
		*/
		auto str = std::to_string(val);
		return str.erase(str.find_last_not_of('0') + 1, std::string::npos);
	}

	std::string StringObject::toString() const {
		return val;
	}

	std::ostream& operator<<(std::ostream& stream, const Object& obj) {
		stream << obj.toString();
		return stream;
	}

} // namespace Copper