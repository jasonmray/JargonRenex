
#include "JargonRenex.h"

#include "Jargon/BinaryStreamReader.h"
#include "Jargon/TextReader.h"
#include "Jargon/FileSystem/BinaryFileWriter.h"
#include "Jargon/FileSystem/Utilities.h"
#include "Jargon/StreamUtilities.h"
#include "Jargon/StringUtilities.h"

#include <cstdarg>
#include <iostream>
#include <string>


JargonRenex::JargonRenex(const ProgramOptions& programOptions, Jargon::ErrorContext& errorContext) :
	programOptions(programOptions),
	errorContext(errorContext),
	perlReplaceExpression(errorContext),
	regex(errorContext)
{
}

JargonRenex::~JargonRenex(){
}

bool JargonRenex::run() {

	if (!perlReplaceExpression.parse(programOptions.regex)) {
		printError(errorContext);
		return false;
	}

	if (!regex.compile(perlReplaceExpression)) {
		printError(errorContext);
		return false;
	}

	Jargon::RenexResults renexResults;

	if (programOptions.readStdin) {
		if (!Jargon::StreamUtilities::isStdinPiped()) {
			printError("Error: Using -stdin option, but no stdin data was found.\n");
			return false;
		}
		
		Jargon::BinaryStreamReader stdinReader(std::cin);
		Jargon::TextReader stdinTextReader(stdinReader);

		std::string inputLine;
		while (stdinTextReader.readLine(&inputLine)) {
			if (!Jargon::StringUtilities::isEmptyOrWhitespace(inputLine)) {
				if (!processFilenameOrContinue(inputLine, renexResults)) {
					return false;
				}
			}
		}
	} else {
		for (const auto& inputFilename : programOptions.inputFilenames) {
			if (!processFilenameOrContinue(inputFilename, renexResults)) {
				return false;
			}
		}
	}

	if (renexResults.fileRenames.size() > 0) {
		bool performRenames = programOptions.performRenames;

		if (!performRenames) {
			printMessage("\n\nRenames to perform:\n\n");
			for (auto& fileRename : renexResults.fileRenames) {
				printMessage("  %s\n  %s\n\n", fileRename.oldFilename.c_str(), fileRename.newFilename.c_str());
			}
		}

		if (programOptions.askForConfirmation) {
			printMessage("\n\nDo you want to rename these files? [y/N] :");
			int response = getc(stdin);

			performRenames = (response == 'y' || response == 'Y');
		}

		if (performRenames) {
			doRenames(renexResults);
		} else {
			printMessage("\nNo renames performed. Use -yes (-y) or -ask (-a) to rename files\n");
		}
	}

	return true;
}

bool JargonRenex::processFilenameOrContinue(const std::string& inputFilename, Jargon::RenexResults& renexResults) {
	if (!processFilename(inputFilename, renexResults)) {
		if (programOptions.useNormalOutput()) {
			printError(errorContext);
		}

		if (programOptions.continueOnError == false) {
			printError("Use option -continueOnError to proceed past this type of error.\n");
			printError("Stopping.\n");
			return false;
		}
	}
	return true;
}

bool JargonRenex::processFilename(const std::string& inputFilename, Jargon::RenexResults& renexResults) {
	std::string filenameToProcess;
	if (programOptions.includePath) {
		if (!Jargon::FileSystem::canonicalizePathName(inputFilename, filenameToProcess)) {
			filenameToProcess = inputFilename;
		}
	} else {
		filenameToProcess = inputFilename;
	}

	if (programOptions.useVerboseOutput()) {
		printMessage("Processing %s\n", filenameToProcess.c_str());
	}

	std::string processedFilename;

	if (!renex(filenameToProcess, processedFilename)) {
		return false;
	}

	if (filenameToProcess != processedFilename) {
		if (programOptions.useVerboseOutput()) {
			printMessage("  -> %s\n", processedFilename.c_str());
		}

		renexResults.fileRenames.push_back({ filenameToProcess, processedFilename });
	}
	return true;
}

bool JargonRenex::renex(const std::string& inputFilename, std::string& processedFilename) {
	const bool isFolder = Jargon::FileSystem::isDirectory(inputFilename.c_str());

	if (programOptions.includePath) {
		if (programOptions.includeExtension || isFolder) {
			// process entire path & filename with extension
			return regex.replace(perlReplaceExpression, inputFilename, processedFilename);
		} else {
			// process path & filename without extension
			std::string filenameAndPath;
			std::string dottedExtension;

			// extracted filename will include path if present
			Jargon::FileSystem::extractFilenameAndExtension(inputFilename.c_str(), filenameAndPath, dottedExtension);

			// run replacement without the extension
			std::string processedFilenameAndPath;
			if (!regex.replace(perlReplaceExpression, filenameAndPath, processedFilenameAndPath)) {
				return false;
			}

			// append the file extension to the result
			processedFilename.reserve(processedFilenameAndPath.size() + dottedExtension.size());
			processedFilename = processedFilenameAndPath;
			processedFilename += dottedExtension;
		}
	} else {
		if (programOptions.includeExtension || isFolder) {
			// process filename & extension without path

			std::string path;
			std::string filenameAndExt;

			Jargon::FileSystem::extractPathAndFilename(inputFilename.c_str(), path, filenameAndExt);

			// run replacement without the path
			std::string processedFilenameAndExt;
			if (!regex.replace(perlReplaceExpression, filenameAndExt, processedFilenameAndExt)) {
				return false;
			}

			// prepend the path back onto the result
			processedFilename.reserve(path.size() + processedFilenameAndExt.size());
			processedFilename = path;
			processedFilename += processedFilenameAndExt;
		} else {
			// process only base filename

			std::string path;
			std::string filename;
			std::string dottedExtension;

			Jargon::FileSystem::extractPathFilenameAndExtension(inputFilename.c_str(), path, filename, dottedExtension);

			// run replacement without the path or extension
			std::string processedBaseFilename;
			if (!regex.replace(perlReplaceExpression, filename, processedBaseFilename)) {
				return false;
			}

			// rebuild with path & extension
			processedFilename.reserve(path.size() + processedBaseFilename.size() + dottedExtension.size());
			processedFilename = path;
			processedFilename += processedBaseFilename;
			processedFilename += dottedExtension;
		}
	}

	return true;
}

bool JargonRenex::doRenames(const Jargon::RenexResults& renexResults) {

	ActionLog actionLog(programOptions, errorContext);
	if(!actionLog.open()){
		printError(errorContext);
		printError("Stopping.\n");
		return false;
	}

	actionLog.writeLine("-- renames performed -------------------------------");

	for (auto& fileRename : renexResults.fileRenames) {
		if (!doRename(fileRename.oldFilename, fileRename.newFilename, actionLog)) {
			if (programOptions.continueOnError == false) {
				printError("Use option -continueOnError to proceed past this type of error.\n");
				printError("Stopping.\n");
				return false;
			}
		}
	}

	actionLog.writeLine("-- undo commands -----------------------------------");
	for (auto& fileRename : renexResults.fileRenames) {
		actionLog.writeLineFormatted("move \"%s\" \"%s\"", fileRename.newFilename.c_str(), fileRename.oldFilename.c_str());
	}
	return true;
}

bool JargonRenex::doRename(const std::string& oldFilename, const std::string& newFilename, ActionLog& actionLog) {
	if (oldFilename == newFilename) {
		printError("Error: source and destination filename are the same: %s\n", newFilename.c_str());
		return false;
	}

	if (Jargon::StringUtilities::stringContainsAnyOf(newFilename, Jargon::FileSystem::WindowsIllegalFilepathChars)) {
		printError("Failed to rename file to destination. Destination contains illegal characters: %s\n", newFilename.c_str());
		return false;
	}

	if (programOptions.allowMove == false) {
		// ensure old and new filenames have same path
		const std::string_view oldPath = Jargon::FileSystem::getPathComponent(oldFilename.c_str());
		const std::string_view newPath = Jargon::FileSystem::getPathComponent(newFilename.c_str());
		if (oldPath != newPath) {
			printError("Error: Attempting to rename file to a different folder: \"%s\" -> \"%s\"\n", oldFilename.c_str(), newFilename.c_str());
			printError("Specify -allowMove option to move files from their current folder.\n");
			return false;
		}
	}

	if (!Jargon::StringUtilities::stringEqualCaseInsensitive(oldFilename, newFilename)) {
		if (Jargon::FileSystem::fileExists(newFilename.c_str())) {
			printError("Failed to rename file to destination. A file with that name already exists: %s\n", newFilename.c_str());
			return false;
		}
	}

	if (programOptions.createFolders) {
		std::string destinationPath;
		std::string destinationFilename;
		Jargon::FileSystem::extractPathAndFilename(newFilename.c_str(), destinationPath, destinationFilename);

		if (!Jargon::FileSystem::createNestedFolders(destinationPath.c_str())) {
			printError("Failed to create destination folder: %s\n", destinationPath.c_str());
			return false;
		}
	}

	if (!moveFile(oldFilename, newFilename)) {
		printError("Failed to rename file: \"%s\" -> \"%s\"\n", oldFilename.c_str(), newFilename.c_str());
		return false;
	}

	printMessage("Renamed:\n  %s\n  %s\n", oldFilename.c_str(), newFilename.c_str());
	actionLog.writeLineFormatted("move \"%s\" \"%s\"", oldFilename.c_str(), newFilename.c_str());

	return true;
}

bool JargonRenex::moveFile(const std::string& oldFilename, const std::string& newFilename) {
	// todo: this doesn't handle long filenames when they are being processed as relative paths
	std::string oldFilenameQualified = Jargon::FileSystem::addLongPathPrefixIfNeeded(oldFilename);
	std::string newFilenameQualified = Jargon::FileSystem::addLongPathPrefixIfNeeded(newFilename);

	return Jargon::FileSystem::moveFile(oldFilenameQualified.c_str(), newFilenameQualified.c_str(), true);
}

void JargonRenex::printError(const Jargon::ErrorContext& errorContext) {
	if (errorContext.getLastError() != nullptr) {
		fprintf(stderr, "Error: %s\n", errorContext.getLastError());
	}
}

void JargonRenex::printError(const char* formatString, ...) {
	va_list args;
	va_start(args, formatString);
	printVarArgs(stderr, formatString, args);
	va_end(args);
}

void JargonRenex::printMessage(const char* formatString, ...) {
	va_list args;
	va_start(args, formatString);
	printVarArgs(stdout, formatString, args);
	va_end(args);
}

void JargonRenex::printVarArgs(FILE* destination, const char* formatString, va_list args) {
	std::string errorMessage = Jargon::StringUtilities::formatVarArgs(formatString, args);
	fprintf(destination, "%s", errorMessage.c_str());
}
