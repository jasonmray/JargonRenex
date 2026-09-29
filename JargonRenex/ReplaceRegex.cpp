#include "ReplaceRegex.h"
#include "BoostRegexImpl.h"


namespace Jargon{
	bool ReplaceRegex::Replace(Jargon::ErrorContext& errorContext, const PerlReplaceExpression& expression, const std::string& toProcess, std::string& result) {
		return Replace(
			errorContext,
			expression.getMatchPattern().c_str(),
			expression.getReplacePattern().c_str(),
			expression.getModifiers().c_str(),
			toProcess,
			result
		);
	}

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

	bool ReplaceRegex::compile(const PerlReplaceExpression& expression) {
		return compile(expression.getMatchPattern().c_str(), expression.getModifiers().c_str());
	}

	bool ReplaceRegex::compile(const char* matchPattern, const char* modifiers) {
		return impl->compile(matchPattern, modifiers);
	}

	bool ReplaceRegex::replace(const PerlReplaceExpression& expression, const std::string& toProcess, std::string& result) {
		return replace(expression.getReplacePattern().c_str(), toProcess, result);
	}

	bool ReplaceRegex::replace(const char* replacePattern, const std::string& toProcess, std::string& result) {
		return impl->replace(replacePattern, toProcess, result);
	}
}
