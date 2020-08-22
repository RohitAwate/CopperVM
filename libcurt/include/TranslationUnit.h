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
#include <memory>
#include <string>

namespace Copper {

	struct TranslationUnit {
		TranslationUnit(const std::string& filepath) :
			filepath(filepath), contents(std::make_shared<std::string>(readFile(filepath))) {}

		TranslationUnit(const std::string& filepath, std::string contents) :
			filepath(filepath), contents(std::make_shared<std::string>(contents)) {}

		std::string filepath;
		std::shared_ptr<std::string> contents;

		static std::string readFile(const std::string &path);
		static std::string getOffsetString(const std::string &line, size_t offset);
		std::string getLine(int) const;
	};

} // namespace Copper