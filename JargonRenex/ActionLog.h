#pragma once

#include "ProgramOptions.h"

#include "Jargon/ErrorContext.h"
#include "Jargon/FileSystem/BinaryFileWriter.h"
#include "Jargon/TextWriter.h"


class ActionLog {
	public:
		ActionLog(const ProgramOptions& programOptions, Jargon::ErrorContext& errorContext);
		~ActionLog();

		bool open();

		void write(const char* text);
		void write(const std::string& text);
		void write(char c);

		void writeLine(const char* line);
		void writeLine(const std::string& text);

		void writeFormatted(const char* format, ...);
		void writeFormattedVarArg(const char* format, va_list args);

		void writeLineFormatted(const char* format, ...);
		void writeLineFormattedVarArg(const char* format, va_list args);

		void newLine();

		void flush();

	private:
		const ProgramOptions& programOptions;
		Jargon::ErrorContext& errorContext;
		bool enabled;
		Jargon::FileSystem::BinaryFileWriter logFile;
		Jargon::TextWriter textWriter;
};


