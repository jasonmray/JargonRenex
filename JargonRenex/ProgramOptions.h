#pragma once

#include "Jargon/ErrorContext.h"

#include <cstdio>
#include <string>
#include <vector>

class ProgramOptions{
	public:

		enum PrintMode {
			PrintMode_Quiet   = 0,
			PrintMode_Normal  = 1,
			PrintMode_Verbose = 2
		};

		static void PrintHelp();

		ProgramOptions(Jargon::ErrorContext& errorContext);
		~ProgramOptions();

		bool process(int argc, const char** argv);
		bool verify() const;
		void printSummary(FILE* destination) const;

		bool useVerboseOutput() const;
		bool useNormalOutput() const;

		std::string exeName;
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
		std::string regex;
		std::string logFilename;
		std::vector<std::string> inputFilenames;
	private:
		Jargon::ErrorContext & errorContext;
};


