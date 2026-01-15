#include "Log.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"
#include <vector>

std::shared_ptr<spdlog::logger> Log::s_Logger;

void Log::Init()
{
	std::vector<spdlog::sink_ptr> logSinks;

	logSinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
	logSinks[0]->set_pattern("%^[%T] %n: %v%$");

	logSinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/engine.log", true));
	logSinks[1]->set_pattern("[%T] [%l] %n: %v");

	s_Logger = std::make_shared<spdlog::logger>("GAME", begin(logSinks), end(logSinks));
	spdlog::register_logger(s_Logger);

	s_Logger->set_level(spdlog::level::trace);
	s_Logger->set_level(spdlog::level::info);
	s_Logger->set_level(spdlog::level::warn);
	s_Logger->set_level(spdlog::level::err);
	s_Logger->set_level(spdlog::level::critical);

	s_Logger->flush_on(spdlog::level::trace);
}

std::shared_ptr<spdlog::logger>& Log::GetLogger()
{
	return s_Logger;
}