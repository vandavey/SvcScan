/*
* @file
*     main.h
* @brief
*     Header file for the application entry point.
*/
#pragma once

#ifndef MAIN_H
#define MAIN_H

#include "utils/args.h"

namespace scan
{
    void setup_console();

    int run_scan(const Args &t_args);
}

int main(int argc, char **argv);

#endif // !MAIN_H
