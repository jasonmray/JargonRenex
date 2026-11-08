#pragma once

#include "Jargon/ErrorContext.h"

#include <cstdio>
#include <string>
#include <vector>


class ProgramOptions{
	public:
		typedef int (*PrintFunction)(_Printf_format_string_ const wchar_t*, ...);

		enum PrintMode {
			PrintMode_Quiet   = 0,
			PrintMode_Normal  = 1,
			PrintMode_Verbose = 2
		};

		static void PrintHelp();

		ProgramOptions(Jargon::ErrorContext& errorContext);
		~ProgramOptions();

		bool process(int argc, const wchar_t** argv);
		bool verify() const;
		void printSummary(PrintFunction printFunction) const;

		bool useVerboseOutput() const;
		bool useNormalOutput() const;

		std::wstring exeName;
		bool showHelp = false;
		PrintMode printMode = PrintMode_Normal;
		bool continueOnError = false;
		bool testMode = true;
		bool performRenames = false;
		bool askForConfirmation = false;
		bool readStdin = false;
		bool includeExtension = false;
		bool includePath = false;
		bool allowMove = false;
		bool createFolders = false;
		bool logFileEnabled = true;
		std::wstring regex;
		std::wstring logFilename;
		std::vector<std::wstring> inputFilenames;
	private:
		Jargon::ErrorContext & errorContext;
};


