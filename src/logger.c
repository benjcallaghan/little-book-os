#include <stdarg.h>
#include "logger.h"
#include "drivers/framebuffer.h"
#include "drivers/serial.h"
#include "printf.h"

void logf(enum log_level level, char const *format, ...)
{
	va_list args;

	va_start(args, format);	
	vprintf(serial_write_char, format, args);
	printf(serial_write_char, "\n");
	va_end(args);

	if (level > log_debug)
	{
		va_start(args, format);
		vprintf(framebuffer_write_char, format, args);
		printf(framebuffer_write_char, "\n");
		va_end(args);
	}
}
