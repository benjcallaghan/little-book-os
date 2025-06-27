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
            {
                char const *arg = va_arg(args, char const *);
                for (; *arg != '\0'; ++arg)
                {
                    write(*arg);
                    ++bytes_written;
                }
                break;
            }
            case 'X':
            {
                uint64_t arg = va_arg(args, uint64_t);
                for (int i = 28; i >= 0; i -= 4)
                {
                    int nibble = (arg >> i) & 0xF;
                    char c = nibble < 10 ? nibble + '0' : nibble - 10 + 'A';
                    write(c);
                    ++bytes_written;
                }
                break;
            }
            case 'u':
            {
                uint32_t arg = va_arg(args, uint32_t);
                char buf[sizeof arg * __CHAR_BIT__ / 3 + 2]; // Slightly oversized buffer
                char *buf_end = buf + sizeof buf - 1;
                char *result = buf_end;

                do {
                    *--result = '0' + (arg % 10);
                    arg /= 10;
                } while (arg);

                for (; result != buf_end; ++result)
                {
                    write(*result);
                    ++bytes_written;
                }

                break;
            }
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
