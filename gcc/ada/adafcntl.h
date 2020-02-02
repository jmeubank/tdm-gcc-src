/****************************************************************************
 *                                                                          *
 *                         GNAT COMPILER COMPONENTS                         *
 *                                                                          *
 *                             A D A F C N T L                              *
 *                                                                          *
 *                              C Header File                               *
 *                                                                          *
 *          Copyright (C) 2002-2018, Free Software Foundation, Inc.         *
 *                                                                          *
 * GNAT is free software;  you can  redistribute it  and/or modify it under *
 * terms of the  GNU General Public License as published  by the Free Soft- *
 * ware  Foundation;  either version 3,  or (at your option) any later ver- *
 * sion.  GNAT is distributed in the hope that it will be useful, but WITH- *
 * OUT ANY WARRANTY;  without even the  implied warranty of MERCHANTABILITY *
 * or FITNESS FOR A PARTICULAR PURPOSE.                                     *
 *                                                                          *
 * As a special exception under Section 7 of GPL version 3, you are granted *
 * additional permissions described in the GCC Runtime Library Exception,   *
 * version 3.1, as published by the Free Software Foundation.               *
 *                                                                          *
 * You should have received a copy of the GNU General Public License and    *
 * a copy of the GCC Runtime Library Exception along with this program;     *
 * see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see    *
 * <http://www.gnu.org/licenses/>.                                          *
 *                                                                          *
 * GNAT was originally developed  by the GNAT team at  New York University. *
 * Extensive contributions were provided by Ada Core Technologies Inc.      *
 *                                                                          *
 ****************************************************************************/

/*  This file provides a wrapper around the system <fcntl.h> header,
 *  for use in situations where MINGW32 builds may require fall-back
 *  support for some Unicode specific constants, when compiling for
 *  use on legacy (pre-Vista) MS-Windows-32.  */

#ifndef __ADAFCNTL_H_
#define __ADAFCNTL_H_

#include <fcntl.h>

#ifdef __MINGW32__

#ifndef _O_U8TEXT
#define _O_U8TEXT   _O_TEXT
#endif

#ifndef _O_U16TEXT
#define _O_U16TEXT  _O_TEXT
#endif

#ifndef _O_WTEXT
#define _O_WTEXT    _O_TEXT
#endif

#endif	/* __MINGW32__ */
#endif	/* !__ADAFCNTL_H_ */
