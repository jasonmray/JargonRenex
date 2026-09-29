#include "ActionLog.h"


ActionLog::ActionLog(const ProgramOptions& programOptions, Jargon::ErrorContext& errorContext):
	programOptions(programOptions),
	errorContext(errorContext),
	textWriter(logFile),
	enabled(programOptions.logFileEnabled)
{
}

ActionLog::~ActionLog(){
}

bool ActionLog::open() {
	if (enabled) {
		if (!logFile.openFile(programOptions.logFilename.c_str(), Jargon::FileSystem::BinaryFileWriter::OpenMode_Append)) {
			errorContext.setLastError("Failed to open log file for writing: %s", programOptions.logFilename.c_str());
			return false;
		}
	}

	return true;
}

void ActionLog::write(const char* text) {
	if (enabled) {
		textWriter.write(text);
	}
}

void ActionLog::write(const std::string& text) {
	if (enabled) {
		textWriter.write(text);
	}
}

void ActionLog::write(char c) {
	if (enabled) {
		textWriter.write(c);
	}
}

void ActionLog::writeLine(const char* line) {
	if (enabled) {
		textWriter.writeLine(line);
	}
}

void ActionLog::writeLine(const std::string& text) {
	if (enabled) {
		textWriter.writeLine(text);
	}
}

void ActionLog::writeFormatted(const char* format, ...) {
	if (enabled) {
		va_list args;
		va_start(args, format);
		textWriter.writeFormattedVarArg(format, args);
		va_end(args);
	}
}

void ActionLog::writeFormattedVarArg(const char* format, va_list args) {
	if (enabled) {
		textWriter.writeFormattedVarArg(format, args);
	}
}

void ActionLog::writeLineFormatted(const char* format, ...) {
	if (enabled) {
		va_list args;
		va_start(args, format);
		textWriter.writeLineFormattedVarArg(format, args);
		va_end(args);
	}
}

void ActionLog::writeLineFormattedVarArg(const char* format, va_list args) {
	if (enabled) {
		textWriter.writeLineFormattedVarArg(format, args);
	}
}

void ActionLog::newLine() {
	if (enabled) {
		textWriter.newLine();
	}
}

void ActionLog::flush() {
	if (enabled) {
		textWriter.flush();
	}
}
