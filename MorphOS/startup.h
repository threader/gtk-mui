/*****************************************************************************
 * 
 * mui-gtk - a wrapper library to wrap GTK+ calls to MUI
 *
 * Copyright (C) 2006 Ilkka Lehtoranta
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 * 
 * Contact information:
 *
 * Ilkka Lehtoranta
 *
 * E-Mail: ilkleht "at" isoveli.org
 *
 * $Id: startup.h,v 1.1 2006/01/02 23:06:26 itix Exp $
 *
 *****************************************************************************/

#ifndef __STARTUP_H__
#define __STARTUP_H__

ULONG libnix_start(APTR base);
VOID  libnix_finish(APTR base);

#endif /* __STARTUP_H__ */