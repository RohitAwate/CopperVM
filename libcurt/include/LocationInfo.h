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
#include <map>

namespace cu {

	struct Location {
		const unsigned int line;
		const unsigned int column;
	};

	class LocationInfo {
	public:
		void add(const Location& loc) {
			if (lines.find(loc.line) == lines.end()) {
				lines[loc.line] = {};
			}

			lines[loc.line].addColumn(loc.column);
		}

		void print() const {
			for (auto lineItr = lines.begin(); lineItr != lines.end(); lineItr++) {
				auto lineInfo = lineItr->second;

				std::cout << "line " << lineItr->first << "\t";

				for (auto colItr = lineInfo.cols.begin(); colItr != lineInfo.cols.end(); colItr++) {
					std::cout << colItr->first << ": " << colItr->second << " | ";
				}

				std::cout << std::endl;
			}
		}

		Location get(size_t& bytecodeOffset) const {
			for (auto lineItr = lines.begin(); lineItr != lines.end(); lineItr++) {
				auto lineInfo = lineItr->second;

				if (lineInfo.sum < bytecodeOffset) {
					bytecodeOffset -= lineInfo.sum;
					continue;
				}

				for (auto colItr = lineInfo.cols.begin(); colItr != lineInfo.cols.end(); colItr++) {
					auto colOffset = colItr->second;

					if (colOffset < bytecodeOffset) {
						bytecodeOffset -= colOffset;
						continue;
					} else {
						return { lineItr->first, colItr->first };
					}
				}
			}

			return { 0, 0 };
		}

		void clear() {
			lines.clear();
		}

		unsigned int size() const {
			unsigned int count = 0;

			for (const auto& lineInfo : lines) {
				count += lineInfo.second.cols.size();
			}

			return count;
		}
	private:
		struct LineInfo {
			unsigned int sum = 0;
			std::map<unsigned int, unsigned int> cols;

			void addColumn(const unsigned int& col) {
				if (cols.find(col) == cols.end()) {
					cols[col] = 0;
				}

				cols[col]++;
				sum++;
			}
		};

		std::map<unsigned int, LineInfo> lines;
	};

} // namespace cu