#  ============================================================================
#   Copyright (C) 2006-2007 Andes Technology, Inc.
#   This file is part of GLIBC and is licensed under the GPL.
#   See the file COPYING for conditions for redistribution.
# 
#   Abstract:
# 
#     This file is for target-dependent code for Andes NDS32 architecture.
# 
#   Revision History:
# 
#     Sep.07.2006     Created by Harry, initial reference from ARM platform.
# 
#   Note:
# 
#     TBD means to-be-done, to-be-develope, whatever.
#     WIP means working-in-progress.
# 
#  ============================================================================
ifeq ($(subdir),misc)
sysdep_headers += sys/cachectl.h
sysdep_routines += cacheflush prctl mremap
endif


ifeq ($(subdir),stdlib)
gen-as-const-headers += ucontext_i.sym
endif

ifeq ($(subdir),elf)
sysdep_routines += dl-vdso
endif
