#include "Jargon/FileSystem/WindowsFileFinder.h"
#include "Jargon/FileSystem/Utilities.h"
#include "Jargon/StringUtilities.h"
#include "Jargon/System/WindowsUtilities.h"

#include "Jargon/System/WindowsDefines.h"
#include <windows.h>


namespace Jargon{
namespace FileSystem{

	class WindowsFileFinder::WindowsFileFinderImpl {
		public:
			WindowsFileFinderImpl(Jargon::ErrorContext& errorContext) :
				errorContext(errorContext)
			{
				findData = {};
				findHandle = INVALID_HANDLE_VALUE;
				lastErrorCode = NO_ERROR;
			}

			~WindowsFileFinderImpl() {
				stop();
			}

			bool start(const std::string_view& pathExpression) {
				stop();

				pathExpressionWide = Jargon::StringUtilities::utf8ToWide(pathExpression);

				findHandle = ::FindFirstFileW(pathExpressionWide.c_str(), &findData);

				if (findHandle == INVALID_HANDLE_VALUE) {
					DWORD error = GetLastError();
					if (error != ERROR_NO_MORE_FILES) {
						setWindowsError("Directory read failed", error);
						return false;
					}
				} else {
					skipDotDirectories();
				}

				return true;
			}

			bool start(const std::wstring_view& pathExpression) {
				stop();

				pathExpressionWide = pathExpression;

				findHandle = ::FindFirstFileW(pathExpressionWide.c_str(), &findData);

				if (findHandle == INVALID_HANDLE_VALUE) {
					DWORD error = GetLastError();
					if (error != ERROR_NO_MORE_FILES) {
						setWindowsError("Directory read failed", error);
						return false;
					}
				} else {
					skipDotDirectories();
				}

				return true;
			}

			void stop() {
				if (findHandle != INVALID_HANDLE_VALUE) {
					FindClose(findHandle);
				}

				findData = {};
				findHandle = INVALID_HANDLE_VALUE;
				lastErrorCode = NO_ERROR;
				errorContext.clearLastError();
			}

			bool getCurrentFilename(std::string& filenameOut) {
				if (findHandle == INVALID_HANDLE_VALUE) {
					return false;
				}
				filenameOut = Jargon::StringUtilities::wideToUtf8(findData.cFileName);
				return true;
			}

			bool getCurrentFilename(std::wstring& filenameOut) {
				if (findHandle == INVALID_HANDLE_VALUE) {
					return false;
				}
				filenameOut = findData.cFileName;
				return true;
			}

			bool advance() {
				if (findHandle == INVALID_HANDLE_VALUE) {
					return false;
				}

				if (!findNextFile()) {
					return false;
				}

				if (!skipDotDirectories()) {
					return false;
				}

				return true;
			}

			DWORD getLastErrorCode() const {
				return lastErrorCode;
			}

		private:
			Jargon::ErrorContext& errorContext;
			WIN32_FIND_DATAW findData;
			HANDLE findHandle;
			DWORD lastErrorCode;
			std::wstring pathExpressionWide;

			bool skipDotDirectories() {
				if (findHandle == INVALID_HANDLE_VALUE) {
					return false;
				}

				while (Jargon::StringUtilities::stringEqual(L".", findData.cFileName) || Jargon::StringUtilities::stringEqual(L"..", findData.cFileName)) {
					if (!findNextFile()) {
						return false;
					}
				}

				return true;
			}

			bool findNextFile() {
				if (FindNextFileW(findHandle, &findData) == 0) {
					const DWORD error = GetLastError();

					if (error != ERROR_NO_MORE_FILES) {
						setWindowsError("Directory read failed", error);
					}

					if (findHandle != INVALID_HANDLE_VALUE) {
						FindClose(findHandle);
					}

					findData = {};
					findHandle = INVALID_HANDLE_VALUE;

					return false;
				}

				return true;
			}

			void setWindowsError(const char* message, DWORD errorCode) {
				lastErrorCode = errorCode;
				std::string systemMessage = Jargon::System::getWindowsErrorMessage(errorCode);
				errorContext.setLastError("%s on %S : %s", message, Jargon::FileSystem::skipLongPathPrefix(pathExpressionWide), systemMessage.c_str());
			}
	};

	WindowsFileFinder::WindowsFileFinder() {
		impl = new WindowsFileFinderImpl(*this);
	}

	WindowsFileFinder::~WindowsFileFinder() {
	}

	bool WindowsFileFinder::start(const char* pathExpression) {
		return start(std::string_view(pathExpression));
	}

	bool WindowsFileFinder::start(const std::string& pathExpression) {
		return start(std::string_view(pathExpression));
	}

	bool WindowsFileFinder::start(const std::string_view& pathExpression){
		return impl->start(pathExpression);
	}

	bool WindowsFileFinder::start(const wchar_t* pathExpression) {
		return start(std::wstring_view(pathExpression));
	}

	bool WindowsFileFinder::start(const std::wstring& pathExpression) {
		return start(std::wstring_view(pathExpression));
	}

	bool WindowsFileFinder::start(const std::wstring_view& pathExpression) {
		return impl->start(pathExpression);
	}

	void WindowsFileFinder::stop() {
		impl->stop();
	}

	bool WindowsFileFinder::getCurrentFilename(std::string& filenameOut) {
		return impl->getCurrentFilename(filenameOut);
	}

	bool WindowsFileFinder::getCurrentFilename(std::wstring& filenameOut) {
		return impl->getCurrentFilename(filenameOut);
	}

	bool WindowsFileFinder::advance() {
		return impl->advance();
	}

	uint32_t WindowsFileFinder::getLastErrorCode() const {
		return (uint32_t)impl->getLastErrorCode();
	}

}
}
