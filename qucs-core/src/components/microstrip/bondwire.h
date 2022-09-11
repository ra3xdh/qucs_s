/*
 * bondwire.h - bondwire class definitions
 *
 * Copyright (C) 2006 Bastien Roucaries <roucaries.bastien@gmail.com>
 * Copyright (C) 2007, 2008 Stefan Jahn <stefan@lkcc.org>
 *
 * This is free software; you can redistribute it and/or modify
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
 * along with this package; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street - Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 * $Id$
 *
 */

#ifndef __BONDWIRE_H__
#define __BONDWIRE_H__


class bondwire : public qucs::circuit
{
 public:
  CREATOR (bondwire);
  void initSP (void);
  void calcSP (const nr_double_t);
  void calcNoiseSP (nr_double_t);
  void initDC (void);
  void initAC (void);
  void calcAC (nr_double_t);
  void calcNoiseAC (nr_double_t);
  qucs::matrix calcMatrixY (nr_double_t);
  void saveCharacteristics (nr_double_t);

 private:
  void getProperties (void);
  nr_double_t resistance (const nr_double_t f) const;
  nr_double_t Lfreespace (const nr_double_t f) const;
  nr_double_t Lmirror () const;

  nr_double_t l;     /*!< length of bond wire (m) */
  nr_double_t d;     /*!< diameter of bond wire (m) */
  nr_double_t h;     /*!< height from ground plane only used in mirror model */
  nr_double_t rho;   /*!< resistivity */
  nr_double_t mur;   /*!< relative magnetic permeabilty */
  int model;         /*!< model number */
  nr_double_t R, L;
  nr_double_t temp;  /*!< ambient temperature */
};

#endif /* __BONDWIRE_H__ */
