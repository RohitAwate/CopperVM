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
#include <stack>
#include <unordered_map>

#include "Bytecode.h"

namespace Copper {

	class VM {
	public:
		int run(const Bytecode&);
	private:
		std::stack<std::shared_ptr<Object>> m_stack;

		/**
		 * Stores the global variables.
		 * Map from identifier to a pair constituting the actual object
		 * and a flag indicating if the variable is const.
		 */ 
		std::unordered_map<std::string, std::pair<std::shared_ptr<Object>, bool>> m_globals;

		size_t m_ip = 0;

		void error(const std::string& msg) const;
	};

} // namespace Copper