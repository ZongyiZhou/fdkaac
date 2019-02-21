/* 
 * Copyright (C) 2013 nu774
 * For conditions of distribution and use, see copyright notice in COPYING
 */
#ifndef PROGRESS_H
#define PROGRESS_H

typedef struct aacenc_progress_t {
    int64_t start;
    int64_t prev_tick;
    double timescale_rcp;
    int64_t total;
} aacenc_progress_t;

void aacenc_progress_init(aacenc_progress_t *progress, int64_t total,
                          int32_t timescale);
void aacenc_progress_update(aacenc_progress_t *progress, int64_t current,
                            int32_t period);
void aacenc_progress_finish(aacenc_progress_t *progress, int64_t current);

#endif
