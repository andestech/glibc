ifeq ($(subdir),elf)
sysdep-dl-routines += tlsdesc dl-tlsdesc
sysdep_routines += tlsdesc dl-tlsdesc
sysdep-rtld-routines += tlsdesc dl-tlsdesc
endif

ASFLAGS-.os += $(pic-ccflag)

ifeq ($(subdir),csu)
gen-as-const-headers += tlsdesc.sym
endif
