#ifndef _LINKMAP_H
#define _LINKMAP_H
struct link_map_machine
  {
    ElfW(Addr) plt; /* Address of .plt + 0x28 */
    ElfW(Addr) gotplt; /* Address of .got + 0x0c */
    void *tlsdesc_table;
  };

#endif

