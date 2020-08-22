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

#include <fstream>

#include "Colors.h"
#include "TranslationUnit.h"

namespace Copper {

	std::string TranslationUnit::readFile(const std::string &path) {
		std::ifstream fd;
		fd.open(path);

		if (!fd) {
			std::cout << "Could not open file: " << path << std::endl;
			std::exit(1);
		}

		return std::string((std::istreambuf_iterator<char>(fd)), (std::istreambuf_iterator<char>()));
	}	

	std::string TranslationUnit::getOffsetString(const std::string& line, size_t offset) {
		std::string offsetString;

		for (size_t i = 0; i < offset; i++) {
			if (line.at(i) == '\t') {
				offsetString.append("\t");
			} else {
				offsetString.append(" ");
			}
		}

		return offsetString;
	}

	std::string TranslationUnit::getLine(int line) const {
		size_t start = 0, end = contents.get()->size();

		int currentLine = 1;
		for (size_t i = 0; i < contents.get()->size(); i++) {
			if (contents.get()->at(i) == '\n') {
				currentLine++;

				if (currentLine == line) {
					start = i + 1;
				} else if (currentLine == line + 1) {
					end = i - 1;
				}
			}
		}

		return contents.get()->substr(start, end - start + 1);
	}

} // namespace Copper