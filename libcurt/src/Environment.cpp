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

#include "Environment.h"

namespace cu {

	bool Environment::newVariable(const std::string &identifier, const bool isConst) {
		if (isVariableInScope(identifier)) {
			return false;
		}

		const auto newVar = Variable(identifier, isConst);
		variables.push_back(newVar);
		
		// Store globals separately so that they can be referenced
		// in REPL sessions.
		if (currScope == 1) globals.push_back(newVar);

		return true;
	}

	int Environment::resolveVariable(const std::string &identifier) {
		if (variables.empty() && globals.empty()) return -1;

		for (int i = variables.size() - 1; i >= 0; i--) {
			if (variables[i].identifier == identifier) return i;
		}

		for (int i = globals.size() - 1; i >= 0; i--) {
			if (globals[i].identifier == identifier) return i;
		}

		return -1;
	}

	void Environment::beginScope() {
		currScope++;
		scopeBoundaries.push_back(variables.size());
	}

	size_t Environment::closeScope() {
		currScope--;
		size_t popCount = variables.size() - scopeBoundaries[currScope];
		variables.erase(variables.begin() + scopeBoundaries[currScope], variables.end());
		scopeBoundaries.pop_back();
		return popCount;
	}

	void Environment::clear() {
		currScope = 0;
	}

	bool Environment::isVariableInScope(const std::string &identifier) const {
		for (size_t i = scopeBoundaries[currScope - 1]; i < variables.size(); i++) {
			if (variables[i].identifier == identifier) {
				return true;
			}
		}

		for (const auto& global : globals) {
			if (global.identifier == identifier) {
				return true;
			}
		}

		return false;
	}

	bool Environment::isVariableConst(const size_t stackIndex) const {
		if (stackIndex >= variables.size()) return false;
		return variables[stackIndex].isConst;
	}

}	// namespace cu