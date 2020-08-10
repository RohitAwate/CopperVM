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
#include <string>

namespace Copper {

    enum class ObjectType {
		BOOLEAN,
		NUMBER,
		STRING,
	};

	class Object {
	public:
		Object(ObjectType type, bool isMutable = true) : type(type), isMutable(isMutable) {}
		
		virtual std::string toString() const = 0;
		friend std::ostream &operator<<(std::ostream&, const Object&);

		const ObjectType type;
		const bool isMutable;
	};

	class BooleanObject : public Object {
	public:
		BooleanObject(const std::string& lexeme, const bool& isMutable = true)
			: Object(ObjectType::BOOLEAN, isMutable), val(lexeme == "true") {}

		BooleanObject(const bool& val, const bool &isMutable)
			: Object(ObjectType::BOOLEAN, isMutable), val(val) {}

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
		NumberObject(const std::string& lexeme, const bool& isMutable = true)
			: Object(ObjectType::NUMBER, isMutable), val(std::stod(lexeme)) {}
			
		NumberObject(const double &val, const bool &isMutable)
			: Object(ObjectType::NUMBER, isMutable), val(val) {}
			
		std::string toString() const;
		double get() const { return val; }

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
		StringObject(const std::string lexeme, const bool& isMutable = true)
			: Object(ObjectType::STRING, isMutable), val(std::move(lexeme)) {}

		std::string toString() const;
		std::string get() const { return val; }
	private:
		std::string val;
	};

}