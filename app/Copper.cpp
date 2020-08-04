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

#define DEBUG_TRACE_PARSE

static std::string readFile(const std::string& path) {
	std::ifstream fd;
	fd.open(path);

	if (!fd) {
		std::cout << "Could not open file: " << path << std::endl;
		std::exit(1);
	}

	std::string line;
	std::ostringstream buffer;
	while (std::getline(fd, line)) {
		buffer << line;
		buffer << '\n';
	}
		
	return buffer.str();
}

int main(int argc, const char* argv[]) {
	if (argc == 1) {
		std::cout << "CopperVM 0.1" << std::endl;
		
		for (;;) {
			std::cout << "> ";
			std::string input;
			std::getline(std::cin, input);

			if (std::cin.eof()) {
				std::cout << std::endl;
				return 0;
			}

			Copper::Tokenizer tokenizer("<stdin>", std::make_unique<std::string>(input));
			auto tokens = tokenizer.run();

#ifdef DEBUG_TRACE_PARSE
			for (Copper::Token token : tokens) {
				std::cout << Copper::toString(token.getType()) << " " << token.getLexeme() << " [" << token.getLine() << ":" << token.getColumn() << "]" << std::endl;
			}
#endif
		}
	} else if (argc == 2) {
		Copper::Tokenizer tokenizer(argv[1], std::make_unique<std::string>(readFile(argv[1])));
		auto tokens = tokenizer.run();

#ifdef DEBUG_TRACE_PARSE
		for (Copper::Token token : tokens)
		{
			std::cout << Copper::toString(token.getType()) << " " << token.getLexeme() << " [" << token.getLine() << ":" << token.getColumn() << "]" << std::endl;
		}
#endif
	} else {
		std::cout << "Usage:" << std::endl;
		std::cout << "REPL: copper" << std::endl;
		std::cout << "Run file: copper <file_path>" << std::endl;
		return 1;
	}

	return 0;
}
