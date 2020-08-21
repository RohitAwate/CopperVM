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

namespace Copper {

	bool Environment::addNewLocal(const std::string &identifier, const bool isConst) {
		if (isLocalInScope(identifier)) {
			return false;
		}

		locals.push_back({ identifier, isConst });
		return true;
	}

	int Environment::resolveLocal(const std::string &identifier) {
		if (locals.empty()) return -1;

		for (int i = locals.size() - 1; i >= 0; i--) {
			if (locals[i].identifier == identifier) return i;
		}

		return -1;
	}

	void Environment::beginScope() {
		currScope++;
		scopeBoundaries.push_back(locals.size());
	}

	size_t Environment::closeScope() {
		currScope--;
		size_t popCount = locals.size() - scopeBoundaries[currScope];
		locals.erase(locals.begin() + scopeBoundaries[currScope], locals.end());
		scopeBoundaries.pop_back();
		return popCount;
	}

	bool Environment::isLocalInScope(const std::string &identifier) const {
		for (size_t i = scopeBoundaries[currScope - 1]; i < locals.size(); i++) {
			if (locals[i].identifier == identifier) {
				return true;
			}
		}

		return false;
	}

	bool Environment::isLocalConst(const size_t stackIndex) const {
		if (stackIndex >= locals.size()) return false;
		return locals[stackIndex].isConst;
	}

}	// namespace Copper