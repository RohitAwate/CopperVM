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

#include <string>
#include <vector>

namespace Copper {

	struct LocalVariable {
	public:
		std::string identifier;
		bool isConst;

		LocalVariable(const std::string identifier, const bool isConst) :
			identifier(identifier), isConst(isConst) {}

		void markInitialized() { initialized = true; }
		bool isInitialized() const { return initialized; }
	private:
		bool initialized = false;
	};

	class Environment {
	public:
		bool addNewLocal(const std::string& identifier, const bool isConst);
		int resolveLocal(const std::string& identifier);
		void beginScope();
		size_t closeScope();

		bool isLocalInScope(const std::string& identifier) const;
		bool isLocalConst(const size_t stackIndex) const;
	private:
		std::vector<LocalVariable> locals;
		std::vector<unsigned int> scopeBoundaries;

		size_t currScope = 0;
	};

}	// namespace Copper