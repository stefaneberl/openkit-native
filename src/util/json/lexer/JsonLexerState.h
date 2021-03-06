/**
 * Copyright 2018-2020 Dynatrace LLC
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

#ifndef _UTIL_JSON_LEXER_JSONLEXERSTATE_H
#define _UTIL_JSON_LEXER_JSONLEXERSTATE_H

namespace util
{
	namespace json
	{
		namespace lexer
		{
			///
			/// Specifies the state of the @ref util::json::lexer::JsonLexer "JSON lexer"
			///
			enum class JsonLexerState
			{
				///
				/// Initial parsing state
				///
				INITIAL,

				///
				/// Parsing state
				///
				PARSING,

				///
				/// End of file state
				///
				END_OF_FILE,

				///
				/// Error state
				///
				ERROR
			};
		}
	}
}

#endif //_UTIL_JSON_LEXER_JSONLEXERSTATE_H
