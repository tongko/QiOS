/*-- QiOS ---------------------------------------------------------------------*
 *                                                                             *
 *	Copyright (C) 2017, 2018 Liew Tze Wei                                      *
 *                                                                             *
 * 	This file is part of the QiOS kernel, and is made available under the      *
 *  terms of The Unlicense (That means just do whatever you want with the code *
 *  base).                                                                     *
 * 																			   *
 *----------------------------------------------------------------------------*/
#ifndef __UNISTD_H_
#define __UNISTD_H_

#include <bits/types.h>
#include <sys/cdefs.h>

__BEGIN_DECLS

#ifndef __pid_t_defined
typedef __pid_t pid_t;
#define __pid_t_defined
#endif

/* Replace the current process, executing PATH with arguments ARGV and
   environment ENVP.  ARGV and ENVP are terminated by NULL pointers.  */
extern int execve(const char *path, char *const argv[],
                  char *const envp[]) __THROW __nonnull((1, 2));

/* Execute PATH with arguments ARGV and environment from `environ'.  */
extern int execv(const char *path, char *const argv[])
    __THROW __nonnull((1, 2));

/* Execute FILE, searching in the `PATH' environment variable if it contains
   no slashes, with arguments ARGV and environment from `environ'.  */
extern int execvp(const char *file, char *const argv[])
    __THROW __nonnull((1, 2));

/* Clone the calling process, creating an exact copy.
   Return -1 for errors, 0 to the new process,
   and the process ID of the new process to the old process.  */
extern __pid_t fork(void) __THROWNL;

__END_DECLS

#endif  //	__UNISTD_H_