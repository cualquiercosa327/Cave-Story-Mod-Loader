// Alternate music mod for 2004 Cave Story
// Copyright © 2018 Clownacy

#pragma once

#include <stdbool.h>

#define STREAM_SAMPLE_RATE 48000
#define STREAM_CHANNEL_COUNT 2

typedef struct BackendStream BackendStream;

bool Backend_Init(void);
void Backend_Deinit(void);
BackendStream* Backend_CreateStream(void (*callback)(void*, float*, unsigned long), void *user_data);
bool Backend_DestroyStream(BackendStream *stream);
bool Backend_SetVolume(BackendStream *stream, float volume);
bool Backend_PauseStream(BackendStream *stream);
bool Backend_ResumeStream(BackendStream *stream);
