ifeq ($(subdir),elf)
sysdep-dl-routines += tlsdesc dl-tlsdesc
sysdep_routines += tlsdesc dl-tlsdesc
sysdep-rtld-routines += tlsdesc dl-tlsdesc
endif

ifeq ($(subdir),csu)
gen-as-const-headers += tlsdesc.sym
endif

ifeq ($(subdir),stdlib)
# Avoid compiler calling abort() function when encountering
# divde-by-zero case.
CFLAGS-divrem.c += -fno-isolate-erroneous-paths-dereference
endif
