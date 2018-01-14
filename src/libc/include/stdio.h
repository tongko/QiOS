/*-- QiOS ---------------------------------------------------------------------*
 *                                                                             *
 *	Copyright (C) 2017, 2018 Liew Tze Wei                                      *
 *                                                                             *
 * 	This file is part of the QiOS kernel, and is made available under the      *
 *  terms of The Unlicense (That means just do whatever you want with the code *
 *  base).                                                                     *
 * 																			   *
 *----------------------------------------------------------------------------*/
#ifndef __STDIO_H_
#define __STDIO_H_

#include <stdarg.h>

/* The possibilities for the third argument to `fseek'.
   These values should not be changed.  */
#define SEEK_SET 0 /* Seek from beginning of file.  */
#define SEEK_CUR 1 /* Seek from current position.  */
#define SEEK_END 2 /* Seek from end of file.  */

/* Opaque structure to hide the complexity */
struct _IO_FILE;
#define FILE struct _IO_FILE

/*	Standard stram	*/
extern FILE *stdin;
extern FILE *stdout;
extern FILE *stderr;
/* should this be macro or fields? */
#define stdin stdin
#define stdout stdout
#define stderr stderr

/* Close STREAM.

   This function is a possible cancellation point and therefore not
   marked with __THROW.  */
extern int fclose(FILE *stream);

/* Flush STREAM, or all streams if STREAM is NULL.

   This function is a possible cancellation point and therefore not
   marked with __THROW.  */
extern int fflush(FILE *stream);

/* Open a file and create a new stream for it.

   This function is a possible cancellation point and therefore not
   marked with __THROW.  */
extern FILE *fopen(const char *filename,
                   const char *modes) __wur;

/* If BUF is NULL, make STREAM unbuffered.
   Else make it use buffer BUF, of size BUFSIZ.  */
extern void setbuf(FILE *stream, char *buf) __THROW;

/* Write formatted output to STREAM.

   This function is a possible cancellation point and therefore not
   marked with __THROW.  */
extern int fprintf(FILE *stream, const char *format, ...);

/* Write formatted output to S from argument list ARG.

   This function is a possible cancellation point and therefore not
   marked with __THROW.  */
extern int vfprintf(FILE *stream, const char *format, va_list __arg);

/* Read chunks of generic data from STREAM.

   This function is a possible cancellation point and therefore not
   marked with __THROW.  */
extern size_t fread(void *ptr, size_t size,
                    size_t n, FILE *stream) __wur;

/* Write chunks of generic data to STREAM.

   This function is a possible cancellation point and therefore not
   marked with __THROW.  */
extern size_t fwrite(const void *ptr, size_t size,
                     size_t n, FILE *stream);

/* Seek to a certain position on STREAM.

   This function is a possible cancellation point and therefore not
   marked with __THROW.  */
extern int fseek(FILE *stream, long int off, int whence);

/* Return the current position of STREAM.

   This function is a possible cancellation point and therefore not
   marked with __THROW.  */
extern long int ftell(FILE *stream) __wur;

#endif  //	__STDIO_H_