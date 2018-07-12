#include "decoder_predecode.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include "decoder_common.h"
#include "memory_file.h"

typedef struct DecoderPredecode
{
	MemoryFile *file;
	bool loop;
} DecoderPredecode;

DecoderPredecode* Decoder_Predecode_Open(const char *file_path, bool loop, DecoderInfo *info, DecoderBackend *backend)
{
	DecoderPredecode *this = NULL;

	void *backend_object = backend->Open(file_path, loop, info, backend->backend);

	if (backend_object)
	{
		this = malloc(sizeof(DecoderPredecode));

		this->loop = loop;

		unsigned char *buffer = malloc(info->decoded_size);
		backend->GetSamples(backend_object, buffer, info->decoded_size);
		this->file = MemoryFile_fopen_from(buffer, info->decoded_size);
		backend->Close(backend_object);
		free(backend);
	}

	return this;
}

void Decoder_Predecode_Close(DecoderPredecode *this)
{
	MemoryFile_fclose(this->file);
	free(this);
}

void Decoder_Predecode_Rewind(DecoderPredecode *this)
{
	MemoryFile_fseek(this->file, 0, SEEK_SET);
}

unsigned long Decoder_Predecode_GetSamples(DecoderPredecode *this, void *output_buffer, unsigned long bytes_to_do)
{
	unsigned long bytes_done_total = 0;

	for (;;)
	{
		bytes_done_total += MemoryFile_fread(output_buffer + bytes_done_total, 1, bytes_to_do - bytes_done_total, this->file);

		if (bytes_done_total == bytes_to_do || !this->loop)
			break;

		Decoder_Predecode_Rewind(this);
	}

	return bytes_done_total;
}