/* @(#)crc16.h	1.3 02/03/04 Copyright 1998-2002 J. Schilling */
/*
 *	Q-subchannel CRC definitions
 *
 *	Copyright (c) 1998-2002 J. Schilling
 */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef _CRC16_H_
#define _CRC16_H_

#include <stdlib.h>
//#include <inttypes.h>
//#include <sys/types.h>
#include <SupportDefs.h>

// typedef unsigned int UInt16_t;
// typedef unsigned int uint16;
// typedef unsigned char Uchar;
// typedef unsigned char uchar;
// typedef unsigned int Uint;

uint16 calcCRC(uchar* buf, uint16 bsize);
uint16 fillcrc(uchar* buf, uint bsize);
uint16 flip_crc_error_corr(uchar* b, uint bsize, uint p_crc);

#endif
