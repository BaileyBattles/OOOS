/*
 * _start()
 *
 * This is where we land immediately after leaving the bootloader.
 *
 * ATM it's really shaky so don't put code before it ^_^
 */

extern void main();

extern "C" void _start()
{
    main();
    asm volatile("cli; hlt");
}