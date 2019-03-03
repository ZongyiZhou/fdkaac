/* 
 * Copyright (C) 2013 nu774
 * For conditions of distribution and use, see copyright notice in COPYING
 */
#ifndef LPCM_H
#define LPCM_H
#include "fdk-aac/FDK_archdef.h"

enum pcm_type {
    PCM_TYPE_UNKNOWN = 0,
    PCM_TYPE_SINT = 1,
    PCM_TYPE_UINT = 2,
    PCM_TYPE_FLOAT = 4,
    PCM_TYPE_SINT_BE = (8|1),
    PCM_TYPE_UINT_BE = (8|2),
    PCM_TYPE_FLOAT_BE = (8|4),
};

typedef struct pcm_sample_description_t {
    enum pcm_type sample_type;
    uint32_t sample_rate;
    uint8_t bits_per_channel;
    uint8_t bytes_per_frame;
    uint8_t channels_per_frame;
    uint32_t channel_mask;
} pcm_sample_description_t;

#define PCM_IS_SINT(desc) ((desc)->sample_type & 1)
#define PCM_IS_UINT(desc) ((desc)->sample_type & 2)
#define PCM_IS_FLOAT(desc) ((desc)->sample_type & 4)
#define PCM_IS_BIG_ENDIAN(desc) ((desc)->sample_type & 8)
#define PCM_BYTES_PER_CHANNEL(desc) \
    ((desc)->bytes_per_frame / (desc)->channels_per_frame)

#ifdef __x86__
#ifdef _MSC_VER
#include <intrin.h>
inline int lrintf_fast(const float value) {
    return _mm_cvtss_si32(_mm_set_ss(value));
}
#define lrintf(v) lrintf_fast(v)

inline int lrint_fast(const double value) {
    return _mm_cvtsd_si32(_mm_set_sd(value));
}
#define lrint(v) lrint_fast(v)

#ifdef _M_X64
inline long long llrintf_fast(const float value) {
    return _mm_cvtss_si64(_mm_set_ss(value));
}
#define llrintf(v) llrintf_fast(v)

inline long long llrint_fast(const double value) {
    return _mm_cvtsd_si64(_mm_set_sd(value));
}
#define llrint(v) llrint_fast(v)
#endif // _M_X64
#else
#include <x86intrin.h>
#endif // MSC_VER

inline short pcm_float_2_s16(const float value)
{
    __m128i x = _mm_cvtps_epi32(_mm_set_ss(value * (float)(1<<15)));
    x = _mm_packs_epi32(x, x);
    return x.m128i_i16[0];
}
#else // __x86__
inline short pcm_float_2_s16(const float value)
{
    int r = lrintf(value * (float)(1 << 15));
    return min(32767, max(r, -32768));
}
#endif // __x86__

static
inline float pcm_clip(const float n, const float min_value, const float max_value)
{
    if (n < min_value)
        return min_value;
    else if (n > max_value)
        return max_value;
    return n;
}

#endif
