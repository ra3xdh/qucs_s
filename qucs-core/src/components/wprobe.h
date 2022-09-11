/*
 * wprobe.h - AC/DC and transient watt probe class definitions
 *
 * Copyright (C) 2015, Pedro Macedo
 *		 2017, Alberto Silva, 1120196@isep.ipp.pt	
 *		
 * This file is part of Qucs
 *
 * Qucs is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Qucs.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

//Taking vprobe as a basis

#ifndef __WPROBE_H__
#define __WPROBE_H__

class wprobe : public qucs::circuit
{
 public:
  CREATOR (wprobe);
  void initDC (void);
  void initAC (void);
  void initTR (void);
  void saveOperatingPoints (void);
  void calcOperatingPoints (void);
};

#endif /* __WPROBE_H__ */
