/*
*  i_args_parser.h
*  ---------------
*  Header file for an interface that enforces argument parsing
*/
#pragma once

#ifndef I_ARGS_PARSER_H
#define I_ARGS_PARSER_H

#include "../utils/args.h"
#include "../utils/type_defs.h"

namespace scan
{
    /**
    * @brief  Interface that enforces argument parsing mechanisms.
    */
    class IArgsParser
    {
    public:  /* Constructors & Destructor */
        IArgsParser() = default;
        IArgsParser(const IArgsParser &) = default;
        IArgsParser(IArgsParser &&) = default;

        virtual ~IArgsParser() = default;

    public:  /* Operators */
        IArgsParser &operator=(const IArgsParser &) = default;
        IArgsParser &operator=(IArgsParser &&) = default;

    public:  /* Methods */
        virtual void parse_argsp(shared_ptr<Args> t_argsp) = 0;
    };
}

#endif // !I_ARGS_PARSER_H
