#include "JargonRenex.h"
#include "ProgramOptions.h"
#include "ProgramVersion.h"

#include "Jargon/ErrorContext.h"

#include <cstdio>


int main(int argc, const char ** argv){
	Jargon::ErrorContext errorContext;
	ProgramOptions programOptions(errorContext);

	if (!programOptions.process(argc, argv)) {
		fprintf(stderr, "Error: %s\n", errorContext.getLastError());
		return 1;
	}

	if (programOptions.showHelp || argc == 1) {
		ProgramOptions::PrintHelp();
		return 1;
	}

	if (!programOptions.verify()) {
		fprintf(stderr, "Error: %s\n", errorContext.getLastError());
		return 1;
	}

	if (programOptions.useVerboseOutput()) {
		// print version banner and final program options
		fprintf(stdout, "\nJargon Renex v%s\n\n", ProgramVersion::VersionString);
		programOptions.printSummary(stdout);
	}

	JargonRenex app(programOptions, errorContext);
	if (!app.run()) {
		return 1;
	}

	return 0;
}
