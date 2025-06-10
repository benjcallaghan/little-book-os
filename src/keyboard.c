#include "interrupts.h"
#include "drivers/serial.h"
#include "pic.h"
#include "io.h"
#include "printf.h"
#include "drivers/framebuffer.h"
#include <stdint.h>
#include <stddef.h>

static constexpr uint8_t keyboard_pic_interrupt = 1;
static constexpr uint16_t controller_data_port = 0x60;
static constexpr uint16_t controller_command_port = 0x64;
static constexpr int largest_scan_code = 6;

static uint8_t in_progress_scan_code[largest_scan_code];
static size_t scan_code_pos = 0;

static bool key_pressed[256];

enum controller_command : uint8_t
{
    read_first_byte = 0x20,
    write_first_byte = 0x60,
    disable_second_port = 0xA7,
    enable_second_port = 0xA8,
    test_second_port = 0xA9,
    test_controller = 0xAA,
    test_first_port = 0xAB,
    diagnostic_dump = 0xAC,
    disable_first_port = 0xAD,
    enable_first_port = 0xAE,
    read_controller_input = 0xC0,
    copy_low_input_to_status = 0xC1,
    copy_high_input_to_status = 0xC2,
    read_controller_output = 0xD0,
    write_controller_output = 0xD1,
    write_first_output = 0xD2,
    write_second_output = 0xD3,
    write_second_input = 0xD4,
    reset_cpu = 0xFE,
};

enum controller_configuration : uint8_t
{
    first_port_interrupt = 0x01,
    second_port_interrupt = 0x02,
    system_post = 0x04,
    first_port_clock_disabled = 0x10,
    second_port_clock_disabled = 0x20,
    first_port_translation = 0x40,
};

enum keyboard_command : uint8_t
{
    set_leds = 0xED,
    echo = 0xEE,
    scan_code = 0xF0,
    identify = 0xF2,
    typematic_rate_and_delay = 0xF3,
    enable_scanning = 0xF4,
    disable_scanning = 0xF5,
    set_default_params = 0xF6,
    set_all_typematic = 0xF7,
    set_all_make_release = 0xF8,
    set_all_make_only = 0xF9,
    set_all_typematic_make_release = 0xFA,
    set_key_typematic = 0xFB,
    set_key_make_release = 0xFC,
    set_key_make_only = 0xFD,
    resend = 0xFE,
    reset_and_test = 0xFF,
};

enum key_code : uint8_t
{
    KEY_NONE = 0x00,
    KEY_ESCAPE = 0x01,
    KEY_1 = 0x02,
    KEY_2 = 0x03,
    KEY_3 = 0x04,
    KEY_4 = 0x05,
    KEY_5 = 0x06,
    KEY_6 = 0x07,
    KEY_7 = 0x08,
    KEY_8 = 0x09,
    KEY_9 = 0x0A,
    KEY_0 = 0x0B,
    KEY_MINUS = 0x0C,
    KEY_EQUAL = 0x0D,
    KEY_BACKSPACE = 0x0E,
    KEY_TAB = 0x0F,
    KEY_Q = 0x10,
    KEY_W = 0x11,
    KEY_E = 0x12,
    KEY_R = 0x13,
    KEY_T = 0x14,
    KEY_Y = 0x15,
    KEY_U = 0x16,
    KEY_I = 0x17,
    KEY_O = 0x18,
    KEY_P = 0x19,
    KEY_LEFT_BRACKET = 0x1A,
    KEY_RIGHT_BRACKET = 0x1B,
    KEY_ENTER = 0x1C,
    KEY_LEFT_CTRL = 0x1D,
    KEY_A = 0x1E,
    KEY_S = 0x1F,
    KEY_D = 0x20,
    KEY_F = 0x21,
    KEY_G = 0x22,
    KEY_H = 0x23,
    KEY_J = 0x24,
    KEY_K = 0x25,
    KEY_L = 0x26,
    KEY_SEMICOLON = 0x27,
    KEY_APOSTROPHE = 0x28,
    KEY_GRAVE = 0x29,
    KEY_LEFT_SHIFT = 0x2A,
    KEY_BACKSLASH = 0x2B,
    KEY_Z = 0x2C,
    KEY_X = 0x2D,
    KEY_C = 0x2E,
    KEY_V = 0x2F,
    KEY_B = 0x30,
    KEY_N = 0x31,
    KEY_M = 0x32,
    KEY_COMMA = 0x33,
    KEY_DOT = 0x34,
    KEY_SLASH = 0x35,
    KEY_RIGHT_SHIFT = 0x36,
    KEY_KEYPAD_STAR = 0x37,
    KEY_LEFT_ALT = 0x38,
    KEY_SPACE = 0x39,
    KEY_CAPS_LOCK = 0x3A,
    KEY_F1 = 0x3B,
    KEY_F2 = 0x3C,
    KEY_F3 = 0x3D,
    KEY_F4 = 0x3E,
    KEY_F5 = 0x3F,
    KEY_F6 = 0x40,
    KEY_F7 = 0x41,
    KEY_F8 = 0x42,
    KEY_F9 = 0x43,
    KEY_F10 = 0x44,
    KEY_NUM_LOCK = 0x45,
    KEY_SCROLL_LOCK = 0x46,
    KEY_KEYPAD_7 = 0x47,
    KEY_KEYPAD_8 = 0x48,
    KEY_KEYPAD_9 = 0x49,
    KEY_KEYPAD_MINUS = 0x4A,
    KEY_KEYPAD_4 = 0x4B,
    KEY_KEYPAD_5 = 0x4C,
    KEY_KEYPAD_6 = 0x4D,
    KEY_KEYPAD_PLUS = 0x4E,
    KEY_KEYPAD_1 = 0x4F,
    KEY_KEYPAD_2 = 0x50,
    KEY_KEYPAD_3 = 0x51,
    KEY_KEYPAD_0 = 0x52,
    KEY_KEYPAD_DOT = 0x53,
    KEY_F11 = 0x57,
    KEY_F12 = 0x58,

    // Extended keys (0xE0 prefix in scan code)
    KEY_KEYPAD_ENTER = 0x9C,
    KEY_RIGHT_CTRL = 0x9D,
    KEY_KEYPAD_SLASH = 0xB5,
    KEY_RIGHT_ALT = 0xB8,
    KEY_HOME = 0xC7,
    KEY_UP_ARROW = 0xC8,
    KEY_PAGE_UP = 0xC9,
    KEY_LEFT_ARROW = 0xCB,
    KEY_RIGHT_ARROW = 0xCD,
    KEY_END = 0xCF,
    KEY_DOWN_ARROW = 0xD0,
    KEY_PAGE_DOWN = 0xD1,
    KEY_INSERT = 0xD2,
    KEY_DELETE = 0xD3,
    KEY_LEFT_GUI = 0xDB,
    KEY_RIGHT_GUI = 0xDC,
    KEY_APPS = 0xDD,
};

struct key_event
{
    enum key_code key;
    bool pressed;
    char character;
};

static void reset_scan_code()
{
    for (int i = 0; i < largest_scan_code; i++)
    {
        in_progress_scan_code[i] = 0;
    }
    scan_code_pos = 0;
}

static bool input_buffer_full()
{
    return inb(controller_command_port) & 0x02;
}

static void write_controller_data(uint8_t data)
{
    while (input_buffer_full())
        ;
    outb(controller_data_port, data);
}

static bool output_buffer_empty()
{
    return inb(controller_command_port) & 0x01;
}

static uint8_t read_controller_response()
{
    while (!output_buffer_empty())
        ;
    return inb(controller_data_port);
}

static uint8_t quick_read_controller_response()
{
    return inb(controller_data_port);
}

int keyboard_initialze()
{
    // TODO: Determine if a PS/2 controller exists.

    // Disable PS/2 devices to avoid errant data during init.
    serial_write("Disabling all PS/2 ports.\n");
    outb(controller_command_port, disable_first_port);
    outb(controller_command_port, disable_second_port);

    // Flush the output buffer to ensure a clean state.
    serial_write("Flushing the PS/2 output buffer.\n");
    inb(controller_data_port);

    // Disable interrupts and translation during init.
    serial_write("Configuring the PS/2 controller.\n");
    outb(controller_command_port, read_first_byte);
    uint8_t configuration = read_controller_response();
    configuration &= ~first_port_interrupt;
    configuration &= ~second_port_interrupt;
    configuration &= ~first_port_clock_disabled;
    outb(controller_command_port, write_first_byte);
    write_controller_data(configuration);

    // Test the first port (we don't care about the second right now).
    serial_write("Testing the first PS/2 port.\n");
    outb(controller_command_port, test_first_port);
    uint8_t first_port_results = read_controller_response();
    if (first_port_results)
    {
        printf(serial_write_char, "The first PS/2 port failed its initialization test. Result %X\n", first_port_results);
        return 1;
    }

    // Enable first port, with interrupts
    serial_write("Enabling the first PS/2 port, with interrupts.\n");
    outb(controller_command_port, enable_first_port);
    configuration |= first_port_interrupt;
    outb(controller_command_port, write_first_byte);
    write_controller_data(configuration);

    // Reset the first port's device
    serial_write("Resetting the keyboard plugged into PS/2 port 1.\n");
    write_controller_data(reset_and_test);
    uint8_t keyboard_result = read_controller_response();
    if (keyboard_result != 0xFA)
    {
        printf(serial_write_char, "The keyboard did not positively acknowledge the reset command. Result %X\n", keyboard_result);
        return 2;
    }

    keyboard_result = read_controller_response();
    if (keyboard_result != 0xAA)
    {
        printf(serial_write_char, "The keyboard's self-test failed. Result %X\n", keyboard_result);
        return 2;
    }

    for (int i = 0; i < 256; i++)
    {
        key_pressed[i] = false;
    }

    reset_scan_code();
    return 0;
}

static char key_code_to_ascii(enum key_code key)
{
    bool shift = key_pressed[KEY_LEFT_SHIFT] || key_pressed[KEY_RIGHT_SHIFT];

    switch (key)
    {
    case 0x02:
        return shift ? '!' : '1';
    case 0x03:
        return shift ? '@' : '2';
    case 0x04:
        return shift ? '#' : '3';
    case 0x05:
        return shift ? '$' : '4';
    case 0x06:
        return shift ? '%' : '5';
    case 0x07:
        return shift ? '^' : '6';
    case 0x08:
        return shift ? '&' : '7';
    case 0x09:
        return shift ? '*' : '8';
    case 0x0A:
        return shift ? '(' : '9';
    case 0x0B:
        return shift ? ')' : '0';
    case 0x0C:
        return shift ? '_' : '-';
    case 0x0D:
        return shift ? '+' : '=';
    case 0x1A:
        return shift ? '{' : '[';
    case 0x1B:
        return shift ? '}' : ']';
    case 0x2B:
        return shift ? '|' : '\\';
    case 0x27:
        return shift ? ':' : ';';
    case 0x28:
        return shift ? '"' : '\'';
    case 0x29:
        return shift ? '~' : '`';
    case 0x33:
        return shift ? '<' : ',';
    case 0x34:
        return shift ? '>' : '.';
    case 0x35:
        return shift ? '?' : '/';
    case 0x10:
        return shift ? 'Q' : 'q';
    case 0x11:
        return shift ? 'W' : 'w';
    case 0x12:
        return shift ? 'E' : 'e';
    case 0x13:
        return shift ? 'R' : 'r';
    case 0x14:
        return shift ? 'T' : 't';
    case 0x15:
        return shift ? 'Y' : 'y';
    case 0x16:
        return shift ? 'U' : 'u';
    case 0x17:
        return shift ? 'I' : 'i';
    case 0x18:
        return shift ? 'O' : 'o';
    case 0x19:
        return shift ? 'P' : 'p';
    case 0x1E:
        return shift ? 'A' : 'a';
    case 0x1F:
        return shift ? 'S' : 's';
    case 0x20:
        return shift ? 'D' : 'd';
    case 0x21:
        return shift ? 'F' : 'f';
    case 0x22:
        return shift ? 'G' : 'g';
    case 0x23:
        return shift ? 'H' : 'h';
    case 0x24:
        return shift ? 'J' : 'j';
    case 0x25:
        return shift ? 'K' : 'k';
    case 0x26:
        return shift ? 'L' : 'l';
    case 0x2C:
        return shift ? 'Z' : 'z';
    case 0x2D:
        return shift ? 'X' : 'x';
    case 0x2E:
        return shift ? 'C' : 'c';
    case 0x2F:
        return shift ? 'V' : 'v';
    case 0x30:
        return shift ? 'B' : 'b';
    case 0x31:
        return shift ? 'N' : 'n';
    case 0x32:
        return shift ? 'M' : 'm';
    case 0x39:
        return ' ';
    case 0x1C:
        return '\n';
    case 0x0F:
        return '\t';
    case 0x0E:
        return '\b';
    default:
        return -1;
    }
}

static void read_scan_code(struct key_event *result)
{
    result->pressed = false;
    result->key = KEY_NONE;

    if (scan_code_pos == 0)
    {
    }

    // Single-byte scan code
    else if (scan_code_pos == 1)
    {
        uint8_t code = in_progress_scan_code[0];
        result->pressed = !(code & 0x80);
        result->key = (enum key_code)(code & 0x7F); // Turn off highest-bit
    }

    // Extended scan code
    else if (scan_code_pos == 2 && in_progress_scan_code[0] == 0xE0)
    {
        uint8_t code = in_progress_scan_code[1];
        result->pressed = !(code & 0x80);
        result->key = (enum key_code)(code | 0x80); // Turn on highest-bit
    }

    // TODO: Add Pause/Break and Print Screen

    result->character = key_code_to_ascii(result->key);
}

__attribute__((interrupt, target("general-regs-only"))) void keyboard_interrupt_handler(__attribute__((unused)) struct interrupt_frame const *frame)
{
    uint8_t scan_code = quick_read_controller_response();
    struct key_event result = {KEY_NONE};

    if (scan_code_pos > 0)
    {
        // We're already in a multi-byte scan code.
        in_progress_scan_code[scan_code_pos++] = scan_code;

        if (in_progress_scan_code[0] == 0xE0 && in_progress_scan_code[1] == 0x2A && scan_code_pos < 4)
        {
            // Partial print screen pressed. Do nothing for now.
        }
        else if (in_progress_scan_code[0] == 0xE0 && in_progress_scan_code[1] == 0xB7 && scan_code_pos < 4)
        {
            // Partial print screen released. Do nothing for now.
        }
        else if (in_progress_scan_code[0] == 0xE1 && scan_code_pos < 6)
        {
            // Partial Pause/Break pressed. Do nothing for now.
        }
        else
        {
            // This is the end of the multi-byte scan code.
            read_scan_code(&result);
            reset_scan_code();
        }
    }
    else if (scan_code == 0xE0 || scan_code == 0xE1)
    {
        // This is the start of multi-byte scan code.
        in_progress_scan_code[scan_code_pos++] = scan_code;
    }
    else
    {
        // This is a complete single-byte scan code.
        in_progress_scan_code[scan_code_pos++] = scan_code;
        read_scan_code(&result);
        reset_scan_code();
    }

    if (result.key != KEY_NONE)
    {
        if (result.pressed)
        {
            printf(serial_write_char, "Key pressed %X\n", result.key);
            if (result.character != -1)
            {
                framebuffer_write_char(result.character);
            }
        }
        else
        {
            printf(serial_write_char, "Key released %X\n", result.key);
        }

        key_pressed[result.key] = result.pressed;
    }

    pic_acknowledge(keyboard_pic_interrupt);
}