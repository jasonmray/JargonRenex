
#include "ProgramOptions.h"
#include "ProgramVersion.h"

#include "Jargon/FileSystem/Utilities.h"
#include "Jargon/NumberParsing.h"
#include "Jargon/StringUtilities.h"

#include <cassert>
#include <cstdio>


static const char* getBoolString(bool value) {
	return value ? "true" : "false";
}

static const char* getPrintModeString(ProgramOptions::PrintMode value) {
	switch (value) {
		case ProgramOptions::PrintMode_Quiet:
			return "quiet";
		case ProgramOptions::PrintMode_Normal:
			return "normal";
		case ProgramOptions::PrintMode_Verbose:
			return "verbose";
		default:
			return "unknown";
	}
}

class ArgVector {
	public:
		ArgVector(int argc, const char** argv) :
			argc(argc),
			argv(argv),
			currentIndex(0)
		{
		}

		bool hasNext() const {
			return currentIndex + 1 < argc;
		}

		bool moveNext() {
			currentIndex++;
			return isValid();
		}

		bool isValid() const {
			return currentIndex < argc;
		}

		bool isSwitch() const {
			assert(isValid());
			return argv[currentIndex][0] == '-';
		}

		bool isGlob() const {
			assert(isValid());
			return Jargon::StringUtilities::stringContainsAnyOf(argv[currentIndex], "*?");
		}

		const char* getCurrent() const {
			assert(isValid());
			return argv[currentIndex];
		}

		bool matches(const char* s) const {
			return Jargon::StringUtilities::stringEqualCaseInsensitive(s, getCurrent());
		}

		bool matches(const char* shortOption, const char* longOption) const {
			return Jargon::StringUtilities::stringEqualCaseInsensitive(shortOption, getCurrent()) ||
				Jargon::StringUtilities::stringEqualCaseInsensitive(longOption, getCurrent())
			;
		}

	private:
		int argc;
		const char** argv;
		int currentIndex;
};


void ProgramOptions::PrintHelp() {
	printf("\n");
	printf(" Jargon Renex v%s\n", ProgramVersion::VersionString);
	printf("\n");
	printf("    A utility for renaming files with regular expressions.\n");
	printf("    https://github.com/jasonmray/JargonRenex/\n");
	printf("\n");
	printf(" Usage:\n");
	printf("    renex.exe [options] <regex> <source files...>\n");
	printf("\n");
	printf(" Examples:\n");
	printf("    renex.exe \"s/ /-/g\" *.mp4\n");
	printf("    renex.exe \"s/old/New/ig\" *.docx\n");
	printf("    renex.exe \"s/(\\d\\d)-(\\d\\d)-(\\d\\d\\d\\d)/$3-$2-$1/\" *\n");
	printf("    renex.exe \"s/[^\\x20-\\x7e]//g\" *\n");
	printf("    dir /b /s | renex.exe -stdin \"s/(\\d\\d)/0$1/\"\n");
	printf("    dir /b /s | renex.exe -stdin -ext \"s/^(\\d\\d)\\.jpeg$/0$1.jpg/\"\n");
	printf("    renex.exe -allowMove -createFolders \"s/(\\w+)_(\\w+)/$1\\\\$2/ig\" *\n");
	printf("\n");
	printf(" Notes:\n");
	printf("    * Regex format is Perl-compatible, e.g. s/old/new/\n");
	printf("    * Supported regex flags: [gix], e.g s/old/new/gix\n");
	printf("    * By default, regex is only applied to the base filename of each file.\n");
	printf("      Use the -path and -ext options to include file path and/or extension\n");
	printf("      in processing.\n");
	printf("    * By default, no files are renamed. Pass the -y option to rename files,\n");
	printf("      or use -ask to get a confirmation prompt after reviewing the actions\n");
	printf("      to be performed.\n");
	printf("\n");
	printf(" General options:\n");
	printf("    -h,-help               Show help screen and exit\n");
	printf("    -v,-verbose            Verbose mode\n");
	printf("    -q,-quiet              Quiet mode\n");
	printf("    -c,-continueOnError    Continue processing files when certain errors are encountered.\n");
	printf("    -y,-yes                Rename files without confirmation\n");
	printf("    -a,-ask                Ask for confirmation before renaming\n");
	printf("    -e,-ext                Include file extension in regex processing\n");
	printf("    -p,-path               Include path in regex processing\n");
	printf("    -m,-allowMove          Allow moving files to another folder\n");
	printf("    -f,-createFolders      Create any necessary folders to move files\n");
	printf("    -s,-stdin              Read filenames to process from stdin\n");
	printf("    -n,-noLog              Don't create renex.log file\n");
	printf("\n");
	printf("\n");
}

ProgramOptions::ProgramOptions(Jargon::ErrorContext& errorContext):
	errorContext(errorContext)
{
	logFilename = "renex.log";
}

ProgramOptions::~ProgramOptions(){
}

bool ProgramOptions::process(int argc, const char** argv) {
	ArgVector argVector(argc, argv);

	if (argVector.isValid()) {
		exeName = argVector.getCurrent();
	}

	while (argVector.hasNext()) {
		argVector.moveNext();

		if (argVector.matches("/?")) {
			showHelp = true;
		} else if (argVector.isSwitch()) {
			if (argVector.matches("-h", "-help") || argVector.matches("--help") || argVector.matches("-?")) {
				showHelp = true;
			} else if (argVector.matches("-v", "-verbose")) {
				printMode = PrintMode_Verbose;
			} else if (argVector.matches("-q", "-quiet")) {
				printMode = PrintMode_Quiet;
			} else if (argVector.matches("-c", "-continueOnError")) {
				continueOnError = true;
			} else if (argVector.matches("-y", "-yes")) {
				performRenames = true;
			} else if (argVector.matches("-a", "-ask")) {
				askForConfirmation = true;
			} else if (argVector.matches("-e", "-ext")) {
				includeExtension = true;
			} else if (argVector.matches("-p", "-path")) {
				includePath = true;
			} else if (argVector.matches("-m", "-allowMove")) {
				allowMove = true;
			} else if (argVector.matches("-f", "-createFolders")) {
				createFolders = true;
			} else if (argVector.matches("-s", "-stdin")) {
				readStdin = true;
			} else if (argVector.matches("-n", "-nolog")) {
				logFileEnabled = false;
			} else {
				errorContext.setLastError("Unrecognized option %s", argVector.getCurrent());
				return false;
			}
		} else if (regex.empty()){
			regex = argVector.getCurrent();
		} else if (argVector.isGlob()) {
			Jargon::FileSystem::globFiles(argVector.getCurrent(), inputFilenames, false);
		} else {
			inputFilenames.push_back(argVector.getCurrent());
		}
	}

	return true;
}

bool ProgramOptions::verify() const {

	if (inputFilenames.size() == 0 && !readStdin) {
		errorContext.setLastError("Please specify at least one file to rename or use -stdin to read filenames from stdin");
		return false;
	}

	if (readStdin && inputFilenames.size() != 0) {
		errorContext.setLastError("Found command-line file arguments when using -stdin option");
		return false;
	}

	if (regex.empty()) {
		errorContext.setLastError("Please specify a regex pattern");
		return false;
	}

	if (askForConfirmation && performRenames) {
		errorContext.setLastError("Option -yes (-y) conflicts with -prompt (-p).");
		return false;
	}

	return true;
}

void ProgramOptions::printSummary(FILE* destination) const {
	fprintf(destination, "Options:\n");
	fprintf(destination, "  Command: %s\n", exeName.c_str());
	fprintf(destination, "  Console messages: %s\n", getPrintModeString(printMode));
	fprintf(destination, "  Continue on error: %s\n", getBoolString(continueOnError));

	fprintf(destination, "  Actually rename files: %s\n", getBoolString(performRenames));
	fprintf(destination, "  Ask for confirmation before renaming: %s\n", getBoolString(askForConfirmation));
	fprintf(destination, "  Include file extensions in processing: %s\n", getBoolString(includeExtension));
	fprintf(destination, "  Include file paths in processing: %s\n", getBoolString(includePath));
	fprintf(destination, "  Create any necessary folder for renaming: %s\n", getBoolString(createFolders));
	fprintf(destination, "  Write renex.log file: %s\n", getBoolString(logFileEnabled));

	fprintf(destination, "  Regex: %s\n", regex.c_str());

	fprintf(destination, "  Input files: %zd\n", inputFilenames.size());
	for (size_t i = 0; i < inputFilenames.size(); i++) {
		fprintf(destination, "      %zd: %s\n", i, inputFilenames[i].c_str());
	}

	fprintf(destination, "\n");

}

bool ProgramOptions::useVerboseOutput() const {
	return printMode == PrintMode_Verbose;
}

bool ProgramOptions::useNormalOutput() const {
	return printMode >= PrintMode_Normal;
}
