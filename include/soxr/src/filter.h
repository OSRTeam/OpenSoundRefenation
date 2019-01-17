/* SoX Resampler Library      Copyright (c) 2007-13 robs@users.sourceforge.net
 * Licence for this file: LGPL v2.1                  See LICENCE for details. */

#if !defined soxr_filter_included
#define soxr_filter_included

#include "aliases.h"

__declspec(dllexport) double lsx_bessel_I_0(double x);
__declspec(dllexport) void lsx_init_fft_cache(void);
__declspec(dllexport) void lsx_clear_fft_cache(void);
__declspec(dllexport) void lsx_init_fft_cache_f(void);
__declspec(dllexport) void lsx_clear_fft_cache_f(void);
#define lsx_is_power_of_2(x) !(x < 2 || (x & (x - 1)))
__declspec(dllexport) void lsx_safe_rdft(int len, int type, double * d);
__declspec(dllexport) void lsx_safe_cdft(int len, int type, double * d);
__declspec(dllexport) void lsx_safe_rdft_f(int len, int type, float * d);
__declspec(dllexport) void lsx_safe_cdft_f(int len, int type, float * d);
__declspec(dllexport) void lsx_ordered_convolve(int n, void * not_used, double * a, const double * b);
__declspec(dllexport) void lsx_ordered_convolve_f(int n, void * not_used, float * a, const float * b);
__declspec(dllexport) void lsx_ordered_partial_convolve(int n, double * a, const double * b);
__declspec(dllexport) void lsx_ordered_partial_convolve_f(int n, float * a, const float * b);

__declspec(dllexport) double lsx_kaiser_beta(double att, double tr_bw);
__declspec(dllexport) double * lsx_make_lpf(int num_taps, double Fc, double beta, double rho,
    double scale);
__declspec(dllexport) void lsx_kaiser_params(double att, double Fc, double tr_bw, double * beta, int * num_taps);
__declspec(dllexport) double * lsx_design_lpf(
    double Fp,      /* End of pass-band */
    double Fs,      /* Start of stop-band */
    double Fn,      /* Nyquist freq; e.g. 0.5, 1, PI; < 0: dummy run */
    double att,     /* Stop-band attenuation in dB */
    int * num_taps, /* 0: value will be estimated */
    int k,          /* >0: number of phases; <0: num_taps ≡ 1 (mod -k) */
    double beta);   /* <0: value will be estimated */
__declspec(dllexport) void lsx_fir_to_phase(double * * h, int * len,
    int * post_len, double phase0);

#endif
