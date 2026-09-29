
#include "BoostRegexImpl.h"

namespace Jargon{

	BoostRegexImpl::BoostRegexImpl(Jargon::ErrorContext& errorContext) :
		errorContext(errorContext)
	{
	}

	BoostRegexImpl::~BoostRegexImpl(){
	}

	bool BoostRegexImpl::compile(const char* matchPattern, const char* modifiers) {
		if (!setModifiers(modifiers)) {
			return false;
		}

		try {
			auto flags = getSyntaxOptions();
			regex.assign(matchPattern, flags);
		} catch (boost::bad_expression& e) {
			errorContext.setLastError("Bad regex expression %s %s: %s", matchPattern, modifiers, e.what());
			return false;
		} catch (std::exception& e) {
			errorContext.setLastError("Failed to compile regex pattern %s %s: %s", matchPattern, modifiers, e.what());
			return false;
		} catch (...) {
			errorContext.setLastError("Failed to compile regex pattern %s %s", matchPattern, modifiers);
			return false;
		}

		return true;
	}

	bool BoostRegexImpl::setModifiers(const char* modifiers) {
		global = false;
		multiline = false;
		caseInsensitive = false;
		extended = false;

		for (const char* c = modifiers; *c != '\0'; c++) {
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

	bool BoostRegexImpl::replace(const char* replacePattern, const std::string& toProcess, std::string& result) {
		const auto flags = getMatchFlags();

		try {
			result = boost::regex_replace(toProcess, regex, replacePattern, flags);
		} catch (std::exception& e) {
			errorContext.setLastError("Failed to run regex replace pattern %s %s: %s", regex.str(), replacePattern, e.what());
			return false;
		} catch (...) {
			errorContext.setLastError("Failed to run regex replace pattern %s %s", regex.str(), replacePattern);
			return false;
		}

		return true;
	}

	bool BoostRegexImpl::match(const std::string& toProcess) const {
		const auto flags = getMatchFlags();
		try {
			return boost::regex_match(toProcess, regex, flags);
		} catch (std::exception& e) {
			errorContext.setLastError("Failed to run regex match pattern %s : %s", regex.str(), e.what());
			return false;
		} catch (...) {
			errorContext.setLastError("Failed to run regex match pattern %s", regex.str());
			return false;
		}
	}

	bool BoostRegexImpl::match(const std::string& toProcess, std::vector<std::string>& capturesOut) {
		const auto flags = getMatchFlags();
		boost::cmatch matchResults;
		
		try {
			if (!boost::regex_match(toProcess.c_str(), matchResults, regex, flags)) {
				return false;
			}
		} catch (std::exception& e) {
			errorContext.setLastError("Failed to run regex match pattern %s : %s", regex.str(), e.what());
			return false;
		} catch (...) {
			errorContext.setLastError("Failed to run regex match pattern %s", regex.str());
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

	boost::regex_constants::syntax_option_type BoostRegexImpl::getSyntaxOptions() const {
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

	boost::regex_constants::match_flag_type BoostRegexImpl::getMatchFlags() const {
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
