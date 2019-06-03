GLIBC_PROVIDES dnl See aclocal.m4 in the top level source directory.
# Local configure fragment for sysdeps/nds32.

# For nds32 ABI for linux toolchain, the $r15 register is not only an 
# assembler reserved register but also used to passing the sysem call
# number. In order to ensure the data safety, $r15 is included in
# clobber list by default. Using the option -minline-asm-r15 can
# avoid the conflict between asm-specifier for syscall number and
# clobber list when issuing system call by inline assembly.
CFLAGS="$CFLAGS -minline-asm-r15"
