/*
* @file
*     i_args_parser.h
* @brief
*     Header file for an interface that enforces argument parsing.
*/
#pragma once

#ifndef SCAN_I_ARGS_PARSER_H
#define SCAN_I_ARGS_PARSER_H

#include "../console/args.h"
#include "../utils/aliases.h"

namespace scan
{
    /**
    * @brief
    *     Interface that enforces argument parsing mechanisms.
    */
    class IArgsParser
    {
    public:  /* Constructors & Destructor */
        IArgsParser() = default;
        IArgsParser(const IArgsParser&) = default;
        IArgsParser(IArgsParser&&) = default;

        virtual ~IArgsParser() = default;

    public:  /* Operators */
        IArgsParser& operator=(const IArgsParser&) = default;
        IArgsParser& operator=(IArgsParser&&) = default;

    public:  /* Methods */
        virtual void parse_argsp(shared_ptr<Args> t_argsp) = 0;
    };
}

#endif // !SCAN_I_ARGS_PARSER_H
