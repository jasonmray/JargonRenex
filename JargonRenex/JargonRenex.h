#pragma once

#include "ActionLog.h"
#include "PerlReplaceExpression.h"
#include "ProgramOptions.h"
#include "RenexResults.h"
#include "ReplaceRegexWide.h"

#include "Jargon/ErrorContext.h"


class JargonRenex {
	public:
		JargonRenex(const ProgramOptions& programOptions, Jargon::ErrorContext& errorContext);
		~JargonRenex();

		bool run();

	private:
		const ProgramOptions& programOptions;
		Jargon::ErrorContext& errorContext;

		Jargon::PerlReplaceExpression perlReplaceExpression;
		Jargon::ReplaceRegexWide regex;

		bool processFilenameOrContinue(const std::wstring& inputFilename, Jargon::RenexResults& renexResults);
		bool processFilename(const std::wstring& inputFilename, Jargon::RenexResults& renexResults);
		bool renex(const std::wstring& inputFilename, std::wstring& processedFilename);
		bool doRenames(const Jargon::RenexResults& renexResults);
		bool doRename(const std::wstring& oldFilename, const std::wstring& newFilename, ActionLog& actionLog);
		bool moveFile(const std::wstring& oldFilename, const std::wstring& newFilename);
};

