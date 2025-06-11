#pragma once

enum log_level {
	log_debug,
	log_info,
	log_warn,
	log_error,
};

void logf(enum log_level level, char const *format, ...);

