#include "ReplaceRegexWide.h"
#include "BoostRegexWideImpl.h"


namespace Jargon{
	bool ReplaceRegexWide::Replace(Jargon::ErrorContext& errorContext, const PerlReplaceExpression& expression, const std::wstring& toProcess, std::wstring& result) {
		return Replace(
			errorContext,
			expression.getMatchPattern().c_str(),
			expression.getReplacePattern().c_str(),
			expression.getModifiers().c_str(),
			toProcess,
			result
		);
	}

	bool ReplaceRegexWide::Replace(Jargon::ErrorContext& errorContext, const wchar_t* matchPattern, const wchar_t* replacePattern, const wchar_t* modifiers, const std::wstring& toProcess, std::wstring& result) {
		ReplaceRegexWide::RegexImpl regex(errorContext);

		if (!regex.compile(matchPattern, modifiers)) {
			return false;
		}

		if (!regex.replace(replacePattern, toProcess, result)) {
			return false;
		}

		return true;
	}

	ReplaceRegexWide::ReplaceRegexWide(Jargon::ErrorContext& errorContext){
		impl = new RegexImpl(errorContext);
	}

	ReplaceRegexWide::~ReplaceRegexWide(){
	}

	bool ReplaceRegexWide::compile(const PerlReplaceExpression& expression) {
		return compile(expression.getMatchPattern().c_str(), expression.getModifiers().c_str());
	}

	bool ReplaceRegexWide::compile(const wchar_t* matchPattern, const wchar_t* modifiers) {
		return impl->compile(matchPattern, modifiers);
	}

	bool ReplaceRegexWide::replace(const PerlReplaceExpression& expression, const std::wstring& toProcess, std::wstring& result) {
		return replace(expression.getReplacePattern().c_str(), toProcess, result);
	}

	bool ReplaceRegexWide::replace(const wchar_t* replacePattern, const std::wstring& toProcess, std::wstring& result) {
		return impl->replace(replacePattern, toProcess, result);
	}
}
