/* SoX Resampler Library      Copyright (c) 2007-13 robs@users.sourceforge.net
 * Licence for this file: LGPL v2.1                  See LICENCE for details. */

#include <math.h>
#include <libavcodec/avfft.h>
#include "simd.h"

static void * forward_setup(int len) {return av_rdft_init((int)(log(len)/log(2)+.5),DFT_R2C);}
static void * backward_setup(int len) {return av_rdft_init((int)(log(len)/log(2)+.5),IDFT_C2R);}
static void rdft(int length, void * setup, float * h) {av_rdft_calc(setup, h); (void)length;}
static int multiplier(void) {return 2;}
static void nothing(void) {}

typedef void (* fn_t)(void);
fn_t _soxr_rdft32s_cb[] = {
  (fn_t)forward_setup,
  (fn_t)backward_setup,
  (fn_t)av_rdft_end,
  (fn_t)rdft,
  (fn_t)rdft,
  (fn_t)rdft,
  (fn_t)rdft,
  (fn_t)_soxr_ordered_convolve_simd,
  (fn_t)_soxr_ordered_partial_convolve_simd,
  (fn_t)multiplier,
  (fn_t)nothing,
};
