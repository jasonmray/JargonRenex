#include "Jargon/FileSystem/DirectoryListingWide.h"
#include "Jargon/FileSystem/Utilities.h"
#include "Jargon/StringUtilities.h"


namespace Jargon{
namespace FileSystem{

	DirectoryListingWide::DirectoryListingWide() {
	}

	DirectoryListingWide::~DirectoryListingWide() {
		close();
	}

	bool DirectoryListingWide::open(const wchar_t* path) {
		std::wstring findExpression;
		if (!Jargon::FileSystem::canonicalizePathName(path, findExpression)) {
			setLastError("failed to canonicalize path");
			return false;
		}

		Jargon::FileSystem::ensureTrailingSlash(findExpression);
		this->path = findExpression;

		findExpression += L"*";

		if (!fileFinder.start(findExpression)) {
			setLastErrorFrom(fileFinder);
			return false;
		}

		return true;
	}

	bool DirectoryListingWide::open(const std::wstring& path) {
		return open(path.c_str());
	}

	const std::wstring& DirectoryListingWide::getPath() const {
		return path;
	}

	bool DirectoryListingWide::getCurrentFilename(std::wstring& filenameOut) {
		return fileFinder.getCurrentFilename(filenameOut);
	}

	bool DirectoryListingWide::getCurrentFilenameWithPath(std::wstring& filepathOut) {
		std::wstring filename;
		if (!getCurrentFilename(filename)) {
			return false;
		}

		filepathOut = getPath();
		filepathOut += filename;

		return true;
	}

	bool DirectoryListingWide::advance() {
		return fileFinder.advance();
	}

	void DirectoryListingWide::close() {
		fileFinder.stop();
		path.clear();
	}

}
}
