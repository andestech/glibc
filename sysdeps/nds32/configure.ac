GLIBC_PROVIDES dnl See aclocal.m4 in the top level source directory.
# Local configure fragment for sysdeps/nds32.
CFLAGS="$CFLAGS -minline-asm-r15"
AC_CACHE_CHECK(whether gcc support option -mno-scalbn-transform,
	       libc_cv_scalbn_transform, [dnl
save_CFLAGS="$CFLAGS"
CFLAGS="$CFLAGS -mno-scalbn-transform"
AC_TRY_COMPILE(,,
	       libc_cv_scalbn_transform=yes,
	       libc_cv_scalbn_transform=no)
CFLAGS="$save_CFLAGS"])
if test "$libc_cv_scalbn_transform" = yes; then
CFLAGS="$CFLAGS -mno-scalbn-transform"
fi
