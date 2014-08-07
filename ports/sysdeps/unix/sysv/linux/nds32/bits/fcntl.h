/* O_*, F_*, FD_* bit values for Linux.
   Copyright (C) 1995, 1996, 1997, 1998, 2000, 2002, 2003, 2004
	Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA.  */

#ifndef _FCNTL_H
# error "Never use <bits/fcntl.h> directly; include <fcntl.h> instead."
#endif

#include <sys/types.h>

/* open/fcntl - O_SYNC is only implemented on blocks devices and on files
   located on an ext2 file system */
#define O_APPEND	0x0008
#define O_SYNC		0x4010
#define O_NONBLOCK	0x0080
#define O_CREAT		0x0100	/* not fcntl */
#define O_TRUNC		0x0200	/* not fcntl */
#define O_EXCL		0x0400	/* not fcntl */
#define O_NOCTTY	0x0800	/* not fcntl */
#define O_ASYNC		0x1000

#define __O_DIRECT       0x8000 /* Direct disk access hint.  */
#define __O_DSYNC        0x0010 /* Synchronize data.  */


#ifdef __USE_LARGEFILE64
# define __O_LARGEFILE  0x2000	/* Allow large file opens.  */
#endif




#ifndef __USE_FILE_OFFSET64
# define F_GETLK	14	/* Get record locking info.  */
# define F_SETLK	6	/* Set record locking info (non-blocking).  */
# define F_SETLKW	7	/* Set record locking info (blocking).	*/
#else
# define F_GETLK	F_GETLK64  /* Get record locking info.	*/
# define F_SETLK	F_SETLK64  /* Set record locking info (non-blocking).*/
# define F_SETLKW	F_SETLKW64 /* Set record locking info (blocking).  */
#endif

#define F_GETLK64	33	/* Get record locking info.  */
#define F_SETLK64	34	/* Set record locking info (non-blocking).  */
#define F_SETLKW64	35	/* Set record locking info (blocking).	*/

#if defined __USE_BSD || defined __USE_UNIX98
# define F_SETOWN	24	/* Get owner of socket (receiver of SIGIO).  */
# define F_GETOWN	23	/* Set owner of socket (receiver of SIGIO).  */
#endif

typedef struct flock
  {
    short int l_type;	/* Type of lock: F_RDLCK, F_WRLCK, or F_UNLCK.	*/
    short int l_whence;	/* Where `l_start' is relative to (like `lseek').  */
#ifndef __USE_FILE_OFFSET64
    __off_t l_start;	/* Offset where the lock begins.  */
    __off_t l_len;	/* Size of the locked area; zero means until EOF.  */
#else
    __off64_t l_start;	/* Offset where the lock begins.  */
    __off64_t l_len;	/* Size of the locked area; zero means until EOF.  */
#endif
    __pid_t l_pid;	/* Process holding the lock.  */
} flock_t;

#ifdef __USE_LARGEFILE64
struct flock64
  {
    short int l_type;	/* Type of lock: F_RDLCK, F_WRLCK, or F_UNLCK.	*/
    short int l_whence;	/* Where `l_start' is relative to (like `lseek').  */
    __off64_t l_start;	/* Offset where the lock begins.  */
    __off64_t l_len;	/* Size of the locked area; zero means until EOF.  */
    __pid_t l_pid;	/* Process holding the lock.  */
  };
#endif


/* Include generic Linux declarations.  */
#include <bits/fcntl-linux.h>

