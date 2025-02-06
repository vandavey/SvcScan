/*
* @file
*     main.h
* @brief
*     Header file for the application entry point.
*/
#pragma once

#ifndef SCAN_MAIN_H
#define SCAN_MAIN_H

#include "console/args.h"

int main(int argc, char* argv[]);

namespace scan
{
    int exec_scan(const Args& t_args);
}

#endif // !SCAN_MAIN_H
