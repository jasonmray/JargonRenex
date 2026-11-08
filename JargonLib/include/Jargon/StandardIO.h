#pragma once

#include "Jargon/ErrorContext.h"

#include <cstdio>
#include <cstdarg>


namespace Jargon{
namespace StandardIO {

	void setConsoleUTF8();

	int printError(const Jargon::ErrorContext& errorContext);
	int printError(_Printf_format_string_ const wchar_t* formatString, ...);
	int printMessage(_Printf_format_string_ const wchar_t* formatString, ...);
	int printMessage(FILE* destination, _Printf_format_string_ const wchar_t* formatString, ...);
	int printVarArgs(FILE* destination, _Printf_format_string_ const wchar_t* formatString, va_list args);

}
}

