#pragma once

#include "Jargon/ErrorContext.h"
#include "Jargon/FileSystem/WindowsFileFinder.h"

#include <string>


namespace Jargon{
namespace FileSystem{


	class DirectoryListingWide : public ErrorContext {
		public:
		DirectoryListingWide();
		~DirectoryListingWide();

		bool open(const wchar_t* path);
		bool open(const std::wstring& path);

		const std::wstring& getPath() const;

		// returns false on error or if there is no current file.
		// check hasError() to see if an error occurred.
		bool getCurrentFilename(std::wstring& filenameOut);

		// same as getCurrentFilename(), but prepends the value of getPath()
		bool getCurrentFilenameWithPath(std::wstring& filepathOut);

		// returns false on error or if there is no next file.
		// check hasError() to see if an error occurred.
		bool advance();

		void close();
		private:
		std::wstring path;
		Jargon::FileSystem::WindowsFileFinder fileFinder;
	};


}
}

