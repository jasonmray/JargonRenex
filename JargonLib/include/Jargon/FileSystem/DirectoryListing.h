#pragma once

#include "Jargon/ErrorContext.h"
#include "Jargon/FileSystem/WindowsFileFinder.h"

#include <string>
#include <string_view>


namespace Jargon{
namespace FileSystem{

	class DirectoryListing : public ErrorContext {
		public:
			DirectoryListing();
			~DirectoryListing();

			bool open(const char* path);
			bool open(const std::string& path);
			bool open(const std::string_view& path);

			const std::string& getPath() const;

			// returns false on error or if there is no current file.
			// check hasError() to see if an error occurred.
			bool getCurrentFilename(std::string& filenameOut);

			// same as getCurrentFilename(), but prepends the value of getPath()
			bool getCurrentFilenameWithPath(std::string& filepathOut);

			// returns false on error or if there is no next file.
			// check hasError() to see if an error occurred.
			bool advance();

			void close();
		private:
			std::string path;
			Jargon::FileSystem::WindowsFileFinder fileFinder;
	};

}
}

