ifeq ($(subdir),nptl)
ifeq ($(NDS32_HOST_CYGWIN),yes)
CFLAGS-pthread_spin_lock.c=-DNDS32_HOST_CYGWIN
endif
endif

ifeq ($(subdir),elf)
sysdep-dl-routines += tlsdesc dl-tlsdesc
sysdep_routines += tlsdesc dl-tlsdesc
sysdep-rtld-routines += tlsdesc dl-tlsdesc
endif

ifeq ($(subdir),csu)
gen-as-const-headers += tlsdesc.sym
endif
