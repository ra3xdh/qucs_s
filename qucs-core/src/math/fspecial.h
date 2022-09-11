/*
 * fspecial.h - special functions definitions
 *
 * Copyright (C) 2006 Stefan Jahn <stefan@lkcc.org>
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

#ifndef __FSPECIAL_H__
#define __FSPECIAL_H__

namespace fspecial {

//#ifndef HAVE_ERF
  nr_double_t     erf (nr_double_t);
//#endif
//#ifndef HAVE_ERFC
  nr_double_t    erfc (nr_double_t);
//#endif
  nr_double_t  erfinv (nr_double_t);
  nr_double_t erfcinv (nr_double_t);
  nr_double_t ltqnorm (nr_double_t);
  nr_double_t      i0 (nr_double_t);

  void        ellip_ke (nr_double_t, nr_double_t&, nr_double_t&);
  nr_double_t ellip_rf (nr_double_t, nr_double_t, nr_double_t);
  nr_double_t ellip_sncndn (nr_double_t, nr_double_t,
			    nr_double_t&, nr_double_t&, nr_double_t&);

} // namespace

#endif /* __FSPECIAL_H__ */
