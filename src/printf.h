#pragma once
#include <stdarg.h>

/// @brief Prints a formatted string using a char-printing function.
/// @param write A function that prints a single character.
/// @param format A null-delimited format string.
/// @param args Additional arguments as specified in the format string.
/// @return The number of bytes written.
int printf(void (*write)(char), const char *format, ...);

int vprintf(void (*write)(char), const char *format, va_list args);
