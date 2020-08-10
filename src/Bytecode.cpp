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

#include "Bytecode.h"

namespace Copper {
	
	void Bytecode::emit(byte code) {
		m_blob.push_back(code);
	}

	void Bytecode::emit(byte b1, byte b2) {
		emit(b1); emit(b2);
	}

	size_t Bytecode::addConstant(const Object* constant) {
		switch (constant->type) {
			case ObjectType::BOOLEAN:
				m_constants.push_back(std::shared_ptr<BooleanObject>((BooleanObject*)constant));
				break;
			case ObjectType::NUMBER:
				m_constants.push_back(std::shared_ptr<NumberObject>((NumberObject*)constant));
				break;
			case ObjectType::STRING:
				m_constants.push_back(std::shared_ptr<StringObject>((StringObject*)constant));
				break;
		}

		return m_constants.size() - 1;
	}

	bool Bytecode::addIdentifier(const std::string& identifier, const bool isMutable) {
		if (m_identifers.find(identifier) != m_identifers.end())
			return false;

		m_identifers.insert(identifier);
		auto const &identifierOffset = addConstant(new StringObject(identifier, isMutable));
		
		// This might need to change for locals
		emit(OpCode::DEFGL, identifierOffset);
		return true;
	}

} // namespace Copper
