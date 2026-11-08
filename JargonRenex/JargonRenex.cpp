
#include "JargonRenex.h"

#include "Jargon/BinaryStreamReader.h"
#include "Jargon/TextReader.h"
#include "Jargon/FileSystem/BinaryFileWriter.h"
#include "Jargon/FileSystem/Utilities.h"
#include "Jargon/StandardIO.h"
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
		Jargon::StandardIO::printError(errorContext);
		return false;
	}

	if (!regex.compile(perlReplaceExpression)) {
		Jargon::StandardIO::printError(errorContext);
		return false;
	}

	Jargon::RenexResults renexResults;

	if (programOptions.readStdin) {
		if (!Jargon::StreamUtilities::isStdinPiped()) {
			Jargon::StandardIO::printError(L"Error: Using -stdin option, but no stdin data was found.\n");
			return false;
		}
		
		Jargon::BinaryStreamReader stdinReader(std::cin);
		Jargon::TextReader stdinTextReader(stdinReader);

		std::string inputLine;
		while (stdinTextReader.readLine(&inputLine)) {
			std::wstring inputLineWide = Jargon::StringUtilities::utf8ToWide(inputLine);
			if (!Jargon::StringUtilities::isEmptyOrWhitespace(inputLine)) {
				if (!processFilenameOrContinue(inputLineWide, renexResults)) {
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
			Jargon::StandardIO::printMessage(L"\n\nRenames to perform:\n\n");
			for (auto& fileRename : renexResults.fileRenames) {
				Jargon::StandardIO::printMessage(L"  %ls\n  %ls\n\n", fileRename.oldFilename.c_str(), fileRename.newFilename.c_str());
			}
		}

		if (programOptions.askForConfirmation) {
			Jargon::StandardIO::printMessage(L"\n\nDo you want to rename these files? [y/N] :");
			int response = getc(stdin);

			performRenames = (response == 'y' || response == 'Y');
		}

		if (performRenames) {
			doRenames(renexResults);
		} else {
			Jargon::StandardIO::printMessage(L"\nNo renames performed. Use -yes (-y) or -ask (-a) to rename files\n");
		}
	}

	return true;
}

bool JargonRenex::processFilenameOrContinue(const std::wstring& inputFilename, Jargon::RenexResults& renexResults) {
	if (!processFilename(inputFilename, renexResults)) {
		if (programOptions.useNormalOutput()) {
			Jargon::StandardIO::printError(errorContext);
		}

		if (programOptions.continueOnError == false) {
			Jargon::StandardIO::printError(L"Use option -continueOnError to proceed past this type of error.\n");
			Jargon::StandardIO::printError(L"Stopping.\n");
			return false;
		}
	}
	return true;
}

bool JargonRenex::processFilename(const std::wstring& inputFilename, Jargon::RenexResults& renexResults) {
	std::wstring filenameToProcess;
	if (programOptions.includePath) {
		if (!Jargon::FileSystem::canonicalizePathName(inputFilename, filenameToProcess)) {
			filenameToProcess = inputFilename;
		}
	} else {
		filenameToProcess = inputFilename;
	}

	if (programOptions.useVerboseOutput()) {
		Jargon::StandardIO::printMessage(L"Processing %ls\n", filenameToProcess.c_str());
	}

	std::wstring processedFilename;

	if (!renex(filenameToProcess, processedFilename)) {
		return false;
	}

	if (filenameToProcess != processedFilename) {
		if (programOptions.useVerboseOutput()) {
			Jargon::StandardIO::printMessage(L"  -> %ls\n", processedFilename.c_str());
		}

		renexResults.fileRenames.push_back({ filenameToProcess, processedFilename });
	}
	return true;
}

bool JargonRenex::renex(const std::wstring& inputFilename, std::wstring& processedFilename) {
	const bool isFolder = Jargon::FileSystem::isDirectory(inputFilename.c_str());

	if (programOptions.includePath) {
		if (programOptions.includeExtension || isFolder) {
			// process entire path & filename with extension
			return regex.replace(perlReplaceExpression, inputFilename, processedFilename);
		} else {
			// process path & filename without extension
			std::wstring filenameAndPath;
			std::wstring dottedExtension;

			// extracted filename will include path if present
			Jargon::FileSystem::extractFilenameAndExtension(inputFilename.c_str(), filenameAndPath, dottedExtension);

			// run replacement without the extension
			std::wstring processedFilenameAndPath;
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

			std::wstring path;
			std::wstring filenameAndExt;

			Jargon::FileSystem::extractPathAndFilename(inputFilename.c_str(), path, filenameAndExt);

			// run replacement without the path
			std::wstring processedFilenameAndExt;
			if (!regex.replace(perlReplaceExpression, filenameAndExt, processedFilenameAndExt)) {
				return false;
			}

			// prepend the path back onto the result
			processedFilename.reserve(path.size() + processedFilenameAndExt.size());
			processedFilename = path;
			processedFilename += processedFilenameAndExt;
		} else {
			// process only base filename

			std::wstring path;
			std::wstring filename;
			std::wstring dottedExtension;

			Jargon::FileSystem::extractPathFilenameAndExtension(inputFilename.c_str(), path, filename, dottedExtension);

			// run replacement without the path or extension
			std::wstring processedBaseFilename;
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
		Jargon::StandardIO::printError(errorContext);
		Jargon::StandardIO::printError(L"Stopping.\n");
		return false;
	}

	actionLog.writeLine("-- renames performed -------------------------------");

	for (auto& fileRename : renexResults.fileRenames) {
		if (!doRename(fileRename.oldFilename, fileRename.newFilename, actionLog)) {
			if (programOptions.continueOnError == false) {
				Jargon::StandardIO::printError(L"Use option -continueOnError to proceed past this type of error.\n");
				Jargon::StandardIO::printError(L"Stopping.\n");
				return false;
			}
		}
	}

	actionLog.writeLine("-- undo commands -----------------------------------");
	for (auto& fileRename : renexResults.fileRenames) {
		const std::string newFilenameUtf8 = Jargon::StringUtilities::wideToUtf8(fileRename.newFilename);
		const std::string oldFilenameUtf8 = Jargon::StringUtilities::wideToUtf8(fileRename.oldFilename);
		actionLog.writeLineFormatted("move \"%hs\" \"%hs\"", newFilenameUtf8.c_str(), oldFilenameUtf8.c_str());
	}
	return true;
}

bool JargonRenex::doRename(const std::wstring& oldFilename, const std::wstring& newFilename, ActionLog& actionLog) {
	if (oldFilename == newFilename) {
		Jargon::StandardIO::printError(L"Error: source and destination filename are the same: %ls\n", newFilename.c_str());
		return false;
	}

	if (Jargon::StringUtilities::stringContainsAnyOf(newFilename, Jargon::FileSystem::WindowsIllegalFilepathCharsWide)) {
		Jargon::StandardIO::printError(L"Failed to rename file to destination. Destination contains illegal characters: %ls\n", newFilename.c_str());
		return false;
	}

	if (programOptions.allowMove == false) {
		// ensure old and new filenames have same path
		const std::wstring_view oldPath = Jargon::FileSystem::getPathComponent(oldFilename.c_str());
		const std::wstring_view newPath = Jargon::FileSystem::getPathComponent(newFilename.c_str());
		if (oldPath != newPath) {
			Jargon::StandardIO::printError(L"Error: Attempting to rename file to a different folder: \"%ls\" -> \"%ls\"\n", oldFilename.c_str(), newFilename.c_str());
			Jargon::StandardIO::printError(L"Specify -allowMove option to move files from their current folder.\n");
			return false;
		}
	}

	if (!Jargon::StringUtilities::stringEqualCaseInsensitive(oldFilename, newFilename)) {
		if (Jargon::FileSystem::fileExists(newFilename.c_str())) {
			Jargon::StandardIO::printError(L"Failed to rename file to destination. A file with that name already exists: %ls\n", newFilename.c_str());
			return false;
		}
	}

	if (programOptions.createFolders) {
		std::wstring destinationPath;
		std::wstring destinationFilename;
		Jargon::FileSystem::extractPathAndFilename(newFilename.c_str(), destinationPath, destinationFilename);

		if (!Jargon::FileSystem::createNestedFolders(destinationPath.c_str())) {
			Jargon::StandardIO::printError(L"Failed to create destination folder: %ls\n", destinationPath.c_str());
			return false;
		}
	}

	if (!moveFile(oldFilename, newFilename)) {
		Jargon::StandardIO::printError(L"Failed to rename file: \"%ls\" -> \"%ls\"\n", oldFilename.c_str(), newFilename.c_str());
		return false;
	}

	Jargon::StandardIO::printMessage(L"Renamed:\n  %ls\n  %ls\n", oldFilename.c_str(), newFilename.c_str());

	const std::string newFilenameUtf8 = Jargon::StringUtilities::wideToUtf8(newFilename);
	const std::string oldFilenameUtf8 = Jargon::StringUtilities::wideToUtf8(oldFilename);
	actionLog.writeLineFormatted("move \"%hs\" \"%hs\"", oldFilenameUtf8.c_str(), newFilenameUtf8.c_str());

	return true;
}

bool JargonRenex::moveFile(const std::wstring& oldFilename, const std::wstring& newFilename) {
	// todo: this doesn't handle long filenames when they are being processed as relative paths
	std::wstring oldFilenameQualified = Jargon::FileSystem::addLongPathPrefixIfNeeded(oldFilename);
	std::wstring newFilenameQualified = Jargon::FileSystem::addLongPathPrefixIfNeeded(newFilename);

	return Jargon::FileSystem::moveFile(oldFilenameQualified.c_str(), newFilenameQualified.c_str(), true);
}
