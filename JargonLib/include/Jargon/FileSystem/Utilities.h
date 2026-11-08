#pragma once

#include "Jargon/Buffer.h"

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>


namespace Jargon{
namespace FileSystem{

	extern const int WindowsMaxPathLength;

	extern const char* WindowsLongNamePrefix;
	extern const wchar_t* WindowsLongNamePrefixWide;

	extern const char* WindowsIllegalFilepathChars;
	extern const wchar_t* WindowsIllegalFilepathCharsWide;

	extern const char* WindowsIllegalFilenameChars;
	extern const wchar_t* WindowsIllegalFilenameCharsWide;

	bool filenameHasExtension(const char* filename, const char* extension);

	enum FileType {
		Unknown,
		File,
		Folder,
		Other
	};
	bool getFileType(const char* filename, FileType& fileTypeOut);
	bool getFileType(const wchar_t* filename, FileType& fileTypeOut);

	bool fileExists(const char* filename);
	bool fileExists(const wchar_t* filename);
	bool isDirectory(const char* filename);
	bool isDirectory(const char* filename, bool& existsFlagOut);
	bool isDirectory(const wchar_t* filename);
	bool isDirectory(const wchar_t* filename, bool& existsFlagOut);

	bool getFileSize(const char* filename, int64_t* sizeOut);
	bool getFileSize(const wchar_t* filename, int64_t* sizeOut);

	bool getFileModifiedTimeUnixEpochSeconds(const char* filename, int64_t* unixSecondsOut);
	bool getFileModifiedTimeUnixEpochSeconds(const wchar_t* filename, int64_t* unixSecondsOut);

	bool getFileHiddenAttribute(const char* filename, bool* hiddenFlagOut);
	bool getFileHiddenAttribute(const wchar_t* filename, bool* hiddenFlagOut);

	std::string addLongPathPrefixIfNeeded(const std::string& filename);
	std::wstring addLongPathPrefixIfNeeded(const std::wstring& filename);

	void ensureTrailingSlash(std::string& path);
	void ensureTrailingSlash(std::wstring& path);

	std::string ensureLongPathPrefix(const char* filename);
	std::string ensureLongPathPrefix(const std::string& filename);
	std::wstring ensureLongPathPrefixWide(const char* filename);
	std::wstring ensureLongPathPrefixWide(const wchar_t* filename);
	std::wstring ensureLongPathPrefixWide(const std::wstring& filename);
	const char* skipLongPathPrefix(const char* filename);
	const char* skipLongPathPrefix(const std::string& filename);
	const wchar_t* skipLongPathPrefix(const wchar_t* filename);
	const wchar_t* skipLongPathPrefix(const std::wstring& filename);

	void globFiles(const char* pattern, std::vector<std::string>& files, bool returnAbsolutePaths = true);
	void globFiles(const wchar_t* pattern, std::vector<std::wstring>& files, bool returnAbsolutePaths = true);


	template<typename CharType>
	class FilepathTraits {
		public:
			static const CharType Dot = '.';
			static constexpr CharType PathSeparators[] = { '\\','/',0 };
	};


	// filename without path or extension
	template<class CharType>
	std::basic_string_view<CharType> getBaseFilename(const CharType* sourceFilename) {
		std::basic_string_view<CharType> filenameView(sourceFilename);

		size_t filenameStart = 0;
		size_t filenameEnd = 0;

		size_t slashLocation = filenameView.find_last_of(FilepathTraits<CharType>::PathSeparators);
		if (slashLocation != std::basic_string_view<CharType>::npos) {
			filenameStart = slashLocation + 1;
		}

		size_t dotLocation = std::basic_string_view<CharType>(sourceFilename + filenameStart).find_last_of(FilepathTraits<CharType>::Dot);
		if (dotLocation == std::basic_string_view<CharType>::npos) {
			filenameEnd = filenameView.length();
		} else {
			filenameEnd = filenameStart + dotLocation;
		}

		return filenameView.substr(filenameStart, filenameEnd - filenameStart);
	}

	// filename without path, with extension
	template<class CharType>
	std::basic_string_view<CharType> getFilenameComponent(const CharType* filePath) {
		assert(filePath != nullptr);

		std::basic_string_view<CharType> filePathView(filePath);
		size_t slashLocation = filePathView.find_last_of(FilepathTraits<CharType>::PathSeparators);

		if (slashLocation != std::basic_string_view<CharType>::npos) {
			return filePathView.substr(slashLocation + 1);
		} else {
			return filePathView;
		}
	}

	// includes trailing slash if present
	template<class CharType>
	std::basic_string_view<CharType> getPathComponent(const CharType* filePath) {
		assert(filePath != nullptr);

		std::basic_string_view<CharType> filePathView(filePath);
		size_t slashLocation = filePathView.find_last_of(FilepathTraits<CharType>::PathSeparators);

		if (slashLocation != std::basic_string_view<CharType>::npos) {
			return filePathView.substr(0, slashLocation + 1);
		} else {
			return std::basic_string_view<CharType>();
		}
	}

	template<class CharType>
	std::basic_string_view<CharType> getFileExtension(const CharType* filename) {
		std::string_view filenameView(filename);

		size_t extensionStart = 0;

		size_t dotLocation = filenameView.find_last_of(FilepathTraits<CharType>::Dot);
		if (dotLocation == std::basic_string_view<CharType>::npos) {
			extensionStart = 0;
		} else {
			extensionStart = dotLocation + 1;
		}

		return filenameView.substr(extensionStart, filenameView.length());
	}

	// remove dot and extension
	template<class CharType>
	std::basic_string_view<CharType> removeFileExtension(const CharType* filename) {
		std::string_view filenameView(filename);

		size_t dotLocation = filenameView.find_last_of(FilepathTraits<CharType>::Dot);
		if (dotLocation == std::basic_string_view<CharType>::npos) {
			return filenameView;
		} else {
			return filenameView.substr(0, dotLocation);
		}
	}


	// extract the path and filename+extension.
	// if a path is present, it will be included in filenameOut.
	template<class CharType, class StringClass>
	void extractPathAndFilename(const std::basic_string_view<CharType>& filenameView, StringClass& pathOut, StringClass& filenameAndExtensionOut) {
		const size_t stringEnd = filenameView.length();
		const size_t pathStart = 0;
		size_t filenameStart = pathStart;

		const size_t slashLocation = filenameView.find_last_of(FilepathTraits<CharType>::PathSeparators);
		if (slashLocation != std::basic_string_view<CharType>::npos) {
			filenameStart = slashLocation + 1;
		}

		const size_t pathLength = filenameStart - pathStart;
		const size_t filenameLength = stringEnd - filenameStart;

		pathOut = filenameView.substr(0, filenameStart);
		filenameAndExtensionOut = filenameView.substr(filenameStart, filenameLength);
	}
	template<class CharType, class StringClass>
	void extractPathAndFilename(const CharType* filename, StringClass& pathOut, StringClass& filenameAndExtensionOut) {
		std::basic_string_view<CharType> filenameView(filename);

		extractPathAndFilename(filenameView, pathOut, filenameAndExtensionOut);
	}
	void extractPathAndFilename(const char* filename, std::string& pathOut, std::string& filenameAndExtensionOut);
	void extractPathAndFilename(const char* filename, std::string_view& pathOut, std::string_view& filenameAndExtensionOut);
	void extractPathAndFilename(const wchar_t* filename, std::wstring& pathOut, std::wstring& filenameAndExtensionOut);
	void extractPathAndFilename(const wchar_t* filename, std::wstring_view& pathOut, std::wstring_view& filenameAndExtensionOut);


	// extract filename and extension components.
	// if a path is present, it will be included in filenameOut.
	// if an extension is present, dottedExtensionOut will include its dot
	template<class CharType, class StringClass>
	void extractFilenameAndExtension(const std::basic_string_view<CharType>& filenameView, StringClass& filenameOut, StringClass& dottedExtensionOut) {
		const size_t stringEnd = filenameView.length();
		const size_t filenameStart = 0;

		size_t extensionStart = stringEnd;

		const size_t dotLocation = filenameView.find_last_of(FilepathTraits<CharType>::Dot);
		if (dotLocation != std::basic_string_view<CharType>::npos) {
			extensionStart = dotLocation;
		}

		const size_t filenameLength = dotLocation - filenameStart;
		const size_t extensionLength = stringEnd - extensionStart;

		filenameOut = filenameView.substr(filenameStart, filenameLength);
		dottedExtensionOut = filenameView.substr(extensionStart, extensionLength);
	}
	template<class CharType, class StringClass>
	void extractFilenameAndExtension(const CharType* filename, StringClass& filenameOut, StringClass& dottedExtensionOut) {
		std::basic_string_view<CharType> filenameView(filename);

		extractFilenameAndExtension(filenameView, filenameOut, dottedExtensionOut);
	}
	void extractFilenameAndExtension(const char* filename, std::string_view& filenameOut, std::string_view& dottedExtensionOut);
	void extractFilenameAndExtension(const char* filename, std::string& filenameOut, std::string& dottedExtensionOut);
	void extractFilenameAndExtension(const wchar_t* filename, std::wstring_view& filenameOut, std::wstring_view& dottedExtensionOut);
	void extractFilenameAndExtension(const wchar_t* filename, std::wstring& filenameOut, std::wstring& dottedExtensionOut);


	// extract path, filename and extension components.
	// if a path is present, pathOut will include its trailing slash.
	// filenameOut will receive the base filename
	// if an extension is present, dottedExtensionOut will include its dot
	template<class CharType, class StringClass>
	void extractPathFilenameAndExtension(const std::basic_string_view<CharType>& filenameView, StringClass& pathOut, StringClass& filenameOut, StringClass& dottedExtensionOut) {
		const size_t stringEnd = filenameView.length();
		const size_t pathStart = 0;
		size_t filenameStart = pathStart;

		const size_t slashLocation = filenameView.find_last_of(FilepathTraits<CharType>::PathSeparators);
		if (slashLocation != std::basic_string_view<CharType>::npos) {
			filenameStart = slashLocation + 1;
		}

		const size_t pathLength = filenameStart - pathStart;

		pathOut = filenameView.substr(pathStart, pathLength);

		const std::basic_string_view<CharType> afterPathView = filenameView.substr(filenameStart);
		extractFilenameAndExtension<CharType, StringClass>(afterPathView, filenameOut, dottedExtensionOut);
	}
	template<class CharType, class StringClass>
	void extractPathFilenameAndExtension(const CharType* filename, StringClass& pathOut, StringClass& filenameOut, StringClass& dottedExtensionOut) {
		std::basic_string_view<CharType> filenameView(filename);

		extractPathFilenameAndExtension(filenameView, pathOut, filenameOut, dottedExtensionOut);
	}
	void extractPathFilenameAndExtension(const char* filename, std::string_view& pathOut, std::string_view& filenameOut, std::string_view& dottedExtensionOut);
	void extractPathFilenameAndExtension(const char* filename, std::string& pathOut, std::string& filenameOut, std::string& dottedExtensionOut);
	void extractPathFilenameAndExtension(const wchar_t* filename, std::wstring_view& pathOut, std::wstring_view& filenameOut, std::wstring_view& dottedExtensionOut);
	void extractPathFilenameAndExtension(const wchar_t* filename, std::wstring& pathOut, std::wstring& filenameOut, std::wstring& dottedExtensionOut);

	std::string changeFileExtension(const char* filename, const char* newExtension);
	std::wstring changeFileExtension(const wchar_t* filename, const wchar_t* newExtension);
	
	bool isAbsolutePath(const char* path);
	bool isAbsolutePath(const wchar_t* path);
	bool isRelativePath(const char* path);
	bool isRelativePath(const wchar_t* path);
	std::string mergeRelativePath(const char* startDirectory, const char* relativePath);
	std::string createRelativePath(const char* startDirectory, const char* endDirectory);

	bool getFullPathName(const char* filePath, std::string& fullPathOut);
	bool getFullPathName(const wchar_t* filePath, std::wstring& fullPathOut);
	bool getFullPathName(const char* filePath, std::wstring& fullPathOut);

	bool canonicalizePathName(const wchar_t* filePath, std::wstring& canonicalizedOut);
	bool canonicalizePathName(const char* filePath, std::string& canonicalizedOut);
	bool canonicalizePathName(const std::string_view& filePath, std::string& canonicalizedOutUtf8);
	bool canonicalizePathName(const std::wstring& filePath, std::wstring& canonicalizedOut);
	bool canonicalizePathName(const std::wstring& filePath, std::string& canonicalizedOut);

	bool getCurrentDirectory(std::string& currentDirectoryOut);
	bool getCurrentDirectory(std::wstring& currentDirectoryOut);

	// returns true if folder was created or already exists
	bool createFolder(const char* path);
	bool createFolder(const wchar_t* path);
	bool createNestedFolders(const char* path);
	bool createNestedFolders(const wchar_t* path);

	bool moveFile(const char* oldFilename, const char* newFilename, bool flush = true);
	bool moveFile(const wchar_t* oldFilename, const wchar_t* newFilename, bool flush = true);

	bool readEntireFile(const char* filename, ByteBuffer& fileBufferOut);
}
}
