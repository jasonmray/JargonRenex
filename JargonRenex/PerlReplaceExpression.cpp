#include "PerlReplaceExpression.h"
#include "MatchRegex.h"

#include <string>
#include <vector>


namespace Jargon{

	PerlReplaceExpression::PerlReplaceExpression(Jargon::ErrorContext& errorContext) :
		errorContext(errorContext)
	{
	}

	PerlReplaceExpression::~PerlReplaceExpression(){
	}

	bool PerlReplaceExpression::parse(const char* expression) {
		const std::string expressionString(expression);
		return parse(expressionString);
	}

	bool PerlReplaceExpression::parse(const std::string& expression) {
		const char* perlExpressionFormat = "s/(.*[^\\\\]?)/(.*[^\\\\]?)/([smgix]*)";

		std::vector<std::string> captures;
		if (!MatchRegex::Match(errorContext, perlExpressionFormat, "", expression, captures)) {
			errorContext.setLastError("Regex is not a valid perl replace expression: %s", expression);
			return false;
		}

		if (captures.size() != 3) {
			errorContext.setLastError("Failed to parse perl replace expression: %s", expression);
			return false;
		}

		matchPattern = captures[0];
		replacePattern = captures[1];
		modifiers = captures[2];

		return true;
	}

	const std::string& PerlReplaceExpression::getMatchPattern() const {
		return matchPattern;
	}

	const std::string& PerlReplaceExpression::getReplacePattern() const {
		return replacePattern;
	}

	const std::string& PerlReplaceExpression::getModifiers() const {
		return modifiers;
	}

}
