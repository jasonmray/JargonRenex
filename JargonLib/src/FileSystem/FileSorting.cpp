#include "Jargon/FileSystem/FileSorting.h"
#include "Jargon/FileSystem/Utilities.h"

#include <algorithm>
#include <utility>


namespace Jargon{
namespace FileSystem{

	bool sortFilesByModifiedDate(std::vector<std::string>& files, DateOrder dateOrder) {
		const size_t fileCount = files.size();

		typedef std::pair<int64_t, size_t> DateIndexPair;
		std::vector<DateIndexPair> filesDatesIndexes(fileCount);

		for (size_t i = 0; i < fileCount; i++) {
			int64_t fileModDate = 0;
			if (!Jargon::FileSystem::getFileModifiedTimeUnixEpochSeconds(files[i].c_str(), &fileModDate)) {
				return false;
			}
			filesDatesIndexes[i] = DateIndexPair(fileModDate, i);
		}

		if (dateOrder == DateOrder::OldestFirst) {
			std::sort(filesDatesIndexes.begin(), filesDatesIndexes.end());
		} else {
			std::sort(filesDatesIndexes.begin(), filesDatesIndexes.end(), std::greater<DateIndexPair>());
		}

		std::vector<std::string> sorted(fileCount);

		for (size_t i = 0; i < fileCount; i++) {
			sorted[i] = files[filesDatesIndexes[i].second];
		}

		files.swap(sorted);

		return true;
	}

	bool filterOutHiddenFiles(std::vector<std::string>& files) {
		const size_t fileCount = files.size();

		if (fileCount == 0) {
			return true;
		}

		for (size_t i = fileCount; i > 0; i--) {
			const size_t fileIndex = i - 1;

			bool hiddenFlag = false;
			if (!Jargon::FileSystem::getFileHiddenAttribute(files[fileIndex].c_str(), &hiddenFlag)) {
				return false;
			}

			if (hiddenFlag) {
				files.erase(files.begin() + fileIndex);
			}
		}

		return true;
	}

}
}
