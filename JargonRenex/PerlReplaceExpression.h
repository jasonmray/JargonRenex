#pragma once

#include "Jargon/ErrorContext.h"

#include <string>


namespace Jargon{

	class PerlReplaceExpression{
		public:
			PerlReplaceExpression(Jargon::ErrorContext& errorContext);
			~PerlReplaceExpression();

			bool parse(const char* expression);
			bool parse(const std::string& expression);

			const std::string& getMatchPattern() const;
			const std::string& getReplacePattern() const;
			const std::string& getModifiers() const;

		private:
			Jargon::ErrorContext& errorContext;

			std::string matchPattern;
			std::string replacePattern;
			std::string modifiers;
	};

}

