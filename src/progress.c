/* 
 * Copyright (C) 2013 nu774
 * For conditions of distribution and use, see copyright notice in COPYING
 */
#if HAVE_CONFIG_H
#  include "config.h"
#endif
#include <stdio.h>
#include <limits.h>
#include <float.h>
#include <time.h>
#if HAVE_STDINT_H
#  include <stdint.h>
#endif
#if HAVE_INTTYPES_H
#  include <inttypes.h>
#elif defined _MSC_VER
#  define PRId64 "I64d"
#endif
#include "compat.h"
#include "progress.h"

static
void seconds_to_hms(double seconds, int *h, int *m, int *s, int *millis)
{
    *h = (int)(seconds / 3600.0);
    seconds -= *h * 3600;
    *m = (int)(seconds / 60.0);
    seconds -= *m * 60;
    *s = (int)seconds;
    *millis = (int)((seconds - *s) * 1000.0);
}

static
void print_seconds(FILE *fp, double seconds)
{
    int h, m, s, millis;
    seconds_to_hms(seconds, &h, &m, &s, &millis);
    if (h)
        fprintf(stderr, "%d:%02d:%02d.%03d", h, m, s, millis);
    else
        fprintf(stderr, "%02d:%02d.%03d", m, s, millis);
}

void aacenc_progress_init(aacenc_progress_t *progress, int64_t total,
                          int32_t timescale)
{
    progress->start = aacenc_timer();
    progress->timescale_rcp = 1.0 / timescale;
    progress->prev_tick = progress->start;
    progress->total = total;
}

void aacenc_progress_update(aacenc_progress_t *progress, int64_t current,
                            int32_t period)
{
    int64_t now = aacenc_timer();
    if (now - progress->prev_tick < period)
        return;
    double elapsed = (now - progress->start) * 1E-3;
    double seconds = current * progress->timescale_rcp;
    double speed = elapsed ? seconds / elapsed : 1.0;
    progress->prev_tick = now;

    if (progress->total == INT64_MAX) {
        putc('\r', stderr);
        print_seconds(stderr, seconds);
        fprintf(stderr, " (%.0fx)   ", speed);
    } else {
        double percent = progress->total ? 100.0 * current / progress->total : 100;
        double eta = current ? elapsed * (progress->total / (double)current - 1.0)
                             : progress->total ? DBL_MAX : 0;
        fprintf(stderr, "\r[%.0f%%] ", percent);
        print_seconds(stderr, seconds);
        putc('/', stderr);
        print_seconds(stderr, progress->total * progress->timescale_rcp);
        fprintf(stderr, speed > 9.94? " (%.0fx), ETA " : " (%.1fx), ETA ", speed);
        print_seconds(stderr, eta);
        fputs("   ", stderr);
    }
}

void aacenc_progress_finish(aacenc_progress_t *progress, int64_t current)
{
    double ellapsed = (aacenc_timer() - progress->start) / 1000.0;
    aacenc_progress_update(progress, current, 0);
    if (progress->total == INT64_MAX)
        fprintf(stderr, "\n%" PRId64 " samples processed in ", current);
    else
        fprintf(stderr, "\n%" PRId64 "/%" PRId64 " samples processed in ",
                current, progress->total);
    print_seconds(stderr, ellapsed);
    putc('\n', stderr);
}
