#include "PerlReplaceExpression.h"
#include "MatchRegexWide.h"

#include <string>
#include <vector>


namespace Jargon{

	PerlReplaceExpression::PerlReplaceExpression(Jargon::ErrorContext& errorContext) :
		errorContext(errorContext)
	{
	}

	PerlReplaceExpression::~PerlReplaceExpression(){
	}

	bool PerlReplaceExpression::parse(const wchar_t* expression) {
		const std::wstring expressionString(expression);
		return parse(expressionString);
	}

	bool PerlReplaceExpression::parse(const std::wstring& expression) {
		const wchar_t* perlExpressionFormat = L"s/(.*[^\\\\]?)/(.*[^\\\\]?)/([smgix]*)";

		std::vector<std::wstring> captures;
		if (!MatchRegexWide::Match(errorContext, perlExpressionFormat, L"", expression, captures)) {
			errorContext.setLastError("Regex is not a valid perl replace expression: %ls", expression.c_str());
			return false;
		}

		if (captures.size() != 3) {
			errorContext.setLastError("Failed to parse perl replace expression: %ls", expression.c_str());
			return false;
		}

		matchPattern = captures[0];
		replacePattern = captures[1];
		modifiers = captures[2];

		return true;
	}

	const std::wstring& PerlReplaceExpression::getMatchPattern() const {
		return matchPattern;
	}

	const std::wstring& PerlReplaceExpression::getReplacePattern() const {
		return replacePattern;
	}

	const std::wstring& PerlReplaceExpression::getModifiers() const {
		return modifiers;
	}

}
