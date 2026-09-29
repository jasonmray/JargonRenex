#pragma once

#include "Jargon/ErrorContext.h"

#include <boost/regex.hpp>


namespace Jargon{

	class BoostRegexImpl{
		public:
			BoostRegexImpl(Jargon::ErrorContext& errorContext);
			~BoostRegexImpl();

			bool compile(const char* matchPattern, const char* modifiers);

			// Set PCRE regex modifiers from the given string.
			// These are the characters that appear at the end of the perl regex expression,
			// such as the i and g here:
			//      m/something/ig
			//
			// Only suppored flags are:
			//   m : multiline
			//   g : global
			//   s : single-line
			//   i : case-insensitive
			//   x : extended syntax
			bool setModifiers(const char* modifiers);

			bool replace(const char* replacePattern, const std::string& toProcess, std::string& result);
			bool match(const std::string& toProcess) const;
			bool match(const std::string& toProcess, std::vector<std::string>& captures);

		private:
			Jargon::ErrorContext& errorContext;

			boost::regex regex;
			boost::regex_constants::syntax_option_type flags = 0;

			bool global = false;
			bool multiline = false;
			bool caseInsensitive = false;
			bool extended = false;

			boost::regex_constants::syntax_option_type getSyntaxOptions() const;
			boost::regex_constants::match_flag_type getMatchFlags() const;
	};

}

