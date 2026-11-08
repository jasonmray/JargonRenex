#include "Jargon/StandardIO.h"
#include "Jargon/StringUtilities.h"

#include <windows.h>


namespace Jargon{
namespace StandardIO {

	void setConsoleUTF8() {
		SetConsoleCP(CP_UTF8);
		SetConsoleOutputCP(CP_UTF8);
	}

	int printError(const Jargon::ErrorContext& errorContext) {
		if (errorContext.getLastError() != nullptr) {
			return fwprintf(stderr, L"Error: %hs\n", errorContext.getLastError());
		}

		return 0;
	}

	int printError(_Printf_format_string_ const wchar_t* formatString, ...) {
		va_list args;
		va_start(args, formatString);
		const int returnVal = printVarArgs(stderr, formatString, args);
		va_end(args);

		return returnVal;
	}

	int printMessage(_Printf_format_string_ const wchar_t* formatString, ...) {
		va_list args;
		va_start(args, formatString);
		const int returnVal = printVarArgs(stdout, formatString, args);
		va_end(args);

		return returnVal;
	}

	int printMessage(FILE* destination, _Printf_format_string_ const wchar_t* formatString, ...) {
		va_list args;
		va_start(args, formatString);
		const int returnVal = printVarArgs(destination, formatString, args);
		va_end(args);

		return returnVal;
	}

	int printVarArgs(FILE* destination, _Printf_format_string_ const wchar_t* formatString, va_list args) {
		std::wstring message = Jargon::StringUtilities::formatWideVarArgs(formatString, args);
		std::string messageUtf8 = Jargon::StringUtilities::wideToUtf8(message);
		return fputs(messageUtf8.c_str(), destination);
	}
}
}
