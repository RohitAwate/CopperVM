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
#include <memory>

#include "Bytecode.h"
#include "Compiler.h"
#include "TranslationUnit.h"
#include "VM.h"

int main(int argc, const char* argv[]) {
	Copper::Compiler compiler;
	Copper::VM vm;

	if (argc == 1) {
		printf("CopperVM %s (%s %s on %s)\n", COPPER_VERSION, COMPILER_NAME, COMPILER_VERSION, PLATFORM);

		for (;;) {
			std::cout << "> ";
			std::string input;
			std::getline(std::cin, input);

			if (std::cin.eof()) {
				std::cout << std::endl;
				return 0;
			}

			auto translationUnit = Copper::TranslationUnit("<stdin>", input);
			auto bytecode = compiler.compile(translationUnit);
			vm.run(bytecode, translationUnit);
		}
	} else if (argc == 2) {
		auto translationUnit = Copper::TranslationUnit(argv[1]);
		auto bytecode = compiler.compile(translationUnit);
		return vm.run(bytecode, translationUnit);
	} else {
		std::cout << "Usage:" << std::endl;
		std::cout << "REPL: copper" << std::endl;
		std::cout << "Run file: copper <file_path>" << std::endl;
		return 1;
	}

	return 0;
}
