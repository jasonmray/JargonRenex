#pragma once

#include "Jargon/ErrorContext.h"
#include "Jargon/ScopedPointer.h"

#include <string>
#include <vector>


namespace Jargon{

	class BoostRegexImpl;

	class MatchRegex{
		public:
			static bool Match(Jargon::ErrorContext& errorContext, const char* matchPattern, const char* modifiers, const std::string& toProcess);
			static bool Match(Jargon::ErrorContext& errorContext, const char* matchPattern, const char* modifiers, const std::string& toProcess, std::vector<std::string>& capturesOut);

			MatchRegex(Jargon::ErrorContext& errorContext);
			~MatchRegex();

			bool compile(const char* matchPattern, const char* modifiers);
			bool match(const std::string& toProcess);
			bool match(const std::string& toProcess, std::vector<std::string>& capturesOut);

		private:
			typedef BoostRegexImpl RegexImpl;
			Jargon::ScopedPointer<RegexImpl> impl;
	};

}

