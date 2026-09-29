#include "Log.h"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace Log {
void Init() {
    auto console = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    auto file = std::make_shared<spdlog::sinks::basic_file_sink_mt>("latest.log", true);

    auto logger = std::make_shared<spdlog::logger>("neoretro", spdlog::sinks_init_list{ console, file });
    logger->set_pattern("[%H:%M:%S.%e] [%^%l%$] [%s:%#] %v");
#ifdef _DEBUG
    logger->set_level(spdlog::level::trace);
#else
    logger->set_level(spdlog::level::info);
#endif
    logger->flush_on(spdlog::level::warn);
    spdlog::set_default_logger(logger);
}
} // namespace Log
