#pragma once

/// @brief Writes a single character to the framebuffer's current position
/// @param c The character to write.
void framebuffer_write_char(char c);

/** write:
 * Writes a string to the framebuffer.
 * @param str A null-delimited string.
 */
void framebuffer_write(char const *str);

/**
 * Clears the framebuffer of all contents
 */
void framebuffer_clear();