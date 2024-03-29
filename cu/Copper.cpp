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

#include "Compiler.h"
#include "VM.h"

int main(int argc, const char* argv[]) {
	cu::Compiler compiler;
	cu::VM vm;

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

			auto translationUnit = cu::TranslationUnit("<stdin>", input);
			if (compiler.compile(translationUnit))
				vm.run(compiler.getBytecode(), translationUnit);
		}
	} else if (argc == 2) {
		auto translationUnit = cu::TranslationUnit(argv[1]);
		if (compiler.compile(translationUnit))
			return vm.run(compiler.getBytecode(), translationUnit);
		else
			return 1;
	} else {
		std::cout << "Usage:" << std::endl;
		std::cout << "REPL: copper" << std::endl;
		std::cout << "Run file: copper <file_path>" << std::endl;
		return 1;
	}

	return 0;
}
