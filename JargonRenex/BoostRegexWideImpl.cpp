#include "BoostRegexWideImpl.h"

namespace Jargon{

	BoostRegexWideImpl::BoostRegexWideImpl(Jargon::ErrorContext& errorContext) :
		errorContext(errorContext)
	{
	}

	BoostRegexWideImpl::~BoostRegexWideImpl(){
	}

	bool BoostRegexWideImpl::compile(const wchar_t* matchPattern, const wchar_t* modifiers) {
		if (!setModifiers(modifiers)) {
			return false;
		}

		try {
			auto flags = getSyntaxOptions();
			regex.assign(matchPattern, flags);
		} catch (boost::bad_expression& e) {
			errorContext.setLastError("Bad regex expression %ls %ls: %hs", matchPattern, modifiers, e.what());
			return false;
		} catch (std::exception& e) {
			errorContext.setLastError("Failed to compile regex pattern %ls %ls: %hs", matchPattern, modifiers, e.what());
			return false;
		} catch (...) {
			errorContext.setLastError("Failed to compile regex pattern %ls %ls", matchPattern, modifiers);
			return false;
		}

		return true;
	}

	bool BoostRegexWideImpl::setModifiers(const wchar_t* modifiers) {
		global = false;
		multiline = false;
		caseInsensitive = false;
		extended = false;

		for (const wchar_t* c = modifiers; *c != '\0'; c++) {
			if (*c == 'i') {
				caseInsensitive = true;
			} else if (*c == 'g') {
				global = true;
			} else if (*c == 's') {
				multiline = false;
			} else if (*c == 'm') {
				multiline = true;
			} else if (*c == 'x') {
				extended = true;
			} else {
				errorContext.setLastError("Unsupported regex modifier %c", *c);
				return false;
			}
		}

		return true;
	}

	bool BoostRegexWideImpl::replace(const wchar_t* replacePattern, const std::wstring& toProcess, std::wstring& result) {
		const auto flags = getMatchFlags();

		try {
			result = boost::regex_replace(toProcess, regex, replacePattern, flags);
		} catch (std::exception& e) {
			errorContext.setLastError("Failed to run regex replace pattern %ls %ls: %hs", regex.str().c_str(), replacePattern, e.what());
			return false;
		} catch (...) {
			errorContext.setLastError("Failed to run regex replace pattern %ls %ls", regex.str().c_str(), replacePattern);
			return false;
		}

		return true;
	}

	bool BoostRegexWideImpl::match(const std::wstring& toProcess) const {
		const auto flags = getMatchFlags();
		try {
			return boost::regex_match(toProcess, regex, flags);
		} catch (std::exception& e) {
			errorContext.setLastError("Failed to run regex match pattern %ls : %hs", regex.str().c_str(), e.what());
			return false;
		} catch (...) {
			errorContext.setLastError("Failed to run regex match pattern %ls", regex.str().c_str());
			return false;
		}
	}

	bool BoostRegexWideImpl::match(const std::wstring& toProcess, std::vector<std::wstring>& capturesOut) {
		const auto flags = getMatchFlags();
		boost::wcmatch matchResults;
		
		try {
			if (!boost::regex_match(toProcess.c_str(), matchResults, regex, flags)) {
				return false;
			}
		} catch (std::exception& e) {
			errorContext.setLastError("Failed to run regex match pattern %ls : %hs", regex.str().c_str(), e.what());
			return false;
		} catch (...) {
			errorContext.setLastError("Failed to run regex match pattern %ls", regex.str().c_str());
			return false;
		}
		

		capturesOut.clear();
		capturesOut.reserve(matchResults.size());
		for (size_t i = 1; i < matchResults.size(); i++) {
			auto& match = matchResults[i];
			capturesOut.push_back(match);
		}

		return true;
	}

	boost::regex_constants::syntax_option_type BoostRegexWideImpl::getSyntaxOptions() const {
		boost::regex_constants::syntax_option_type flags;

		flags = boost::regex_constants::perl;
		if (caseInsensitive == true) {
			flags |= boost::regex_constants::icase;
		}
		if (extended == true) {
			flags |= boost::regex_constants::extended;
		}

		return flags;
	}

	boost::regex_constants::match_flag_type BoostRegexWideImpl::getMatchFlags() const {
		boost::regex_constants::match_flag_type flags;

		flags = boost::regex_constants::match_flags::format_perl | boost::regex_constants::match_flags::match_perl;
		if (global == false) {
			flags |= boost::regex_constants::match_flags::format_first_only;
		}
		if (multiline == false) {
			flags |= boost::regex_constants::match_flags::match_single_line;
		}

		return flags;
	}
}
