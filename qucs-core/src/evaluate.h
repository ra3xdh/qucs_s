/*
 * evaluate.h - definitions for Qucs equation evaluations
 *
 * Copyright (C) 2004-2011 Stefan Jahn <stefan@lkcc.org>
 * Copyright (C) 2006 Gunther Kraut <gn.kraut@t-online.de>
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

#ifndef __EVALUATE_H__
#define __EVALUATE_H__

namespace qucs {

namespace eqn {

class constant;

/* This class is merely a container for the applications which can be
   applied to equation constants. */
class evaluate
{
public:
  static constant * plus_d (constant *);
  static constant * plus_c (constant *);
  static constant * plus_v (constant *);
  static constant * plus_m (constant *);
  static constant * plus_mv (constant *);

  static constant * plus_d_d (constant *);
  static constant * plus_c_c (constant *);
  static constant * plus_c_d (constant *);
  static constant * plus_d_c (constant *);
  static constant * plus_v_d (constant *);
  static constant * plus_d_v (constant *);
  static constant * plus_v_c (constant *);
  static constant * plus_c_v (constant *);
  static constant * plus_v_v (constant *);
  static constant * plus_m_m (constant *);
  static constant * plus_m_d (constant *);
  static constant * plus_d_m (constant *);
  static constant * plus_m_c (constant *);
  static constant * plus_c_m (constant *);
  static constant * plus_s_s (constant *);
  static constant * plus_c_s (constant *);
  static constant * plus_s_c (constant *);
  static constant * plus_mv_mv (constant *);
  static constant * plus_mv_m (constant *);
  static constant * plus_m_mv (constant *);
  static constant * plus_mv_d (constant *);
  static constant * plus_d_mv (constant *);
  static constant * plus_mv_c (constant *);
  static constant * plus_c_mv (constant *);
  static constant * plus_mv_v (constant *);
  static constant * plus_v_mv (constant *);

  static constant * minus_d (constant *);
  static constant * minus_c (constant *);
  static constant * minus_v (constant *);
  static constant * minus_m (constant *);
  static constant * minus_mv (constant *);

  static constant * minus_d_d (constant *);
  static constant * minus_c_c (constant *);
  static constant * minus_c_d (constant *);
  static constant * minus_d_c (constant *);
  static constant * minus_v_d (constant *);
  static constant * minus_d_v (constant *);
  static constant * minus_v_c (constant *);
  static constant * minus_c_v (constant *);
  static constant * minus_v_v (constant *);
  static constant * minus_m_m (constant *);
  static constant * minus_m_d (constant *);
  static constant * minus_d_m (constant *);
  static constant * minus_m_c (constant *);
  static constant * minus_c_m (constant *);
  static constant * minus_mv_mv (constant *);
  static constant * minus_mv_m (constant *);
  static constant * minus_m_mv (constant *);
  static constant * minus_mv_d (constant *);
  static constant * minus_d_mv (constant *);
  static constant * minus_mv_c (constant *);
  static constant * minus_c_mv (constant *);
  static constant * minus_mv_v (constant *);
  static constant * minus_v_mv (constant *);

  static constant * times_d_d (constant *);
  static constant * times_c_c (constant *);
  static constant * times_c_d (constant *);
  static constant * times_d_c (constant *);
  static constant * times_v_d (constant *);
  static constant * times_d_v (constant *);
  static constant * times_v_c (constant *);
  static constant * times_c_v (constant *);
  static constant * times_v_v (constant *);
  static constant * times_m_m (constant *);
  static constant * times_m_c (constant *);
  static constant * times_c_m (constant *);
  static constant * times_m_d (constant *);
  static constant * times_d_m (constant *);
  static constant * times_mv_mv (constant *);
  static constant * times_mv_c (constant *);
  static constant * times_c_mv (constant *);
  static constant * times_mv_d (constant *);
  static constant * times_d_mv (constant *);
  static constant * times_mv_m (constant *);
  static constant * times_m_mv (constant *);
  static constant * times_mv_v (constant *);
  static constant * times_v_mv (constant *);

  static constant * over_d_d (constant *);
  static constant * over_c_c (constant *);
  static constant * over_c_d (constant *);
  static constant * over_d_c (constant *);
  static constant * over_v_d (constant *);
  static constant * over_d_v (constant *);
  static constant * over_v_c (constant *);
  static constant * over_c_v (constant *);
  static constant * over_v_v (constant *);
  static constant * over_m_c (constant *);
  static constant * over_m_d (constant *);
  static constant * over_mv_c (constant *);
  static constant * over_mv_d (constant *);
  static constant * over_mv_v (constant *);

  static constant * modulo_d_d (constant *);
  static constant * modulo_c_c (constant *);
  static constant * modulo_c_d (constant *);
  static constant * modulo_d_c (constant *);
  static constant * modulo_v_d (constant *);
  static constant * modulo_d_v (constant *);
  static constant * modulo_v_c (constant *);
  static constant * modulo_c_v (constant *);
  static constant * modulo_v_v (constant *);

  static constant * power_d_d (constant *);
  static constant * power_c_c (constant *);
  static constant * power_c_d (constant *);
  static constant * power_d_c (constant *);
  static constant * power_v_d (constant *);
  static constant * power_d_v (constant *);
  static constant * power_v_c (constant *);
  static constant * power_c_v (constant *);
  static constant * power_v_v (constant *);
  static constant * power_m_d (constant *);
  static constant * power_m_c (constant *);
  static constant * power_mv_d (constant *);
  static constant * power_mv_c (constant *);
  static constant * power_mv_v (constant *);

  static constant * xhypot_d_d (constant *);
  static constant * xhypot_c_c (constant *);
  static constant * xhypot_c_d (constant *);
  static constant * xhypot_d_c (constant *);
  static constant * xhypot_v_d (constant *);
  static constant * xhypot_d_v (constant *);
  static constant * xhypot_v_c (constant *);
  static constant * xhypot_c_v (constant *);
  static constant * xhypot_v_v (constant *);

  static constant * real_d (constant *);
  static constant * real_c (constant *);
  static constant * real_v (constant *);
  static constant * real_m (constant *);
  static constant * real_mv (constant *);

  static constant * imag_d (constant *);
  static constant * imag_c (constant *);
  static constant * imag_v (constant *);
  static constant * imag_m (constant *);
  static constant * imag_mv (constant *);

  static constant * abs_d (constant *);
  static constant * abs_c (constant *);
  static constant * abs_v (constant *);
  static constant * abs_m (constant *);
  static constant * abs_mv (constant *);

  static constant * conj_d (constant *);
  static constant * conj_c (constant *);
  static constant * conj_v (constant *);
  static constant * conj_m (constant *);
  static constant * conj_mv (constant *);

  static constant * norm_d (constant *);
  static constant * norm_c (constant *);
  static constant * norm_v (constant *);

  static constant * phase_d (constant *);
  static constant * phase_c (constant *);
  static constant * phase_v (constant *);
  static constant * phase_m (constant *);
  static constant * phase_mv (constant *);

  static constant * arg_d (constant *);
  static constant * arg_c (constant *);
  static constant * arg_v (constant *);
  static constant * arg_m (constant *);
  static constant * arg_mv (constant *);

  static constant * unwrap_v_1 (constant *);
  static constant * unwrap_v_2 (constant *);
  static constant * unwrap_v_3 (constant *);

  static constant * deg2rad_d (constant *);
  static constant * deg2rad_c (constant *);
  static constant * deg2rad_v (constant *);
  static constant * rad2deg_d (constant *);
  static constant * rad2deg_c (constant *);
  static constant * rad2deg_v (constant *);

  static constant * dB_d (constant *);
  static constant * dB_c (constant *);
  static constant * dB_v (constant *);
  static constant * dB_m (constant *);
  static constant * dB_mv (constant *);

  static constant * sqrt_d (constant *);
  static constant * sqrt_c (constant *);
  static constant * sqrt_v (constant *);

  static constant * exp_d (constant *);
  static constant * exp_c (constant *);
  static constant * exp_v (constant *);

  static constant * limexp_d (constant *);
  static constant * limexp_c (constant *);
  static constant * limexp_v (constant *);

  static constant * ln_d (constant *);
  static constant * ln_c (constant *);
  static constant * ln_v (constant *);

  static constant * log10_d (constant *);
  static constant * log10_c (constant *);
  static constant * log10_v (constant *);

  static constant * log2_d (constant *);
  static constant * log2_c (constant *);
  static constant * log2_v (constant *);

  static constant * sin_d (constant *);
  static constant * sin_c (constant *);
  static constant * sin_v (constant *);

  static constant * arcsin_d (constant *);
  static constant * arcsin_c (constant *);
  static constant * arcsin_v (constant *);

  static constant * cos_d (constant *);
  static constant * cos_c (constant *);
  static constant * cos_v (constant *);

  static constant * arccos_d (constant *);
  static constant * arccos_c (constant *);
  static constant * arccos_v (constant *);

  static constant * tan_d (constant *);
  static constant * tan_c (constant *);
  static constant * tan_v (constant *);

  static constant * arctan_d (constant *);
  static constant * arctan_c (constant *);
  static constant * arctan_v (constant *);

  static constant * cot_d (constant *);
  static constant * cot_c (constant *);
  static constant * cot_v (constant *);

  static constant * arccot_d (constant *);
  static constant * arccot_c (constant *);
  static constant * arccot_v (constant *);

  static constant * sec_d (constant *);
  static constant * sec_c (constant *);
  static constant * sec_v (constant *);

  static constant * arcsec_d (constant *);
  static constant * arcsec_c (constant *);
  static constant * arcsec_v (constant *);

  static constant * cosec_d (constant *);
  static constant * cosec_c (constant *);
  static constant * cosec_v (constant *);

  static constant * arccosec_d (constant *);
  static constant * arccosec_c (constant *);
  static constant * arccosec_v (constant *);

  static constant * sinh_d (constant *);
  static constant * sinh_c (constant *);
  static constant * sinh_v (constant *);

  static constant * arsinh_d (constant *);
  static constant * arsinh_c (constant *);
  static constant * arsinh_v (constant *);

  static constant * cosh_d (constant *);
  static constant * cosh_c (constant *);
  static constant * cosh_v (constant *);

  static constant * arcosh_d (constant *);
  static constant * arcosh_c (constant *);
  static constant * arcosh_v (constant *);

  static constant * tanh_d (constant *);
  static constant * tanh_c (constant *);
  static constant * tanh_v (constant *);

  static constant * artanh_d (constant *);
  static constant * artanh_c (constant *);
  static constant * artanh_v (constant *);

  static constant * coth_d (constant *);
  static constant * coth_c (constant *);
  static constant * coth_v (constant *);

  static constant * arcoth_d (constant *);
  static constant * arcoth_c (constant *);
  static constant * arcoth_v (constant *);

  static constant * sech_d (constant *);
  static constant * sech_c (constant *);
  static constant * sech_v (constant *);

  static constant * arsech_d (constant *);
  static constant * arsech_c (constant *);
  static constant * arsech_v (constant *);

  static constant * cosech_d (constant *);
  static constant * cosech_c (constant *);
  static constant * cosech_v (constant *);

  static constant * arcosech_d (constant *);
  static constant * arcosech_c (constant *);
  static constant * arcosech_v (constant *);

  static constant * ztor_d   (constant *);
  static constant * ztor_d_d (constant *);
  static constant * ztor_d_c (constant *);
  static constant * ztor_c   (constant *);
  static constant * ztor_c_d (constant *);
  static constant * ztor_c_c (constant *);
  static constant * ztor_v   (constant *);
  static constant * ztor_v_d (constant *);
  static constant * ztor_v_c (constant *);

  static constant * rtoz_d   (constant *);
  static constant * rtoz_d_d (constant *);
  static constant * rtoz_d_c (constant *);
  static constant * rtoz_c   (constant *);
  static constant * rtoz_c_d (constant *);
  static constant * rtoz_c_c (constant *);
  static constant * rtoz_v   (constant *);
  static constant * rtoz_v_d (constant *);
  static constant * rtoz_v_c (constant *);

  static constant * ytor_d   (constant *);
  static constant * ytor_d_d (constant *);
  static constant * ytor_d_c (constant *);
  static constant * ytor_c   (constant *);
  static constant * ytor_c_d (constant *);
  static constant * ytor_c_c (constant *);
  static constant * ytor_v   (constant *);
  static constant * ytor_v_d (constant *);
  static constant * ytor_v_c (constant *);

  static constant * rtoy_d   (constant *);
  static constant * rtoy_d_d (constant *);
  static constant * rtoy_d_c (constant *);
  static constant * rtoy_c   (constant *);
  static constant * rtoy_c_d (constant *);
  static constant * rtoy_c_c (constant *);
  static constant * rtoy_v   (constant *);
  static constant * rtoy_v_d (constant *);
  static constant * rtoy_v_c (constant *);

  static constant * rtoswr_d (constant *);
  static constant * rtoswr_c (constant *);
  static constant * rtoswr_v (constant *);

  static constant * diff_v_2 (constant *);
  static constant * diff_v_3 (constant *);

  static constant * max_d (constant *);
  static constant * max_c (constant *);
  static constant * max_v (constant *);
  static constant * max_r (constant *);

  static constant * max_d_d (constant *);
  static constant * max_c_d (constant *);
  static constant * max_d_c (constant *);
  static constant * max_c_c (constant *);

  static constant * min_d (constant *);
  static constant * min_c (constant *);
  static constant * min_v (constant *);
  static constant * min_r (constant *);

  static constant * min_d_d (constant *);
  static constant * min_c_d (constant *);
  static constant * min_d_c (constant *);
  static constant * min_c_c (constant *);

  static constant * sum_d (constant *);
  static constant * sum_c (constant *);
  static constant * sum_v (constant *);

  static constant * prod_d (constant *);
  static constant * prod_c (constant *);
  static constant * prod_v (constant *);

  static constant * avg_d (constant *);
  static constant * avg_c (constant *);
  static constant * avg_v (constant *);
  static constant * avg_r (constant *);

  static constant * signum_d (constant *);
  static constant * signum_c (constant *);
  static constant * signum_v (constant *);

  static constant * sign_d (constant *);
  static constant * sign_c (constant *);
  static constant * sign_v (constant *);

  static constant * sinc_d (constant *);
  static constant * sinc_c (constant *);
  static constant * sinc_v (constant *);

  static constant * length_d (constant *);
  static constant * length_c (constant *);
  static constant * length_v (constant *);
  static constant * length_m (constant *);
  static constant * length_mv (constant *);

  static void extract_vector (constant *, int, int &, int &, constant *);
  static constant * index_mv_2 (constant *);
  static constant * index_mv_1 (constant *);
  static constant * index_v_1 (constant *);
  static constant * index_v_2 (constant *);
  static constant * index_m_2 (constant *);
  static constant * index_s_1 (constant *);

  static constant * stos_m_d   (constant *);
  static constant * stos_m_d_d (constant *);
  static constant * stos_m_d_c (constant *);
  static constant * stos_m_d_v (constant *);
  static constant * stos_m_c   (constant *);
  static constant * stos_m_c_d (constant *);
  static constant * stos_m_c_c (constant *);
  static constant * stos_m_c_v (constant *);
  static constant * stos_m_v   (constant *);
  static constant * stos_m_v_d (constant *);
  static constant * stos_m_v_c (constant *);
  static constant * stos_m_v_v (constant *);

  static constant * stos_mv_d   (constant *);
  static constant * stos_mv_d_d (constant *);
  static constant * stos_mv_d_c (constant *);
  static constant * stos_mv_d_v (constant *);
  static constant * stos_mv_c   (constant *);
  static constant * stos_mv_c_d (constant *);
  static constant * stos_mv_c_c (constant *);
  static constant * stos_mv_c_v (constant *);
  static constant * stos_mv_v   (constant *);
  static constant * stos_mv_v_d (constant *);
  static constant * stos_mv_v_c (constant *);
  static constant * stos_mv_v_v (constant *);

  static constant * stoy_m    (constant *);
  static constant * stoy_m_d  (constant *);
  static constant * stoy_m_c  (constant *);
  static constant * stoy_m_v  (constant *);
  static constant * stoy_mv   (constant *);
  static constant * stoy_mv_d (constant *);
  static constant * stoy_mv_c (constant *);
  static constant * stoy_mv_v (constant *);
  static constant * stoz_m    (constant *);
  static constant * stoz_m_d  (constant *);
  static constant * stoz_m_c  (constant *);
  static constant * stoz_m_v  (constant *);
  static constant * stoz_mv   (constant *);
  static constant * stoz_mv_d (constant *);
  static constant * stoz_mv_c (constant *);
  static constant * stoz_mv_v (constant *);
  static constant * ytos_m    (constant *);
  static constant * ytos_m_d  (constant *);
  static constant * ytos_m_c  (constant *);
  static constant * ytos_m_v  (constant *);
  static constant * ytos_mv   (constant *);
  static constant * ytos_mv_d (constant *);
  static constant * ytos_mv_c (constant *);
  static constant * ytos_mv_v (constant *);
  static constant * ztos_m    (constant *);
  static constant * ztos_m_d  (constant *);
  static constant * ztos_m_c  (constant *);
  static constant * ztos_m_v  (constant *);
  static constant * ztos_mv   (constant *);
  static constant * ztos_mv_d (constant *);
  static constant * ztos_mv_c (constant *);
  static constant * ztos_mv_v (constant *);

  static constant * ztoy_m  (constant *);
  static constant * ztoy_mv (constant *);
  static constant * ytoz_m  (constant *);
  static constant * ytoz_mv (constant *);

  static constant * twoport_m  (constant *);
  static constant * twoport_mv (constant *);
  static constant * inverse_m  (constant *);
  static constant * inverse_mv (constant *);
  static constant * transpose_m  (constant *);
  static constant * transpose_mv (constant *);
  static constant * det_m  (constant *);
  static constant * det_mv (constant *);
  static constant * eye_m  (constant *);
  static constant * adjoint_m  (constant *);
  static constant * adjoint_mv (constant *);

  static constant * rollet_m  (constant *);
  static constant * rollet_mv (constant *);
  static constant * mu1_m  (constant *);
  static constant * mu1_mv (constant *);
  static constant * mu2_m  (constant *);
  static constant * mu2_mv (constant *);
  static constant * b1_m   (constant *);
  static constant * b1_mv  (constant *);

  static constant * linspace (constant *);
  static constant * logspace (constant *);
  static constant * noise_circle_d   (constant *);
  static constant * noise_circle_d_d (constant *);
  static constant * noise_circle_d_v (constant *);
  static constant * noise_circle_v   (constant *);
  static constant * noise_circle_v_d (constant *);
  static constant * noise_circle_v_v (constant *);
  static constant * stab_circle_l   (constant *);
  static constant * stab_circle_l_d (constant *);
  static constant * stab_circle_l_v (constant *);
  static constant * stab_circle_s   (constant *);
  static constant * stab_circle_s_d (constant *);
  static constant * stab_circle_s_v (constant *);
  static constant * ga_circle_d   (constant *);
  static constant * ga_circle_d_d (constant *);
  static constant * ga_circle_d_v (constant *);
  static constant * ga_circle_v   (constant *);
  static constant * ga_circle_v_d (constant *);
  static constant * ga_circle_v_v (constant *);
  static constant * gp_circle_d   (constant *);
  static constant * gp_circle_d_d (constant *);
  static constant * gp_circle_d_v (constant *);
  static constant * gp_circle_v   (constant *);
  static constant * gp_circle_v_d (constant *);
  static constant * gp_circle_v_v (constant *);
  static constant * plot_vs_v  (constant *);
  static constant * plot_vs_mv (constant *);

  static constant * interpolate_v_v_d (constant *);
  static constant * interpolate_v_v (constant *);

  static constant * fft_v (constant *);
  static constant * ifft_v (constant *);
  static constant * dft_v (constant *);
  static constant * idft_v (constant *);
  static constant * time2freq_v_v (constant *);
  static constant * freq2time_v_v (constant *);
  static constant * receiver_v_v (constant *);
  static constant * fftshift_v (constant *);

  static constant * xvalue_d (constant *);
  static constant * xvalue_c (constant *);
  static constant * yvalue_d (constant *);
  static constant * yvalue_c (constant *);

  static constant * range_d_d (constant *);
  static constant * range_d_c (constant *);
  static constant * range_c_d (constant *);
  static constant * range_c_c (constant *);

  static constant * ceil_d   (constant *);
  static constant * ceil_c   (constant *);
  static constant * ceil_v   (constant *);
  static constant * floor_d  (constant *);
  static constant * floor_c  (constant *);
  static constant * floor_v  (constant *);
  static constant * fix_d    (constant *);
  static constant * fix_c    (constant *);
  static constant * fix_v    (constant *);
  static constant * step_d   (constant *);
  static constant * step_c   (constant *);
  static constant * step_v   (constant *);
  static constant * round_d  (constant *);
  static constant * round_c  (constant *);
  static constant * round_v  (constant *);

  static constant * erf_d     (constant *);
  static constant * erf_c     (constant *);
  static constant * erf_v     (constant *);
  static constant * erfc_d    (constant *);
  static constant * erfc_c    (constant *);
  static constant * erfc_v    (constant *);
  static constant * erfinv_d  (constant *);
  static constant * erfinv_c  (constant *);
  static constant * erfinv_v  (constant *);
  static constant * erfcinv_d (constant *);
  static constant * erfcinv_c (constant *);
  static constant * erfcinv_v (constant *);

  static constant * rms_d      (constant *);
  static constant * rms_c      (constant *);
  static constant * rms_v      (constant *);
  static constant * variance_d (constant *);
  static constant * variance_c (constant *);
  static constant * variance_v (constant *);
  static constant * stddev_d   (constant *);
  static constant * stddev_c   (constant *);
  static constant * stddev_v   (constant *);

  static constant * cumsum_d  (constant *);
  static constant * cumsum_c  (constant *);
  static constant * cumsum_v  (constant *);
  static constant * cumavg_d  (constant *);
  static constant * cumavg_c  (constant *);
  static constant * cumavg_v  (constant *);
  static constant * cumprod_d (constant *);
  static constant * cumprod_c (constant *);
  static constant * cumprod_v (constant *);

  static constant * smooth_d_d  (constant *);
  static constant * smooth_c_d  (constant *);
  static constant * smooth_v_d  (constant *);

  static constant * groupdelay_mv_i_i  (constant *);
  static constant * groupdelay_mv (constant *);
  static constant * groupdelay_v (constant *);

  static constant * i0_d   (constant *);
  static constant * i0_c   (constant *);
  static constant * i0_v   (constant *);
  static constant * jn_d_d (constant *);
  static constant * jn_d_c (constant *);
  static constant * jn_d_v (constant *);
  static constant * yn_d_d (constant *);
  static constant * yn_d_c (constant *);
  static constant * yn_d_v (constant *);

  static constant * sqr_d  (constant *);
  static constant * sqr_c  (constant *);
  static constant * sqr_v  (constant *);
  static constant * sqr_m  (constant *);
  static constant * sqr_mv (constant *);

  static constant * polar_d_d (constant *);
  static constant * polar_d_c (constant *);
  static constant * polar_c_d (constant *);
  static constant * polar_c_c (constant *);
  static constant * polar_d_v (constant *);
  static constant * polar_c_v (constant *);
  static constant * polar_v_d (constant *);
  static constant * polar_v_c (constant *);
  static constant * polar_v_v (constant *);

  static constant * arctan2_d_d (constant *);
  static constant * arctan2_d_v (constant *);
  static constant * arctan2_v_d (constant *);
  static constant * arctan2_v_v (constant *);

  static constant * w2dbm_d (constant *);
  static constant * w2dbm_c (constant *);
  static constant * w2dbm_v (constant *);
  static constant * dbm2w_d (constant *);
  static constant * dbm2w_c (constant *);
  static constant * dbm2w_v (constant *);

  static constant * integrate_d_d (constant *);
  static constant * integrate_c_c (constant *);
  static constant * integrate_v_d (constant *);
  static constant * integrate_v_c (constant *);

  static constant * dbm_d   (constant *);
  static constant * dbm_d_d (constant *);
  static constant * dbm_c   (constant *);
  static constant * dbm_c_d (constant *);
  static constant * dbm_v   (constant *);
  static constant * dbm_v_d (constant *);
  static constant * dbm_d_c (constant *);
  static constant * dbm_c_c (constant *);
  static constant * dbm_v_c (constant *);

  static constant * runavg_d_d (constant *);
  static constant * runavg_c_d (constant *);
  static constant * runavg_v_d (constant *);

  static constant * vt_d (constant *);
  static constant * vt_c (constant *);
  static constant * vt_v (constant *);

  static constant * kbd_d_d (constant *);
  static constant * kbd_d   (constant *);

  static constant * ifthenelse_b_b (constant *);
  static constant * ifthenelse_b_d (constant *);
  static constant * ifthenelse_d_b (constant *);
  static constant * ifthenelse_d_d (constant *);
  static constant * ifthenelse_c_c (constant *);
  static constant * ifthenelse_m_m (constant *);
  static constant * ifthenelse_v_v (constant *);
  static constant * ifthenelse_v_v_v (constant *);

  static constant * less_d_d (constant *);
  static constant * less_d_c (constant *);
  static constant * less_d_v (constant *);
  static constant * less_c_d (constant *);
  static constant * less_c_c (constant *);
  static constant * less_c_v (constant *);
  static constant * less_v_d (constant *);
  static constant * less_v_c (constant *);
  static constant * less_v_v (constant *);
  static constant * greater_d_d (constant *);
  static constant * greater_d_c (constant *);
  static constant * greater_d_v (constant *);
  static constant * greater_c_d (constant *);
  static constant * greater_c_c (constant *);
  static constant * greater_c_v (constant *);
  static constant * greater_v_d (constant *);
  static constant * greater_v_c (constant *);
  static constant * greater_v_v (constant *);
  static constant * lessorequal_d_d (constant *);
  static constant * lessorequal_d_c (constant *);
  static constant * lessorequal_d_v (constant *);
  static constant * lessorequal_c_d (constant *);
  static constant * lessorequal_c_c (constant *);
  static constant * lessorequal_c_v (constant *);
  static constant * lessorequal_v_d (constant *);
  static constant * lessorequal_v_c (constant *);
  static constant * lessorequal_v_v (constant *);
  static constant * greaterorequal_d_d (constant *);
  static constant * greaterorequal_d_c (constant *);
  static constant * greaterorequal_d_v (constant *);
  static constant * greaterorequal_c_d (constant *);
  static constant * greaterorequal_c_c (constant *);
  static constant * greaterorequal_c_v (constant *);
  static constant * greaterorequal_v_d (constant *);
  static constant * greaterorequal_v_c (constant *);
  static constant * greaterorequal_v_v (constant *);
  static constant * equal_d_d (constant *);
  static constant * equal_d_c (constant *);
  static constant * equal_d_v (constant *);
  static constant * equal_c_d (constant *);
  static constant * equal_c_c (constant *);
  static constant * equal_c_v (constant *);
  static constant * equal_v_d (constant *);
  static constant * equal_v_c (constant *);
  static constant * equal_v_v (constant *);
  static constant * equal_b_b (constant *);
  static constant * notequal_d_d (constant *);
  static constant * notequal_d_c (constant *);
  static constant * notequal_d_v (constant *);
  static constant * notequal_c_d (constant *);
  static constant * notequal_c_c (constant *);
  static constant * notequal_c_v (constant *);
  static constant * notequal_v_d (constant *);
  static constant * notequal_v_c (constant *);
  static constant * notequal_v_v (constant *);
  static constant * notequal_b_b (constant *);
  static constant * not_b (constant *);
  static constant * or_b_b (constant *);
  static constant * and_b_b (constant *);

  static constant * rand (constant *);
  static constant * srand_d (constant *);

  static constant * vector_x (constant *);
  static constant * matrix_x (constant *);

  static constant * assert_d (constant *);
  static constant * assert_v (constant *);
  static constant * assert_b (constant *);
  static constant * bugon_d (constant *);
  static constant * bugon_v (constant *);
  static constant * bugon_b (constant *);
};

// Type of application function.
typedef constant * (* evaluator_t) (constant *);

// Structure defining an application.
struct application_t
{
  const char * application; /* the name of the application     */
  int retval;               /* its return type                 */
  evaluator_t eval;         /* the actual application function */
  int nargs;                /* number of arguments             */
  int args[16];             /* the appropriate argument types  */
};

extern struct application_t applications[];

} /* namespace eqn */

} // namespace qucs

#endif /* __EVALUATE_H__ */
