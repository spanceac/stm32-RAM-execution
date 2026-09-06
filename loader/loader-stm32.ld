MEMORY
{
  ram (rwx) : ORIGIN = 0x20000000, LENGTH = 512
  rom (rx)  : ORIGIN = 0x08000000, LENGTH = 128K
}
SECTIONS
{
    PROVIDE(ram_app_load_addr = ORIGIN(ram) + LENGTH(ram));
    PROVIDE(_stack_top = ORIGIN(ram) + LENGTH(ram));

    .  = 0x08000000;         /* From 0x08000000 */
    .text :
    {
        *(vectors)    /* Vector table */
        *(.text)      /* Program code */
        *(.rodata)    /* Read only data */
    } >rom

    .  = 0x20000000;  /* From 0x20000000 */      
    .data :
    {
        *(.data)      /* Data memory */
    } >ram AT > rom

    /* probably can be removed */
    .bss :
    {
        *(.bss)       /* Zero-filled run time allocate data memory */
    } >ram AT > rom
} 
