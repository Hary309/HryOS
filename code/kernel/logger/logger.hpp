#pragma once

#include <format.hpp>

#include "utility.hpp"

namespace logger
{
    enum class level
    {
        info,
        warning,
        error
    };

    void init();
    void write_char(char ch);

    inline const char* string_level(level lvl)
    {
        switch (lvl)
        {
            case level::info: return "info";
            case level::warning: return "warn";
            case level::error: return "erro";
        }

        return "";
    }

    template<typename... Args>
    void log(level lvl, const char* fmt, Args&&... args)
    {
        hlib::format_to(write_char, "[{}] ", string_level(lvl));
        hlib::format_to(write_char, fmt, hlib::forward<Args>(args)...);
        write_char('\r');
        write_char('\n');
    }

    template<typename... Args>
    void info(const char* fmt, Args&&... args)
    {
        log(level::info, fmt, hlib::forward<Args>(args)...);
    }

    template<typename... Args>
    void warning(const char* fmt, Args&&... args)
    {
        log(level::warning, fmt, hlib::forward<Args>(args)...);
    }

    template<typename... Args>
    void error(const char* fmt, Args&&... args)
    {
        log(level::error, fmt, hlib::forward<Args>(args)...);
    }
} // namespace logger
