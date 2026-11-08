#pragma once

#include <string>
#include <vector>


namespace Jargon{

	struct FileRename {
		std::wstring oldFilename;
		std::wstring newFilename;
	};

	class RenexResults{
		public:
			RenexResults();
			~RenexResults();

			std::vector<FileRename> fileRenames;
	};

}

