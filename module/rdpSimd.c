/*
Copyright 2014-2017 Jay Sorg

Permission to use, copy, modify, distribute, and sell this software and its
documentation for any purpose is hereby granted without fee, provided that
the above copyright notice appear in all copies and that both that
copyright notice and this permission notice appear in supporting
documentation.

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
OPEN GROUP BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

SIMD function assigning

*/

#if defined(HAVE_CONFIG_H)
#include "config_ac.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* this should be before all X11 .h files */
#include <xorg-server.h>
#include <xorgVersion.h>

/* all driver need this */
#include <xf86.h>
#include <xf86_OSproc.h>

#include "rdp.h"
#include "rdpXv.h"
#include "rdpCapture.h"
#include "rdpSimd.h"

/* use simd, run time */
int g_simd_use_accel = 1;

/* use simd, compile time, if zero, g_simd_use_accel does not matter */
#if !defined(SIMD_USE_ACCEL)
#define SIMD_USE_ACCEL 0
#endif

#if SIMD_USE_ACCEL
#if defined(__x86_64__) || defined(__AMD64__) || defined (_M_AMD64)
#include "amd64/funcs_amd64.h"
#elif defined(__x86__) || defined(_M_IX86) || defined(__i386__)
#include "x86/funcs_x86.h"
#endif
#endif

#define LOG_LEVEL 1
#define LLOGLN(_level, _args) \
    do { if (_level < LOG_LEVEL) { ErrorF _args ; ErrorF("\n"); } } while (0)

#if SIMD_USE_ACCEL

#if defined(__x86_64__) || defined(__AMD64__) || defined (_M_AMD64)
/******************************************************************************/
static int
a8r8g8b8_to_nv12_box_amd64_sse2_wrap(const uint8_t *s8, int src_stride,
                                     uint8_t *d8_y, int dst_stride_y,
                                     uint8_t *d8_uv, int dst_stride_uv,
                                     int width, int height)
{
    int aligned_width;
    int left_over_width;
    int error;

    aligned_width = width & ~7;
    left_over_width = width - aligned_width;
    if (height > 0)
    {
        if (aligned_width > 0)
        {
            error = a8r8g8b8_to_nv12_box_amd64_sse2(s8, src_stride,
                                                    d8_y, dst_stride_y,
                                                    d8_uv, dst_stride_uv,
                                                    aligned_width, height);
            if (error != 0)
            {
                return error;
            }
        }
        if (left_over_width > 0)
        {
            error = a8r8g8b8_to_nv12_box(s8 + aligned_width * 4, src_stride,
                                         d8_y + aligned_width, dst_stride_y,
                                         d8_uv + aligned_width, dst_stride_uv,
                                         left_over_width, height);
            if (error != 0)
            {
                return error;
            }
        }
    }
    return 0;
}

/******************************************************************************/
static int
a8r8g8b8_to_nv12_709fr_box_amd64_sse2_wrap(const uint8_t *s8, int src_stride,
                                           uint8_t *d8_y, int dst_stride_y,
                                           uint8_t *d8_uv, int dst_stride_uv,
                                           int width, int height)
{
    int aligned_width;
    int left_over_width;
    int error;

    aligned_width = width & ~7;
    left_over_width = width - aligned_width;
    if (height > 0)
    {
        if (aligned_width > 0)
        {
            error = a8r8g8b8_to_nv12_709fr_box_amd64_sse2(s8, src_stride,
                                                          d8_y, dst_stride_y,
                                                          d8_uv, dst_stride_uv,
                                                          aligned_width,
                                                          height);
            if (error != 0)
            {
                return error;
            }
        }
        if (left_over_width > 0)
        {
            error = a8r8g8b8_to_nv12_709fr_box(s8 + aligned_width * 4,
                                               src_stride,
                                               d8_y + aligned_width,
                                               dst_stride_y,
                                               d8_uv + aligned_width,
                                               dst_stride_uv,
                                               left_over_width, height);
            if (error != 0)
            {
                return error;
            }
        }
    }
    return 0;
}

/*****************************************************************************/
int
a8r8g8b8_to_yuvalp_box_amd64_sse2_wrap(const uint8_t *s8, int src_stride,
                                       uint8_t *d8, int dst_stride,
                                       int width, int height)
{
    int aligned_width;
    int left_over_width;
    int error;

    aligned_width = width & ~7;
    left_over_width = width - aligned_width;
    if (height > 0)
    {
        if (aligned_width > 0)
        {
            error = a8r8g8b8_to_yuvalp_box_amd64_sse2(s8, src_stride,
                                                      d8, dst_stride,
                                                      aligned_width, height);
            if (error != 0)
            {
                return error;
            }
        }
        if (left_over_width > 0)
        {
            error = a8r8g8b8_to_yuvalp_box(s8 + aligned_width * 4, src_stride,
                                           d8 + aligned_width, dst_stride,
                                           left_over_width, height);
            if (error != 0)
            {
                return error;
            }
        }
    }
    return 0;
}
#endif

#if defined(__x86__) || defined(_M_IX86) || defined(__i386__)
/******************************************************************************/
static int
a8r8g8b8_to_nv12_box_x86_sse2_wrap(const uint8_t *s8, int src_stride,
                                   uint8_t *d8_y, int dst_stride_y,
                                   uint8_t *d8_uv, int dst_stride_uv,
                                   int width, int height)
{
    int aligned_width;
    int left_over_width;
    int error;

    aligned_width = width & ~7;
    left_over_width = width - aligned_width;
    if (height > 0)
    {
        if (aligned_width > 0)
        {
            error = a8r8g8b8_to_nv12_box_x86_sse2(s8, src_stride,
                                                  d8_y, dst_stride_y,
                                                  d8_uv, dst_stride_uv,
                                                  aligned_width, height);
            if (error != 0)
            {
                return error;
            }
        }
        if (left_over_width > 0)
        {
            error = a8r8g8b8_to_nv12_box(s8 + aligned_width * 4, src_stride,
                                         d8_y + aligned_width, dst_stride_y,
                                         d8_uv + aligned_width, dst_stride_uv,
                                         left_over_width, height);
            if (error != 0)
            {
                return error;
            }
        }
    }
    return 0;
}

/******************************************************************************/
static int
a8r8g8b8_to_nv12_709fr_box_x86_sse2_wrap(const uint8_t *s8, int src_stride,
                                         uint8_t *d8_y, int dst_stride_y,
                                         uint8_t *d8_uv, int dst_stride_uv,
                                         int width, int height)
{
    int aligned_width;
    int left_over_width;
    int error;

    aligned_width = width & ~7;
    left_over_width = width - aligned_width;
    if (height > 0)
    {
        if (aligned_width > 0)
        {
            error = a8r8g8b8_to_nv12_709fr_box_x86_sse2(s8, src_stride,
                                                        d8_y, dst_stride_y,
                                                        d8_uv, dst_stride_uv,
                                                        aligned_width, height);
            if (error != 0)
            {
                return error;
            }
        }
        if (left_over_width > 0)
        {
            error = a8r8g8b8_to_nv12_709fr_box(s8 + aligned_width * 4,
                                               src_stride,
                                               d8_y + aligned_width,
                                               dst_stride_y,
                                               d8_uv + aligned_width,
                                               dst_stride_uv,
                                               left_over_width, height);
            if (error != 0)
            {
                return error;
            }
        }
    }
    return 0;
}

/*****************************************************************************/
int
a8r8g8b8_to_yuvalp_box_x86_sse2_wrap(const uint8_t *s8, int src_stride,
                                     uint8_t *d8, int dst_stride,
                                     int width, int height)
{
    int aligned_width;
    int left_over_width;
    int error;

    aligned_width = width & ~7;
    left_over_width = width - aligned_width;
    if (height > 0)
    {
        if (aligned_width > 0)
        {
            error = a8r8g8b8_to_yuvalp_box_x86_sse2(s8, src_stride,
                                                    d8, dst_stride,
                                                    aligned_width, height);
            if (error != 0)
            {
                return error;
            }
        }
        if (left_over_width > 0)
        {
            error = a8r8g8b8_to_yuvalp_box(s8 + aligned_width * 4, src_stride,
                                           d8 + aligned_width, dst_stride,
                                           left_over_width, height);
            if (error != 0)
            {
                return error;
            }
        }
    }
    return 0;
}
#endif

#endif

/*****************************************************************************/
Bool
rdpSimdInit(ScreenPtr pScreen, ScrnInfoPtr pScrn)
{
    rdpPtr dev;

    dev = XRDPPTR(pScrn);
    /* assign functions */
    LLOGLN(0, ("rdpSimdInit: assigning yuv functions"));
    dev->yv12_to_rgb32 = YV12_to_RGB32;
    dev->i420_to_rgb32 = I420_to_RGB32;
    dev->yuy2_to_rgb32 = YUY2_to_RGB32;
    dev->uyvy_to_rgb32 = UYVY_to_RGB32;
    dev->a8r8g8b8_to_a8b8g8r8_box = a8r8g8b8_to_a8b8g8r8_box;
    dev->a8r8g8b8_to_nv12_box = a8r8g8b8_to_nv12_box;
    dev->a8r8g8b8_to_nv12_709fr_box = a8r8g8b8_to_nv12_709fr_box;
    dev->a8r8g8b8_to_yuvalp_box = a8r8g8b8_to_yuvalp_box;
#if SIMD_USE_ACCEL
    if (g_simd_use_accel)
    {
#if defined(__x86_64__) || defined(__AMD64__) || defined (_M_AMD64)
        int ax, bx, cx, dx;
        cpuid_amd64(1, 0, &ax, &bx, &cx, &dx);
        LLOGLN(0, ("rdpSimdInit: cpuid ax 1 cx 0 return ax 0x%8.8x bx "
               "0x%8.8x cx 0x%8.8x dx 0x%8.8x", ax, bx, cx, dx));
        if (dx & (1 << 26)) /* SSE 2 */
        {
            dev->yv12_to_rgb32 = yv12_to_rgb32_amd64_sse2;
            dev->i420_to_rgb32 = i420_to_rgb32_amd64_sse2;
            dev->yuy2_to_rgb32 = yuy2_to_rgb32_amd64_sse2;
            dev->uyvy_to_rgb32 = uyvy_to_rgb32_amd64_sse2;
            dev->a8r8g8b8_to_a8b8g8r8_box = a8r8g8b8_to_a8b8g8r8_box_amd64_sse2;
            dev->a8r8g8b8_to_nv12_box = a8r8g8b8_to_nv12_box_amd64_sse2_wrap;
            dev->a8r8g8b8_to_nv12_709fr_box = a8r8g8b8_to_nv12_709fr_box_amd64_sse2_wrap;
            dev->a8r8g8b8_to_yuvalp_box = a8r8g8b8_to_yuvalp_box_amd64_sse2_wrap;
            LLOGLN(0, ("rdpSimdInit: sse2 amd64 yuv functions assigned"));
        }
#elif defined(__x86__) || defined(_M_IX86) || defined(__i386__)
        int ax, bx, cx, dx;
        cpuid_x86(1, 0, &ax, &bx, &cx, &dx);
        LLOGLN(0, ("rdpSimdInit: cpuid ax 1 cx 0 return ax 0x%8.8x bx "
               "0x%8.8x cx 0x%8.8x dx 0x%8.8x", ax, bx, cx, dx));
        if (dx & (1 << 26)) /* SSE 2 */
        {
            dev->yv12_to_rgb32 = yv12_to_rgb32_x86_sse2;
            dev->i420_to_rgb32 = i420_to_rgb32_x86_sse2;
            dev->yuy2_to_rgb32 = yuy2_to_rgb32_x86_sse2;
            dev->uyvy_to_rgb32 = uyvy_to_rgb32_x86_sse2;
            dev->a8r8g8b8_to_a8b8g8r8_box = a8r8g8b8_to_a8b8g8r8_box_x86_sse2;
            dev->a8r8g8b8_to_nv12_box = a8r8g8b8_to_nv12_box_x86_sse2_wrap;
            dev->a8r8g8b8_to_nv12_709fr_box = a8r8g8b8_to_nv12_709fr_box_x86_sse2_wrap;
            dev->a8r8g8b8_to_yuvalp_box = a8r8g8b8_to_yuvalp_box_x86_sse2_wrap;
            LLOGLN(0, ("rdpSimdInit: sse2 x86 yuv functions assigned"));
        }
#endif
    }
#endif
    return 1;
}

