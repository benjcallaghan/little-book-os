#include "interrupts.h"
#include "serial.h"
#include "pic.h"
#include "io.h"
#include "printf.h"
#include <stdint.h>
#include <stddef.h>

constexpr uint8_t keyboard_pic_interrupt = 1;
constexpr uint16_t controller_data_port = 0x60;
constexpr uint16_t controller_command_port = 0x64;
constexpr int largest_scan_code = 6;

uint8_t in_progress_scan_code[largest_scan_code];
size_t scan_code_pos = 0;

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

void reset_scan_code()
{
    for (int i = 0; i < largest_scan_code; i++)
    {
        in_progress_scan_code[i] = 0;
    }
    scan_code_pos = 0;
}

bool input_buffer_full()
{
    return inb(controller_command_port) & 0x02;
}

void write_controller_data(uint8_t data)
{
    while (input_buffer_full())
        ;
    outb(controller_data_port, data);
}

bool output_buffer_empty()
{
    return inb(controller_command_port) & 0x01;
}

uint8_t read_controller_response()
{
    while (!output_buffer_empty())
        ;
    return inb(controller_data_port);
}

uint8_t quick_read_controller_response()
{
    return inb(controller_data_port);
}

int initialize_keyboard()
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

    reset_scan_code();
    return 0;
}

void process_scan_code()
{

}

__attribute__((interrupt, target("general-regs-only"))) void keyboard_interrupt_handler(__attribute__((unused)) struct interrupt_frame const *frame)
{
    uint8_t scan_code = quick_read_controller_response();

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
            printf(serial_write_char, "Complete scan code ");
            for (size_t i = 0; i < scan_code_pos; i++)
            {
                printf(serial_write_char, "%X", in_progress_scan_code[i]);
            }
            printf(serial_write_char, "\n");
            process_scan_code();
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
        printf(serial_write_char, "Complete scan code %X\n", scan_code);
        in_progress_scan_code[scan_code_pos++] = scan_code;
        process_scan_code();
        reset_scan_code();
    }

    pic_acknowledge(keyboard_pic_interrupt);
}