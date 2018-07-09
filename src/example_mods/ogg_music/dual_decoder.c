#include "dual_decoder.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include "mod_loader.h"
#include "sprintfMalloc.h"

#include "decoder.h"
#include "decoder_common.h"

typedef struct DualDecoder
{
	unsigned int current_file;

	bool is_split;
	bool playing_intro;

	Decoder *decoder[2];
} DualDecoder;

static int LoadFiles(const char* const base_path, DecoderType decoder_type, bool loop, bool predecode, DecoderInfo *out_info, Decoder **decoder1, Decoder **decoder2)
{
	int result;

	const char *extension;

	if (decoder_type == DECODER_OGG)
		extension = "ogg";
	else //if (decoder_type == DECODER_FLAC)
		extension = "flac";

	Decoder *decoder[2];
	DecoderInfo info[2];

	// Look for split-file music (Cave Story 3D)
	char *file_path = sprintfMalloc("%s_intro.%s", base_path, extension);
	decoder[0] = Decoder_Open(file_path, decoder_type, loop, predecode, &info[0]);
	free(file_path);

	file_path = sprintfMalloc("%s_loop.%s", base_path, extension);
	decoder[1] = Decoder_Open(file_path, decoder_type, loop, predecode, &info[1]);
	free(file_path);

	if (decoder[0] == NULL || decoder[1] == NULL)
	{
		result = 0;

		if (decoder[0] == NULL && decoder[1] == NULL)
		{
			// Look for single-file music (Cave Story WiiWare)
			file_path = sprintfMalloc("%s.%s", base_path, extension);
			decoder[0] = Decoder_Open(file_path, decoder_type, loop, predecode, &info[0]);
			free(file_path);

			if (decoder[0] == NULL)
			{
				// Neither file could be opened
				result = -1;
			}
		}
		else
		{
			// Only one file could be opened
			if (decoder[0] == NULL)
			{
				// Swap files, since there has to be one in the first slot
				decoder[0] = decoder[1];
				decoder[1] = NULL;
			}
		}
	}
	else
	{
		// Both files opened successfully
		result = 1;
	}

	if (result == 1 && (info[0].channel_count != info[1].channel_count || info[0].sample_rate != info[1].sample_rate))
	{
		Decoder_Close(decoder[0]);
		Decoder_Close(decoder[1]);

		result = -1;
	}

	*out_info = info[0];

	if (result == 1)
		out_info->decoded_size += info[1].decoded_size;

	*decoder1 = decoder[0];
	*decoder2 = decoder[1];

	return result;
}

DualDecoder* DualDecoder_Open(const char* const path, bool loop, bool predecode, DecoderInfo *info)
{
	DualDecoder *song = NULL;

	Decoder *decoder[2];
	const int format = LoadFiles(path, DECODER_OGG, loop, predecode, info, &decoder[0], &decoder[1]);

	if (format != -1)
	{
		song = malloc(sizeof(DualDecoder));

		if (format == 0)
		{
			song->is_split = false;
			song->playing_intro = false;
		}
		else //if (format == 1)
		{
			song->is_split = true;
			song->playing_intro = true;
		}

		song->decoder[0] = decoder[0];
		song->decoder[1] = decoder[1];
		song->current_file = 0;
	}

	return song;
}

void DualDecoder_Close(DualDecoder *this)
{
	if (this)
	{
		for (unsigned int i = 0; i < (this->is_split ? 2 : 1); ++i)
			Decoder_Close(this->decoder[i]);

		free(this);
	}
}

void DualDecoder_Rewind(DualDecoder *this)
{
	if (this)
	{
		this->playing_intro = this->is_split;
		this->current_file = 0;

		for (unsigned int i = 0; i < (this->is_split ? 2 : 1); ++i)
			Decoder_Rewind(this->decoder[i]);
	}
}

unsigned long DualDecoder_GetSamples(DualDecoder *this, void *output_buffer, unsigned long bytes_to_do)
{
	unsigned long bytes_done_total = 0;

	for (unsigned long bytes_done; bytes_done_total != bytes_to_do; bytes_done_total += bytes_done)
	{
		bytes_done = Decoder_GetSamples(this->decoder[this->current_file], output_buffer + bytes_done_total, bytes_to_do - bytes_done_total);

		if (bytes_done < bytes_to_do - bytes_done_total)	// EOF
		{
			if (this->playing_intro)
			{
				this->playing_intro = false;
				++this->current_file;
			}
			else
			{
				break;
			}
		}
	}

	return bytes_done_total;
}