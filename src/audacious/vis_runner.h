/*
 * vis_runner.h
 * Copyright 2009-2010 John Lindgren
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions, and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions, and the following disclaimer in the documentation
 *    provided with the distribution.
 *
 * This software is provided "as is" and without any warranty, express or
 * implied. In no event shall the authors be liable for any damages arising from
 * the use of this software.
 */

#ifndef AUD_VIS_RUNNER_H
#define AUD_VIS_RUNNER_H

#include <libaudcore/core.h>

/* When the decoder thread wants to send data to the vis runner, it must block
 * the vis timeout before blocking output functions; otherwise, the vis timeout
 * will hang up waiting for those output functions to be unblocked while the
 * decoder thread hangs up waiting for the vis timeout to finish. */
void vis_runner_lock (void);
void vis_runner_unlock (void);
bool_t vis_runner_locked (void);

void vis_runner_start_stop (bool_t playing, bool_t paused);
void vis_runner_pass_audio (int time, float * data, int samples, int
 channels, int rate);
void vis_runner_time_offset (int offset);
void vis_runner_flush (void);

void vis_runner_enable (bool_t enable);

#endif
