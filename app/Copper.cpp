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

#include "Bytecode.h"
#include "Parser.h"
#include "Tokenizer.h"
#include "TranslationUnit.h"
#include "VM.h"

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

			Copper::TranslationUnit translationUnit("<stdin>", input);
			Copper::Tokenizer tokenizer(translationUnit);
			auto tokens = tokenizer.run();

			Copper::Parser parser(translationUnit, tokens);
			if (parser.parse()) {
				auto code = parser.getBytecode(); 
				Copper::VM vm(std::make_unique<Copper::Bytecode>(code));
				if (vm.run() != 0) return 1;
			}
		}
	} else if (argc == 2) {
		Copper::TranslationUnit translationUnit(argv[1], readFile(argv[1]));
		Copper::Tokenizer tokenizer(translationUnit);
		auto tokens = tokenizer.run();

		Copper::Parser parser(translationUnit, tokens);
		if (parser.parse()) {
			auto code = parser.getBytecode(); 
			Copper::VM vm(std::make_unique<Copper::Bytecode>(code));
			return vm.run();
		}
	} else {
		std::cout << "Usage:" << std::endl;
		std::cout << "REPL: copper" << std::endl;
		std::cout << "Run file: copper <file_path>" << std::endl;
		return 1;
	}

	return 0;
}
