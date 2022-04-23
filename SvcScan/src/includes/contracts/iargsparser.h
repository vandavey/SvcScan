/*
*  iargsparser.h
*  -------------
*  Header file for an interface that enforces argument parsing
*/
#pragma once

#ifndef I_ARGS_PARSER_H
#define I_ARGS_PARSER_H

#include "../utils/args.h"

namespace scan
{
    /// ***
    /// Interface that enforces argument parsing mechanisms
    /// ***
    class IArgsParser
    {
    public:  /* Constructors & Destructor */
        IArgsParser() = default;
        IArgsParser(const IArgsParser &) = default;

        virtual ~IArgsParser() = default;

    public:  /* Methods */
        virtual void parse_args(const Args &t_args) = 0;
    };
}

#endif // !I_ARGS_PARSER_H
