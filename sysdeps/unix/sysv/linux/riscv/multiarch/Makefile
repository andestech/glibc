ifeq ($(subdir),string)
sysdep_routines += \
  memchr \
  memchr-generic \
  memcpy \
  memcpy-generic \
  memcpy_noalignment \
  memrchr \
  memrchr-generic \
  strchrnul \
  strchrnul-generic \
  strcmp \
  strcmp-generic \
  strlen \
  strlen-generic \
  strncmp \
  strncmp-generic \
  # sysdep_routines

ifeq ($(have-fattribute-zbb-support),yes)
sysdep_routines += \
  memchr-zbb \
  memrchr-zbb \
  strchrnul-zbb \
  strcmp-zbb \
  strlen-zbb \
  strncmp-zbb \
  # Zbb sysdep_routines
endif

CFLAGS-memcpy_noalignment.c += -mno-strict-align
endif
