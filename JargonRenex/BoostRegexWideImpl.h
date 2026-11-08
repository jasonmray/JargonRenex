#pragma once

#include "Jargon/ErrorContext.h"

#include <boost/regex.hpp>


namespace Jargon{

	class BoostRegexWideImpl{
		public:
			BoostRegexWideImpl(Jargon::ErrorContext& errorContext);
			~BoostRegexWideImpl();

			bool compile(const wchar_t* matchPattern, const wchar_t* modifiers);

			// Set PCRE regex modifiers from the given string.
			// These are the wchar_tacters that appear at the end of the perl regex expression,
			// such as the i and g here:
			//      m/something/ig
			//
			// Only suppored flags are:
			//   m : multiline
			//   g : global
			//   s : single-line
			//   i : case-insensitive
			//   x : extended syntax
			bool setModifiers(const wchar_t* modifiers);

			bool replace(const wchar_t* replacePattern, const std::wstring& toProcess, std::wstring& result);
			bool match(const std::wstring& toProcess) const;
			bool match(const std::wstring& toProcess, std::vector<std::wstring>& captures);

		private:
			Jargon::ErrorContext& errorContext;

			boost::wregex regex;
			boost::regex_constants::syntax_option_type flags = 0;

			bool global = false;
			bool multiline = false;
			bool caseInsensitive = false;
			bool extended = false;

			boost::regex_constants::syntax_option_type getSyntaxOptions() const;
			boost::regex_constants::match_flag_type getMatchFlags() const;
	};

}

