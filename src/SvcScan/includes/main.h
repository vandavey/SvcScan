/*
* @file
*     main.h
* @brief
*     Header file for the application entry point.
*/
#pragma once

#ifndef SCAN_MAIN_H
#define SCAN_MAIN_H

#include "utils/args.h"

namespace scan
{
    void setup_console();

    int run_scan(const Args &t_args);
}

int main(int argc, char **argv);

#endif // !SCAN_MAIN_H
