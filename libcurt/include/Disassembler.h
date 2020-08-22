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

#include "Bytecode.h"
#include "TranslationUnit.h"

namespace Copper {

	class Disassembler {
	public:
		void disassemble(const Bytecode&, const TranslationUnit&);
	private:
		size_t ip;
		void printInstruction(const std::string& opcode,
									 const std::string& operands = "",
							  		 const std::string& comment = "");
	};

} // namespace Copper