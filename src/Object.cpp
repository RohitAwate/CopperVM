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
		return std::to_string(val);
	}

	std::string StringObject::toString() const {
		return val;
	}

	std::ostream& operator<<(std::ostream& stream, const Object& obj) {
		stream << obj.toString();
		return stream;
	}

} // namespace Copper