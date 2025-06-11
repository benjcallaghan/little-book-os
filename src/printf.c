#include <stdarg.h>
#include <stdint.h>

int vprintf(void (*write)(char), char const *format, va_list args)
{
    int bytes_written = 0;

    for (; *format != 0; ++format)
    {
        if (*format == '%')
        {
            ++format;
            switch (*format)
            {
            case 's':
		    char const *s_arg = va_arg(args, char const *);
		    for (; *s_arg != '\0'; ++s_arg)
		    {
			    write(*s_arg);
			    ++bytes_written;
		    }
		    break;
            case 'X':
                uint32_t u32_arg = va_arg(args, uint32_t);
                for (int i = 28; i >= 0; i -= 4)
                {
                    int nibble = (u32_arg >> i) & 0xF;
                    char c = nibble < 10 ? nibble + '0' : nibble - 10 + 'A';
                    write(c);
                    ++bytes_written;
                }
                break;
            }
        }
        else
        {
            write(*format);
            ++bytes_written;
        }
    }

    return bytes_written;
}

int printf(void (*write)(char), char const *format, ...)
{
    va_list args;

    va_start(args, format);
    int result = vprintf(write, format, args);
    va_end(args);

    return result;
}
