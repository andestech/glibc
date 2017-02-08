#ifndef _LINKMAP_H
#define _LINKMAP_H
struct link_map_machine
  {
    Elf32_Addr plt; /* Address of .plt + 0x28 */
    Elf32_Addr gotplt; /* Address of .got + 0x0c */
    void *tlsdesc_table;
  };

#endif

