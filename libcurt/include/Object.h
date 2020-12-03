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
#include <cmath>
#include <unordered_map>
#include <memory>
#include <string>
#include <vector>

namespace cu {

    enum class ObjectType {
		BOOLEAN,
		NUMBER,
		STRING,
		ARRAY,
		UNDEFINED,
		NULL_TYPE,
	};

	class Object {
	public:
		Object(ObjectType type) : type(type) {}
		
		virtual std::string toString() const = 0;
		friend std::ostream &operator<<(std::ostream&, const Object&);

		const ObjectType type;
	};

	class EmptyObject : public Object {
	public:
		EmptyObject(const ObjectType& type)
			: Object(type) {}

		std::string toString() const;
	};

	class BooleanObject : public Object {
	public:
		BooleanObject(const std::string& lexeme)
			: Object(ObjectType::BOOLEAN), val(lexeme == "true") {}

		BooleanObject(const bool& val)
			: Object(ObjectType::BOOLEAN), val(val) {}

		std::string toString() const;
		bool get() const { return val; }
	private:
		bool val;
	};

	class NumberObject : public Object {
	public:
		NumberObject(const std::string& lexeme)
			: Object(ObjectType::NUMBER), val(std::stod(lexeme)) {}
			
		NumberObject(const double &val)
			: Object(ObjectType::NUMBER), val(val) {}
			
		std::string toString() const;
		double get() const { return val; }
		void increment() { val++; }
		void decrement() { val--; }
	private:
		double val;
	};

	class StringObject : public Object {
	public:
		StringObject(const std::string& lexeme)
			: Object(ObjectType::STRING), val(std::move(lexeme)) {}

		std::string toString() const;
		std::string get() const { return val; }
	private:
		std::string val;
	};

	class ArrayObject : public Object {
	public:
		ArrayObject()
			: Object(ObjectType::ARRAY) {}

		std::string toString() const;
		std::vector<std::shared_ptr<Object>> get() const { return val; }

		void push(const std::shared_ptr<Object>& obj) { val.push_back(obj); }
		size_t length() const { return val.size(); }

		const std::shared_ptr<Object> operator[] (const size_t index) const;
		const std::shared_ptr<Object> operator[](const std::shared_ptr<Object>& property) const;
		std::shared_ptr<Object>& operator[](const std::shared_ptr<Object>& property);
	private:
		std::vector<std::shared_ptr<Object>> val;
		std::unordered_map<std::string, std::shared_ptr<Object>> props;
	};

} // namespace cu