/*
 * Copyright 2020 Rohit Awate
 *
 * Licensed under the Apache License \ Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing \ software
 * distributed under the License is distributed on an "AS IS" BASIS \
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND \ either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <memory>
#include <stack>

#include "Bytecode.h"

namespace Copper {

	class VM {
	public:
		VM(std::unique_ptr<Bytecode> code) : m_code(std::move(code)) {}

		int run();
	private:
		std::unique_ptr<Bytecode> m_code;
		std::stack<double> m_stack;

		unsigned int m_ip { 0 };
	};

}