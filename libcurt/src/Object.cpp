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
#include <sstream>

#include "Object.h"

namespace cu {

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

	std::string ArrayObject::toString() const {
		std::ostringstream buffer;
		buffer << "[";

		for (auto itr = val.begin(); itr != val.end(); itr++) {
			buffer << itr->get()->toString();

			if (itr + 1 != val.end() || props.size() != 0) {
				buffer << ", ";
			}
		}

		for (auto itr = props.begin(); itr != props.end(); itr++) {
			buffer << "'" << itr->first << "': ";
			buffer << itr->second->toString();

			// itr + 1 works for std::vector because their iterators
			// are random access. std::unordered_map has a forward iterator
			// thus we need this hack.
			//
			// Reference: https://stackoverflow.com/a/28278854
			if (std::next(itr) != props.end()) {
				buffer << ", ";
			}
		}

		buffer << "]";

		return buffer.str();
	}

	const std::shared_ptr<Object> ArrayObject::operator[](const size_t index) const {
		if (index < val.size()) {
			return val[index];
		}

		return std::make_shared<EmptyObject>(ObjectType::UNDEFINED);
	}

	const std::shared_ptr<Object> ArrayObject::operator[](const std::shared_ptr<Object>& property) const {
		switch (property->type) {
			case ObjectType::NUMBER: {
				auto index = std::dynamic_pointer_cast<NumberObject>(property)->get();
				if (index >= 0 && index < val.size()) {
					return val[index];
				}

				break;
			}
			case ObjectType::ARRAY: {
				const auto& arr = std::dynamic_pointer_cast<ArrayObject>(property)->get();
				if (arr.size() == 1) {
					/*
						Emulating the following JS behaviour:

						let arr = [1, 2, 3];
						arr[[1]] -> 2
					*/
					const auto& index = arr[0];
					return (*this)[index];
				} else {
					return props.at(property->toString());
				}

				break;
			}
			case ObjectType::STRING: {
				auto str = std::dynamic_pointer_cast<StringObject>(property)->get();
				try {
					auto index = std::stod(str);

					if (index >= 0 && index < val.size()) {
						return val[index];
					}
				} catch(std::invalid_argument err) {}

				break;
			}
		}

		return std::make_shared<EmptyObject>(ObjectType::UNDEFINED);
	}

	std::shared_ptr<Object>& ArrayObject::operator[](const std::shared_ptr<Object>& property) {
		switch (property->type) {
			case ObjectType::NUMBER: {
				auto index = std::dynamic_pointer_cast<NumberObject>(property)->get();
				if (index >= 0 && index < val.size()) {
					return val[index];
				} else {
					// Resize to double of the required index + 1 to handle
					// the base case of when required index is 0.
					val.resize((index + 1) * 2, std::make_shared<EmptyObject>(ObjectType::UNDEFINED));
					return val[index];
				}

				break;
			}
			case ObjectType::ARRAY: {
				const auto& arr = std::dynamic_pointer_cast<ArrayObject>(property)->get();
				if (arr.size() == 1) {
					/*
						Emulating the following JS behaviour:

						let arr = [1, 2, 3];
						arr[[1]] -> 2
					*/
					const auto& index = arr[0];
					return (*this)[index];
				} else {
					const auto& propStr = property->toString();
					props[propStr] = std::make_shared<EmptyObject>(ObjectType::UNDEFINED);
					return props[propStr];
				}

				break;
			}
			case ObjectType::STRING: {
				auto str = std::dynamic_pointer_cast<StringObject>(property)->get();
				try {
					auto index = std::stod(str);

					if (index >= 0 && index < val.size()) {
						return val[index];
					} else {
						// Resize to double of the required index + 1 to handle
						// the base case of when required index is 0.
						val.resize((index + 1) * 2, std::make_shared<EmptyObject>(ObjectType::UNDEFINED));
						return val[index];
					}
				} catch(std::invalid_argument err) {}

				break;
			}
		}

		const auto& propStr = property->toString();
		props[propStr] = std::make_shared<EmptyObject>(ObjectType::UNDEFINED);
		return props[propStr];
	}

	std::ostream& operator<<(std::ostream& stream, const Object& obj) {
		stream << obj.toString();
		return stream;
	}

} // namespace cu