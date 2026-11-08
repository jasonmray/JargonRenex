#include "ReplaceRegex.h"
#include "BoostRegexImpl.h"


namespace Jargon{

	bool ReplaceRegex::Replace(Jargon::ErrorContext& errorContext, const char* matchPattern, const char* replacePattern, const char* modifiers, const std::string& toProcess, std::string& result) {
		ReplaceRegex::RegexImpl regex(errorContext);

		if (!regex.compile(matchPattern, modifiers)) {
			return false;
		}

		if (!regex.replace(replacePattern, toProcess, result)) {
			return false;
		}

		return true;
	}

	ReplaceRegex::ReplaceRegex(Jargon::ErrorContext& errorContext){
		impl = new RegexImpl(errorContext);
	}

	ReplaceRegex::~ReplaceRegex(){
	}

	bool ReplaceRegex::compile(const char* matchPattern, const char* modifiers) {
		return impl->compile(matchPattern, modifiers);
	}

	bool ReplaceRegex::replace(const char* replacePattern, const std::string& toProcess, std::string& result) {
		return impl->replace(replacePattern, toProcess, result);
	}
}
