#include "Log.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"
#include <vector>

std::shared_ptr<spdlog::logger> Log::s_Logger;

void Log::Init()
{
	auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
	console_sink->set_level(spdlog::level::warn);
	console_sink->set_pattern("[%H:%M:%S %z] [%n] [%^---%L---%$] [thread %t] %v");

	auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/engine.log", true);
	file_sink->set_level(spdlog::level::trace);

	std::vector<spdlog::sink_ptr> logSinks;

	logSinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
	logSinks[0]->set_pattern("%^[%T] [%l] %n: %v%$");
	logSinks[0]->set_level(spdlog::level::warn);

	logSinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/engine.log", true));
	logSinks[1]->set_pattern("[%T] [%l] %n: %v");
	logSinks[1]->set_level(spdlog::level::trace);

	s_Logger = std::make_shared<spdlog::logger>("GAME", begin(logSinks), end(logSinks));
	spdlog::register_logger(s_Logger);

	s_Logger->flush_on(spdlog::level::trace);
}

std::shared_ptr<spdlog::logger>& Log::GetLogger()
{
	return s_Logger;
}