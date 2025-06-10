#pragma once

/// @brief Writes a single character to the framebuffer's current position
/// @param c The character to write.
void fb_write_char(char c);

/** write:
 * Writes a string to the framebuffer.
 * @param str A null-delimited string.
 */
void fb_write(char const *str);

/**
 * Clears the framebuffer of all contents
 */
void fb_clear();