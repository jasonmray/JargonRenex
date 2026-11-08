#pragma once

#include <string>
#include <vector>


namespace Jargon{
namespace FileSystem{

	enum class DateOrder {
		OldestFirst,
		NewestFirst
	};

	bool sortFilesByModifiedDate(std::vector<std::string>& files, DateOrder dateOrder);

	bool filterOutHiddenFiles(std::vector<std::string>& files);

}
}

