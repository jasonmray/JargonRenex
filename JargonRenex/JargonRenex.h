#pragma once

#include "ActionLog.h"
#include "PerlReplaceExpression.h"
#include "ProgramOptions.h"
#include "RenexResults.h"
#include "ReplaceRegex.h"

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
		Jargon::ReplaceRegex regex;

		bool processFilenameOrContinue(const std::string& inputFilename, Jargon::RenexResults& renexResults);
		bool processFilename(const std::string& inputFilename, Jargon::RenexResults& renexResults);
		bool renex(const std::string& inputFilename, std::string& processedFilename);
		bool doRenames(const Jargon::RenexResults& renexResults);
		bool doRename(const std::string& oldFilename, const std::string& newFilename, ActionLog& actionLog);
		bool moveFile(const std::string& oldFilename, const std::string& newFilename);
		void printError(const Jargon::ErrorContext& errorContext);
		void printError(const char* formatString, ...);
		void printMessage(const char* formatString, ...);
		void printVarArgs(FILE* destination, const char* formatString, va_list args);
};

