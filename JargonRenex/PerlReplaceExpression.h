#pragma once

#include "Jargon/ErrorContext.h"

#include <string>


namespace Jargon{

	class PerlReplaceExpression{
		public:
			PerlReplaceExpression(Jargon::ErrorContext& errorContext);
			~PerlReplaceExpression();

			bool parse(const wchar_t* expression);
			bool parse(const std::wstring& expression);

			const std::wstring& getMatchPattern() const;
			const std::wstring& getReplacePattern() const;
			const std::wstring& getModifiers() const;

		private:
			Jargon::ErrorContext& errorContext;

			std::wstring matchPattern;
			std::wstring replacePattern;
			std::wstring modifiers;
	};

}

