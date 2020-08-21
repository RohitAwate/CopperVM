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

#include <iostream>

#include "Bytecode.h"

namespace Copper {

	void Bytecode::emit(byte opcode, unsigned int line, unsigned int column) {
		m_blob.push_back(opcode);
		addInstructionLocation(line, column);
	}

	void Bytecode::emit(byte b1, byte b2, unsigned int line, unsigned int column) {
		emit(b1, line, column);
		emit(b2, line, column);
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
			case ObjectType::UNDEFINED:
			case ObjectType::NULL_TYPE:
				m_constants.push_back(std::shared_ptr<EmptyObject>((EmptyObject*)constant));
				break;
			default:
				// TODO: Get rid of this eventually
				std::cout << "addConstant case missing: " << (int) constant->type << std::endl;
				std::exit(1);
		}

		return m_constants.size() - 1;
	}

	void Bytecode::addIdentifier(const std::string &identifier, const bool isConst, unsigned int line, unsigned int column) {
		auto const &identifierOffset = addConstant(new StringObject(identifier, isConst));
				
		// This might need to change for locals
		emit(OpCode::DEFGL, identifierOffset, line, column);
	}

	std::pair<unsigned int, unsigned int> Bytecode::getSourceLocation(byte opcodeIndex) const {
		for (auto lineItr = m_locations.begin(); lineItr != m_locations.end(); lineItr++) {
			auto& lineLocations = lineItr->second;

			if (opcodeIndex < lineLocations.size()) {
				return {lineItr->first, lineLocations[opcodeIndex]};
			} else {
				opcodeIndex -= lineLocations.size();
			}
		}

		return {0, 0};
	}

	void Bytecode::addInstructionLocation(const unsigned int& line, const unsigned int& column) {
		if (m_locations.find(line) != m_locations.end()) {
			m_locations[line].push_back(column);
		} else {
			m_locations[line] = {};
			m_locations[line].push_back(column);
		}
	}

} // namespace Copper
