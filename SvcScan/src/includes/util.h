/*
*  util.h
*  ------
*  Header file for string and standard stream manipulation
*/
#pragma once

#ifndef UTIL_H
#define UTIL_H

#include <sstream>
#include <string>
#include <vector>
#include "except/argex.h"
#include "except/nullargex.h"
#include "properties/autoprop.h"

namespace Scan
{
    /// ***
    /// String and standard stream manipulation utilities
    /// ***
    class Util
    {
    public:  /* Constants */
        static constexpr char CR = {'\r'};        // Carriage-return 
        static constexpr char LF = {'\n'};        // Unix EOL (line feed)
        static constexpr char CRLF[] = {CR, LF};  // NT EOL (CR-LF)

    private:  /* Types & Constants */
        typedef long long llong;
        typedef unsigned long ulong;

        typedef std::vector<std::string> vector_s;

        // Ansi escape (style) sequences
        static constexpr char CYAN[] = "\033[38;2;0;255;255m";
        static constexpr char RED[] = "\033[38;2;246;0;0m";
        static constexpr char RESET[] = "\033[0m";
        static constexpr char YELLOW[] = "\033[38;2;250;230;39m";

    public:  /* Fields */
        static AutoProp<bool> vt_enabled;  // VT escape processing

    public:  /* Destructor */
        virtual ~Util() = default;

    private:  /* Constructors (deleted) */
        Util() = delete;
        Util(const Util &) = delete;

    public:  /* Methods */
        static void error(const std::string &msg);
        static void errorf(const std::string &msg, const std::string &arg);
        static void except(const ArgEx &ex);
        static void except(const NullArgEx &ex);
        static void print(const std::string &msg);
        static void warn(const std::string &msg);
        static void warnf(const std::string &msg, const std::string &arg);

        static const int enable_vt();
        static const char *itoc(const llong &num);

        static vector_s split(const std::string &data, const char &sep);

        static const std::string ctos(const char &ch);

        template<class T>
        static const std::string fmt(const std::string &msg, const T &arg);

        static const std::string itos(const llong &num);

        static const std::string join(const std::string &sep,
                                      const vector_s &vect) noexcept;

        static const std::string utf8(const std::wstring &data_w);
        static const std::wstring utf16(const std::string &data);
    };
}

/// ***
/// Interpolate string with argument at '%' position(s)
/// ***
template<class T>
inline const std::string Scan::Util::fmt(const std::string &msg,
                                         const T &arg) {
    if (msg.find('%') == -1)
    {
        throw ArgEx("msg", "Unable to locate format char: '%'");
    }
    std::stringstream ss;

    // Populate stringstream data
    for (int i = {0}; i < msg.length(); i++)
    {
        (msg[i] == '%') ? (ss << arg) : (ss << msg[i]);
    }
    return ss.str();
}

#endif // !UTIL_H
