#pragma once

#include "PerlReplaceExpression.h"

#include "Jargon/ErrorContext.h"
#include "Jargon/ScopedPointer.h"

#include <string>


namespace Jargon{

	class BoostRegexWideImpl;

	class ReplaceRegexWide {
		public:
			static bool Replace(Jargon::ErrorContext& errorContext, const PerlReplaceExpression& expression, const std::wstring& toProcess, std::wstring& result);
			static bool Replace(Jargon::ErrorContext& errorContext, const wchar_t* matchPattern, const wchar_t* replacePattern, const wchar_t* modifiers, const std::wstring& toProcess, std::wstring& result);

			ReplaceRegexWide(Jargon::ErrorContext& errorContext);
			~ReplaceRegexWide();

			bool compile(const PerlReplaceExpression& expression);
			bool compile(const wchar_t* matchPattern, const wchar_t* modifiers);

			// returns false on error.
			// returns true on success, even if no replacement was made.
			bool replace(const PerlReplaceExpression& expression, const std::wstring& toProcess, std::wstring& result);
			bool replace(const wchar_t* replacePattern, const std::wstring& toProcess, std::wstring& result);

		private:
			typedef BoostRegexWideImpl RegexImpl;
			Jargon::ScopedPointer<RegexImpl> impl;
	};

}

