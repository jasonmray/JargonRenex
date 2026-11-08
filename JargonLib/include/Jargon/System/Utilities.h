
#ifndef JARGON_SYSTEM_UTILITIES_H
#define JARGON_SYSTEM_UTILITIES_H

#include <string>
#include <vector>

namespace Jargon{
namespace System{

	void waitForDebugger();
	void sleep(unsigned int milliseconds);
	unsigned int getHardwareConcurrencyCount();

	// open Windows Explorer to the given file
	void showFileInExplorer(const char * path);

	// open Windows Explorer to the given file within a zip file
	//    zipFilepath            : filesystem path to .zip file
	//    zippedFilePath         : internal zip path to file
	//    allowPartialNavigation : don't fail if zippedFilePath is not found,
	//       falling back to a parent folder within the zip.
	void showZippedFileInExplorer(const char* zipFilepath, const char* zippedFilePath, bool allowPartialNavigation);

	void notifyDisplayInUse(bool inUse);

	std::string getClockTimeForCurrentUserLocale();
	std::string getUserScreenshotsFolderPath();

	// gets path & exe name of current application
	bool getApplicationPath(std::string* pathOut);

	// gets folder location of current application
	bool getApplicationFolder(std::string* folderOut);

	std::string getHumanReadableSizeBytes(uint64_t sizeBytes);
	void getHumanReadableSizeBytes(uint64_t sizeBytes, std::string& stringOut);
}
}

#endif
