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

#include <memory>
#include <vector>
#include <unordered_map>

#include "Bytecode.h"
#include "TranslationUnit.h"

// #define TRACE_EXECUTION

namespace Copper {

	template <typename T>
	class Stack : public std::vector<T> {
	public:
		void push(const T& t) { this->push_back(t); }

		void pop() { this->pop_back(); }

		void multipop(size_t count) { this->erase(this->end() - count, this->end()); }

		T& top() { return this->back(); }
	};

	class VM {
	public:
		int run(const Bytecode&, const TranslationUnit&);
	private:
		Stack<std::shared_ptr<Object>> stack;

		/**
		 * Stores the global variables.
		 * Map from identifier to a pair constituting the actual object
		 * and a flag indicating if the variable is const.
		 */ 
		std::unordered_map<std::string, std::pair<std::shared_ptr<Object>, bool>> globals;

		size_t ip = 0;

		void error(const TranslationUnit&, const Bytecode& bytecode, const std::string& msg) const;
	};

} // namespace Copper