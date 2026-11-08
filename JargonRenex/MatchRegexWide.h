#pragma once

#include "Jargon/ErrorContext.h"
#include "Jargon/ScopedPointer.h"

#include <string>
#include <vector>


namespace Jargon{

	class BoostRegexWideImpl;

	class MatchRegexWide {
		public:
			static bool Match(Jargon::ErrorContext& errorContext, const wchar_t* matchPattern, const wchar_t* modifiers, const std::wstring& toProcess);
			static bool Match(Jargon::ErrorContext& errorContext, const wchar_t* matchPattern, const wchar_t* modifiers, const std::wstring& toProcess, std::vector<std::wstring>& capturesOut);

			MatchRegexWide(Jargon::ErrorContext& errorContext);
			~MatchRegexWide();

			bool compile(const wchar_t* matchPattern, const wchar_t* modifiers);
			bool match(const std::wstring& toProcess);
			bool match(const std::wstring& toProcess, std::vector<std::wstring>& capturesOut);

		private:
			typedef BoostRegexWideImpl RegexImpl;
			Jargon::ScopedPointer<RegexImpl> impl;
	};

}

