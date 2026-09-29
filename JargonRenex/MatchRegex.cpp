#include "MatchRegex.h"

#include "BoostRegexImpl.h"


namespace Jargon{

	bool MatchRegex::Match(Jargon::ErrorContext& errorContext, const char* matchPattern, const char* modifiers, const std::string& toProcess) {
		MatchRegex::RegexImpl regex(errorContext);

		if (!regex.compile(matchPattern, modifiers)) {
			return false;
		}

		if (!regex.match(toProcess)) {
			return false;
		}

		return true;
	}

	bool MatchRegex::Match(Jargon::ErrorContext& errorContext, const char* matchPattern, const char* modifiers, const std::string& toProcess, std::vector<std::string>& capturesOut) {
		MatchRegex::RegexImpl regex(errorContext);

		if (!regex.compile(matchPattern, modifiers)) {
			return false;
		}

		if (!regex.match(toProcess, capturesOut)) {
			return false;
		}

		return true;
	}

	MatchRegex::MatchRegex(Jargon::ErrorContext& errorContext){
		impl = new RegexImpl(errorContext);
	}

	MatchRegex::~MatchRegex(){
	}

	bool MatchRegex::compile(const char* matchPattern, const char* modifiers) {
		return impl->compile(matchPattern, modifiers);
	}

	bool MatchRegex::match(const std::string& toProcess) {
		return impl->match(toProcess);
	}

	bool MatchRegex::match(const std::string& toProcess, std::vector<std::string>& capturesOut) {
		return impl->match(toProcess, capturesOut);
	}

}
