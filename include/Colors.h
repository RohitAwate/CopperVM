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

namespace Copper {

	namespace ANSICodes {
		// Color codes
		static const char* RED = "\u001b[31m";
		static const char* GREEN = "\u001b[32m";
		static const char* YELLOW = "\u001b[33m";
		static const char* BLUE = "\u001b[34m";
		static const char* MAGENTA = "\u001b[35m";
		static const char* CYAN = "\u001b[36m";
		static const char* WHITE = "\u001b[37m";
		static const char* RESET = "\u001b[0m";
		
		// Decoration codes
		static const char* BOLD = "\u001b[1m";
		static const char* UNDERLINE = "\u001b[4m";
	}
	
} // namespace Copper