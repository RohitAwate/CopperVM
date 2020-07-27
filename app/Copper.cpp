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
#include <iostream>
#include <memory>
#include <sstream>

#include "Tokenizer.h"

static std::string readFile(const std::string& path) {
	std::ifstream fd;
	fd.open(path);

	std::string line;
	std::ostringstream buffer;
	while (std::getline(fd, line))
		buffer << line;
		
	return buffer.str();
}

int main(int argc, char* argv[]) {
	Copper::Tokenizer tokenizer("main.js", std::make_unique<std::string>(readFile(argv[1])));
	auto tokens = tokenizer.run();

	for (Copper::Token token : tokens) {
		std::cout << Copper::toString(token.getType()) << " " << token.getLexeme() << " [" << token.getLine() << ":" << token.getColumn() << "]" << std::endl;
	}

	return 0;
}
