#pragma once

#include "Jargon/ErrorContext.h"
#include "Jargon/ScopedPointer.h"

#include <cstdint>
#include <string>
#include <string_view>


namespace Jargon{
namespace FileSystem{

	class WindowsFileFinder : public ErrorContext {
		public:
			WindowsFileFinder();
			~WindowsFileFinder();

			bool start(const char* pathExpression);
			bool start(const std::string& pathExpression);
			bool start(const std::string_view& pathExpression);

			bool start(const wchar_t* pathExpression);
			bool start(const std::wstring& pathExpression);
			bool start(const std::wstring_view& pathExpression);

			void stop();

			// returns false on error or if there is no current file.
			// check hasError() to see if an error occurred.
			bool getCurrentFilename(std::string& filenameOut);
			bool getCurrentFilename(std::wstring& filenameOut);

			// returns false on error or if there is no next file.
			// check hasError() to see if an error occurred.
			bool advance();

			uint32_t getLastErrorCode() const;

		private:
			class WindowsFileFinderImpl;
			Jargon::ScopedPointer<WindowsFileFinderImpl> impl;
};

}
}

