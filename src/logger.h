#pragma once
#include <memory>
#include <spdlog/logger.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

class Logger
{

  public:
    static inline void init()
    {
        spdlog::set_pattern("[%H:%M:%S %z] [%^%L%$] [%&] %v");
        main_logger = spdlog::stdout_color_mt("main_logger");
    };

    static inline std::shared_ptr<spdlog::logger> get_main_logger()
    {
        if (main_logger == NULL)
        {
            init();
        }
        return main_logger;
    }

  private:
    static inline std::shared_ptr<spdlog::logger> main_logger;
};

#define LOG_INFO(...) Logger::get_main_logger()->info(__VA_ARGS__)
#define LOG_WARN(...) Logger::get_main_logger()->warn(__VA_ARGS__)
#define LOG_DEBUG(...) Logger::get_main_logger()->debug(__VA_ARGS__)
