#include <stdarg.h>

int printf(void (*write)(char), char const *format, ...)
{
    va_list args;
    int bytes_written = 0;

    for (va_start(args, format); *format != 0; ++format)
    {
        if (*format == '%')
        {
            ++format;
            switch (*format)
            {
            case 'X':
                unsigned long arg = va_arg(args, unsigned long);
                for (int i = 28; i >= 0; i -= 4)
                {
                    unsigned char nibble = (arg >> i) & 0xF;
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

    va_end(args);
    return bytes_written;
}