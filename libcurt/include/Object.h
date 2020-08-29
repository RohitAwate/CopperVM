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
#include <memory>
#include <string>
#include <vector>

namespace Copper {

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
		Object(ObjectType type, bool isConst = true) : type(type), isConst(isConst) {}
		
		virtual std::string toString() const = 0;
		friend std::ostream &operator<<(std::ostream&, const Object&);

		const ObjectType type;
		const bool isConst;
	};

	class EmptyObject : public Object {
	public:
		EmptyObject(const ObjectType& type, const bool &isConst = true)
			: Object(type, isConst) {}

		std::string toString() const;
	};

	class BooleanObject : public Object {
	public:
		BooleanObject(const std::string& lexeme, const bool& isConst = true)
			: Object(ObjectType::BOOLEAN, isConst), val(lexeme == "true") {}

		BooleanObject(const bool& val, const bool &isConst)
			: Object(ObjectType::BOOLEAN, isConst), val(val) {}

		std::string toString() const;
		bool get() const { return val; }

		BooleanObject operator==(BooleanObject& o) { return { val == o.val, false }; }
		BooleanObject operator!=(BooleanObject &o) { return {val != o.val, false}; }
		BooleanObject operator&&(BooleanObject &o) { return {val && o.val, false}; }
		BooleanObject operator||(BooleanObject &o) { return {val || o.val, false}; }
		BooleanObject operator!() { return {!val, false}; }
	private:
		bool val;
	};

	class NumberObject : public Object {
	public:
		NumberObject(const std::string& lexeme, const bool& isConst = true)
			: Object(ObjectType::NUMBER, isConst), val(std::stod(lexeme)) {}
			
		NumberObject(const double &val, const bool &isConst)
			: Object(ObjectType::NUMBER, isConst), val(val) {}
			
		std::string toString() const;
		double get() const { return val; }
		void increment() { val++; }
		void decrement() { val--; }

		NumberObject operator+(NumberObject& o) { return { val + o.val, false }; }
		NumberObject operator-(NumberObject& o) { return { val - o.val, false }; }
		NumberObject operator*(NumberObject& o) { return { val * o.val, false }; }
		NumberObject operator/(NumberObject& o) { return { val / o.val, false }; }
		NumberObject operator%(NumberObject& o) { return { std::fmod(val, o.val), false }; }
		NumberObject toPower(NumberObject& o) { return { std::pow(val, o.val), false }; }
		NumberObject operator-() { return { -val, false}; }

		BooleanObject operator==(NumberObject &o) { return { val == o.val, false }; }
		BooleanObject operator!=(NumberObject &o) { return { val != o.val, false }; }

		BooleanObject operator>(NumberObject &o) { return { val > o.val, false }; }
		BooleanObject operator<(NumberObject &o) { return { val < o.val, false }; }
		BooleanObject operator>=(NumberObject &o) { return { val >= o.val, false }; }
		BooleanObject operator<=(NumberObject &o) { return { val <= o.val, false }; }
	private:
		double val;
	};

	class StringObject : public Object {
	public:
		StringObject(const std::string& lexeme, const bool& isConst = true)
			: Object(ObjectType::STRING, isConst), val(std::move(lexeme)) {}

		std::string toString() const;
		std::string get() const { return val; }

		BooleanObject operator==(StringObject &o) { return {val == o.val, false}; }
		BooleanObject operator!=(StringObject &o) { return {val != o.val, false}; }
	private:
		std::string val;
	};

	class ArrayObject : public Object {
	public:
		ArrayObject(const bool& isConst = true)
			: Object(ObjectType::ARRAY, isConst) {}

		std::string toString() const;
		std::vector<std::shared_ptr<Object>> get() const { return val; }

		void push(const std::shared_ptr<Object>& obj) { val.push_back(obj); }
		size_t length() const { return val.size(); }
		std::shared_ptr<Object> operator[] (const size_t index) const;
		std::shared_ptr<Object> operator[] (const std::shared_ptr<Object>& property) const;
	private:
		std::vector<std::shared_ptr<Object>> val;
	};

} // namespace Copper