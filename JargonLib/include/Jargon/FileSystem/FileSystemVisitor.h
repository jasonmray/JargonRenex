#pragma once

#include "Jargon/ErrorContext.h"

#include <cstdint>


namespace Jargon{
namespace FileSystem{

	class FileSystemVisitor{
		public:
			FileSystemVisitor();
			virtual ~FileSystemVisitor();

			// each method should return true to continue processing,
			// return false to abort processing.
			virtual bool visitFile(const char* filename);
			virtual bool beginFolder(const char* path);
			virtual bool endFolder(const char* path);
			virtual bool onError(const ErrorContext& errorContext, uint32_t windowsErrorCode);
		private:

	};

}
}

