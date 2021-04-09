/* Configuration for GCC for hosting on Windows32.
   using GNU tools and the Windows32 API Library.
   Copyright (C) 1997-2020 Free Software Foundation, Inc.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation; either version 3, or (at your option) any later
version.

GCC is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING3.  If not see
<http://www.gnu.org/licenses/>.  */

#define HOST_EXECUTABLE_SUFFIX ".exe"

#undef PATH_SEPARATOR
#define PATH_SEPARATOR ';'

/* This is the name of the null device on windows.  */
#define HOST_BIT_BUCKET "nul"

/*  The st_ino field of struct stat is always 0.  */
#define HOST_LACKS_INODE_NUMBERS

#ifdef __MINGW32__
/* FIXME: The mingw-w64 crew want us to force a definition of the
 * private __USE_MINGW_ANSI_STDIO feature, but it isn't supposed to
 * be defined directly, (as it is here); furthermore, no such direct
 * definition is required, since the feature is implicitly enabled
 * anyway, when either _GNU_SOURCE or _XOPEN_SOURCE is defined.
 */
#if !defined _GNU_SOURCE && !defined _XOPEN_SOURCE
/* If neither of these implicit enabling features has been selected,
 * we force the explicit definition, recognizing that this may raise
 * a "deprecated usage" warning on MinGW.org implementations.
 */
#undef __USE_MINGW_ANSI_STDIO
#define __USE_MINGW_ANSI_STDIO 1
#endif
#else
/* MSVCRT does not support the "ll" format modifier for printing
 * "long long" values.  Instead, we must use "I64".
 */
#define HOST_LONG_LONG_FORMAT "I64"
#endif
