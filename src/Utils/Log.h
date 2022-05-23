#pragma once

#include <memory>

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace Utility {
	class Log
	{
	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetLogger() { return s_Logger; }

	private:
		static std::shared_ptr<spdlog::logger> s_Logger;
	};
}

// Log macros
#define LOG_ERROR(...)			::Utility::Log::GetLogger()->error(__VA_ARGS__)
#define LOG_WARN(...)			::Utility::Log::GetLogger()->warn(__VA_ARGS__)
#define LOG_INFO(...)			::Utility::Log::GetLogger()->info(__VA_ARGS__)
#define LOG_TRACE(...)			::Utility::Log::GetLogger()->trace(__VA_ARGS__)
#define LOG_CRITICAL(...)		::Utility::Log::GetLogger()->critical(__VA_ARGS__)

// Strip trace and info from release build
#ifdef RELEASE
	#undef LOG_TRACE
	#define LOG_TRACE
#endif

// Strip all from full dist build
#ifdef DIST
	#undef LOG_ERROR
	#undef LOG_WARN
	#undef LOG_INFO
	#undef LOG_TRACE
	#undef LOG_CRITICAL
	#define LOG_ERROR
	#define LOG_WARN
	#define LOG_INFO
	#define LOG_TRACE
	#define LOG_CRITICAL
#endif