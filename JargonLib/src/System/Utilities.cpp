#include "Jargon/System/Utilities.h"
#include "Jargon/StringUtilities.h"
#include "Jargon/FileSystem/Utilities.h"

#ifdef _WIN32
	#include "Jargon/System/WindowsDefines.h"
	#include "Jargon/System/ComUtilities.h"
	#include <windows.h>
	#include <shlobj.h>
	#include <shlobj_core.h>
	#include <shlwapi.h>
	#include <WinBase.h>
	#include <debugapi.h>
#endif

#include <algorithm>
#include <cassert>
#include <thread>

namespace Jargon{
namespace System{

	#ifdef _WIN32

		void waitForDebugger() {
			while (!IsDebuggerPresent()) {
				sleep(100);
			}
		}

		void sleep(unsigned int milliseconds){
			Sleep(milliseconds);
		}

		unsigned int getHardwareConcurrencyCount(){
			unsigned int count = std::thread::hardware_concurrency();
			if (count == 0) {
				count = 1;
			}
			return count;
		}

		void showFileInExplorer(const char * path) {
			std::wstring widePath = Jargon::StringUtilities::utf8ToWide(path);
			std::replace(widePath.begin(), widePath.end(), '/', '\\');

			// if the path begins with "\\?\" for windows long-path support, SHParseDisplayName
			// will fail with E_INVALIDARG. advance past that prefix if it is present.
			const wchar_t* displayNameToParse = Jargon::FileSystem::skipLongPathPrefix(widePath);

			PIDLIST_ABSOLUTE pidl = 0;
			SFGAOF flags = 0;
			HRESULT result = SHParseDisplayName(displayNameToParse, NULL, &pidl, 0, &flags);
			try {
				// Open Explorer and select the thing
				SHOpenFolderAndSelectItems(pidl, 0, NULL, 0);
			} catch(...) {
				// Use the task allocator to free to returned pidl
				CoTaskMemFree(pidl);
			}
		}

		static bool SelectZippedFileInExplorer(const std::wstring& zipPath, const std::wstring& relativeFilePath, bool allowPartialNavigation = true) {
			try {
				Jargon::System::ComInitializer comInitializer(COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

				if (!comInitializer) {
					return false;
				}

				std::vector<std::wstring> pathSegments;
				Jargon::StringUtilities::tokenizeString(relativeFilePath, L"/\\", pathSegments);
				if (pathSegments.empty()) {
					return false;
				}

				HRESULT hr = S_OK;

				// Get the absolute PIDL for the zip file
				CoTaskMemPtr<PIDLIST_ABSOLUTE> pidlZipRoot;
				JARGON_RETURN_FALSE_IF_HR_FAIL(SHParseDisplayName(zipPath.c_str(), nullptr, pidlZipRoot.addressOf(), 0, nullptr));

				ComPtr<IShellFolder> pDesktop;
				JARGON_RETURN_FALSE_IF_HR_FAIL(SHGetDesktopFolder(pDesktop.addressOf()));

				// The current folder we are working within (starts at zip root)
				ComPtr<IShellFolder> pCurrentFolder;
				JARGON_RETURN_FALSE_IF_HR_FAIL(pDesktop->BindToObject(pidlZipRoot, nullptr, IID_PPV_ARGS(pCurrentFolder.addressOf())));

				// The fully qualified PIDL of the parent folder as we descend
				CoTaskMemPtr<PIDLIST_ABSOLUTE> pidlCurrentParentFullyQualified;
				pidlCurrentParentFullyQualified = ILCloneFull(pidlZipRoot);

				if (!pidlCurrentParentFullyQualified) {
					return false;
				}

				bool navigationFailed = false;
				CoTaskMemPtr<PITEMID_CHILD> pidlLastChildRelative;

				// process segments of the internal path
				for (size_t i = 0; i < pathSegments.size(); ++i) {
					CoTaskMemPtr<PITEMID_CHILD> pidlChildRelative;

					// parse the single segment inside the current folder context
					hr = pCurrentFolder->ParseDisplayName(
						nullptr,
						nullptr,
						const_cast<LPWSTR>(pathSegments[i].c_str()),
						nullptr,
						pidlChildRelative.addressOf(),
						nullptr
					);

					if (FAILED(hr)) {
						navigationFailed = true;
						break;
					}

					// if this is not the last item, it's a folder we need to enter
					if (i < pathSegments.size() - 1) {
						ComPtr<IShellFolder> pNextFolder;
						hr = pCurrentFolder->BindToObject(pidlChildRelative, nullptr, IID_PPV_ARGS(pNextFolder.addressOf()));

						if (FAILED(hr)) {
							navigationFailed = true;
							break;
						}

						// append the child PIDL
						pidlCurrentParentFullyQualified = ILCombine(pidlCurrentParentFullyQualified, pidlChildRelative);

						// descend into the next subfolder level
						pCurrentFolder = pNextFolder;
					}

					pidlLastChildRelative = pidlChildRelative;
				}

				if (navigationFailed == true && allowPartialNavigation == false) {
					return false;
				}

				if (pidlLastChildRelative) {
					LPCITEMIDLIST arrPidl = { pidlLastChildRelative.getRaw() };

					// Open explorer to the target file within the zip file.
					JARGON_RETURN_FALSE_IF_HR_FAIL(SHOpenFolderAndSelectItems(pidlCurrentParentFullyQualified, 1, &arrPidl, 0));

					return true;
				}
			} catch (...) {
			}

			return false;
		}

		void showZippedFileInExplorer(const char* zipFilepath, const char* zippedFilePath, bool allowPartialNavigation) {
			const std::wstring zipFilepathWide = Jargon::StringUtilities::utf8ToWide(zipFilepath);
			const std::wstring zippedFilePathWide = Jargon::StringUtilities::utf8ToWide(zippedFilePath);

			SelectZippedFileInExplorer(zipFilepathWide, zippedFilePathWide);
		}

		void notifyDisplayInUse(bool inUse) {
			if (inUse) {
				SetThreadExecutionState(ES_CONTINUOUS | ES_DISPLAY_REQUIRED);
			} else {
				SetThreadExecutionState(ES_CONTINUOUS);
			}
		}

		std::string getClockTimeForCurrentUserLocale() {
			wchar_t buff[128] = { 0 };
			GetTimeFormat(LOCALE_USER_DEFAULT, 0, NULL, NULL, buff, 128);
			return Jargon::StringUtilities::wideToUtf8(buff);
		}

		std::string getUserScreenshotsFolderPath() {
			DWORD flags = 0;
			PWSTR widePath;

			HRESULT result = SHGetKnownFolderPath(FOLDERID_Screenshots, 0, NULL, &widePath);

			if (result != S_OK) {
				result = SHGetKnownFolderPath(FOLDERID_SavedPictures, 0, NULL, &widePath);
			}

			if (result != S_OK) {
				result = SHGetKnownFolderPath(FOLDERID_CameraRoll, 0, NULL, &widePath);
			}

			if (result != S_OK) {
				result = SHGetKnownFolderPath(FOLDERID_Pictures, 0, NULL, &widePath);
			}
			
			if (result != S_OK) {
				result = SHGetKnownFolderPath(FOLDERID_Documents, 0, NULL, &widePath);
			}

			if (result != S_OK) {
				result = SHGetKnownFolderPath(FOLDERID_Desktop, 0, NULL, &widePath);
			}

			std::string path = Jargon::StringUtilities::wideToUtf8(widePath);
			CoTaskMemFree(widePath);

			return path;
		}

		bool getApplicationPath(std::string* pathOut) {
			wchar_t path[1024];
			int numCharsFilled = GetModuleFileNameW(nullptr, path, sizeof(path) / sizeof(wchar_t));

			if(numCharsFilled == 0){
				return false;
			}

			*pathOut = Jargon::StringUtilities::wideToUtf8(path);
			return true;
		}

		bool getApplicationFolder(std::string* folderOut) {
			std::string applicationPath;
			if (!getApplicationPath(&applicationPath)) {
				return false;
			}

			*folderOut = Jargon::FileSystem::getPathComponent(applicationPath.c_str());
			return true;
		}

	#endif

		std::string getHumanReadableSizeBytes(uint64_t sizeBytes) {
			const char prefixes[] = "BKMGTPE";
			const char maxPrefixIndex = 6;

			int magnitude = 0;
			uint64_t value = sizeBytes;
			uint64_t denominator = 1;
			while (value >= 1024 && magnitude <= maxPrefixIndex) {
				value /= 1024;
				denominator *= 1024;
				magnitude++;
			}

			// note: this effectively truncates the fractional part instead of rounding,
			// but the result matches e.g. Windows Explorer.
			const uint64_t remainder = ((sizeBytes * 100) / denominator) % 100;

			const char prefix = prefixes[magnitude];

			if (magnitude == 0) {
				return Jargon::StringUtilities::format("%d %c", (int)value, prefix);
			} else {
				return Jargon::StringUtilities::format("%d.%02d %cB", (int)value, (int)remainder, prefix);
			}
		}

		void getHumanReadableSizeBytes(uint64_t sizeBytes, std::string& stringOut) {
			stringOut = getHumanReadableSizeBytes(sizeBytes);
		}
}
}
