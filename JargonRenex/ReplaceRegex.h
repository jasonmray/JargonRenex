#pragma once

#include "PerlReplaceExpression.h"

#include "Jargon/ErrorContext.h"
#include "Jargon/ScopedPointer.h"

#include <string>


namespace Jargon{

	class BoostRegexImpl;

	class ReplaceRegex{
		public:
			static bool Replace(Jargon::ErrorContext& errorContext, const PerlReplaceExpression& expression, const std::string& toProcess, std::string& result);
			static bool Replace(Jargon::ErrorContext& errorContext, const char* matchPattern, const char* replacePattern, const char* modifiers, const std::string& toProcess, std::string& result);

			ReplaceRegex(Jargon::ErrorContext& errorContext);
			~ReplaceRegex();

			bool compile(const PerlReplaceExpression& expression);
			bool compile(const char* matchPattern, const char* modifiers);

			// returns false on error.
			// returns true on success, even if no replacement was made.
			bool replace(const PerlReplaceExpression& expression, const std::string& toProcess, std::string& result);
			bool replace(const char* replacePattern, const std::string& toProcess, std::string& result);

		private:
			typedef BoostRegexImpl RegexImpl;
			Jargon::ScopedPointer<RegexImpl> impl;
	};

}

