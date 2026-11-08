#include "MatchRegexWide.h"

#include "BoostRegexWideImpl.h"


namespace Jargon{

	bool MatchRegexWide::Match(Jargon::ErrorContext& errorContext, const wchar_t* matchPattern, const wchar_t* modifiers, const std::wstring& toProcess) {
		MatchRegexWide::RegexImpl regex(errorContext);

		if (!regex.compile(matchPattern, modifiers)) {
			return false;
		}

		if (!regex.match(toProcess)) {
			return false;
		}

		return true;
	}

	bool MatchRegexWide::Match(Jargon::ErrorContext& errorContext, const wchar_t* matchPattern, const wchar_t* modifiers, const std::wstring& toProcess, std::vector<std::wstring>& capturesOut) {
		MatchRegexWide::RegexImpl regex(errorContext);

		if (!regex.compile(matchPattern, modifiers)) {
			return false;
		}

		if (!regex.match(toProcess, capturesOut)) {
			return false;
		}

		return true;
	}

	MatchRegexWide::MatchRegexWide(Jargon::ErrorContext& errorContext){
		impl = new RegexImpl(errorContext);
	}

	MatchRegexWide::~MatchRegexWide(){
	}

	bool MatchRegexWide::compile(const wchar_t* matchPattern, const wchar_t* modifiers) {
		return impl->compile(matchPattern, modifiers);
	}

	bool MatchRegexWide::match(const std::wstring& toProcess) {
		return impl->match(toProcess);
	}

	bool MatchRegexWide::match(const std::wstring& toProcess, std::vector<std::wstring>& capturesOut) {
		return impl->match(toProcess, capturesOut);
	}

}
