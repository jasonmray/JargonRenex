#pragma once

#include <string>
#include <vector>


namespace Jargon{

	struct FileRename {
		std::string oldFilename;
		std::string newFilename;
	};

	class RenexResults{
		public:
			RenexResults();
			~RenexResults();

			std::vector<FileRename> fileRenames;
	};

}

