/*
*  std_util.h
*  ----------
*  Header file for standard console stream utilities
*/
#pragma once

#ifndef STD_UTIL_H
#define STD_UTIL_H

#include <atomic>
#include <mutex>
#include "../utils/algorithm.h"

namespace scan
{
    /**
    * @brief  Standard console stream utilities.
    */
    class StdUtil final
    {
    private:  /* Type Aliases */
        using ulong = unsigned long;

        using algo        = Algorithm;
        using atomic_bool = std::atomic_bool;
        using mutex       = std::mutex;
        using ostream     = std::ostream;
        using scoped_lock = std::scoped_lock<mutex>;
        using string      = std::string;

    public:  /* Constants */
        static constexpr char CR[]   = "\r";    // Carriage return
        static constexpr char CRLF[] = "\r\n";  // Carriage return line feed
        static constexpr char LF[]   = "\n";    // Line feed

    private:  /* Constants */
        static constexpr char RESET[] = "\033[0m";  // Ansi reset sequence

        // Ansi color escape sequences
        static constexpr char CYAN[]   = "\033[38;2;0;255;255m";
        static constexpr char GREEN[]  = "\033[38;2;166;226;46m";
        static constexpr char RED[]    = "\033[38;2;246;0;0m";
        static constexpr char YELLOW[] = "\033[38;2;250;230;39m";

    public:  /* Fields */
        static atomic_bool vt_enabled;  // VT escape processing

    private:  /* Fields */
        static mutex m_cerr_mtx;  // Standard error mutex
        static mutex m_cout_mtx;  // Standard output mutex

    public:  /* Destructor */
        StdUtil() = delete;
        StdUtil(const StdUtil &) = delete;
        StdUtil(StdUtil &&) = delete;

        virtual ~StdUtil() = default;

    public:  /* Operators */
        StdUtil &operator=(const StdUtil &) = default;
        StdUtil &operator=(StdUtil &&) = default;

    public:  /* Methods */
        static void error(const string &t_msg);

        template<class T, class ...Args>
        static void errorf(const string &t_msg,
                           const T &t_arg,
                           const Args &...t_args);

        static void except(const string &t_msg);
        static void info(const string &t_msg);

        template<class T, class ...Args>
        static void infof(const string &t_msg,
                          const T &t_arg,
                          const Args &...t_args);

        static void print(const string &t_msg);

        template<class T, class ...Args>
        static void printf(const string &t_msg,
                           const T &t_arg,
                           const Args &...t_args);

        static void warn(const string &t_msg);

        template<class T, class ...Args>
        static void warnf(const string &t_msg,
                          const T &t_arg,
                          const Args &...t_args);

        static int enable_vt();

    private:  /* Methods */
        static string str_color(const string &t_fg, const string &t_msg);
    };
}

/**
* @brief  Interpolate arguments in the error message and write the
*         result to the standard error stream.
*/
template<class T, class ...Args>
inline void scan::StdUtil::errorf(const string &t_msg,
                                  const T &t_arg,
                                  const Args &...t_args) {

    error(algo::fstr(t_msg, t_arg, t_args...));
}

/**
* @brief  Interpolate arguments in the informational message and write the
*         result to the standard output stream.
*/
template<class T, class ...Args>
inline void scan::StdUtil::infof(const string &t_msg,
                                 const T &t_arg,
                                 const Args &...t_args) {

    info(algo::fstr(t_msg, t_arg, t_args...));
}

/**
* @brief  Interpolate arguments in the status message and write the
*         result to the standard output stream.
*/
template<class T, class ...Args>
inline void scan::StdUtil::printf(const string &t_msg,
                                  const T &t_arg,
                                  const Args &...t_args) {

    print(algo::fstr(t_msg, t_arg, t_args...));
}

/**
* @brief  Interpolate arguments in the warning message and write the
*         result to the standard error stream.
*/
template<class T, class ...Args>
inline void scan::StdUtil::warnf(const string &t_msg,
                                 const T &t_arg,
                                 const Args &...t_args) {

    warn(algo::fstr(t_msg, t_arg, t_args...));
}

#endif // !STD_UTIL_H
