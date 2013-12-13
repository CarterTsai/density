/*
 * Centaurean Density
 * http://www.libssc.net
 *
 * Copyright (c) 2013, Guillaume Voirin
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Centaurean nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * 18/10/13 23:30
 */

#ifndef DENSITY_ENCODE_H
#define DENSITY_ENCODE_H

#include <string.h>

#include "block_footer.h"
#include "block_header.h"
#include "main_header.h"
#include "main_footer.h"
#include "block_mode_marker.h"
#include "block_encode.h"
#include "kernel_chameleon_encode.h"
#include "kernel_mandala_encode.h"
#include "density_api.h"

typedef enum {
    DENSITY_ENCODE_STATE_READY = 0,
    DENSITY_ENCODE_STATE_STALL_ON_OUTPUT_BUFFER,
    DENSITY_ENCODE_STATE_STALL_ON_INPUT_BUFFER,
    DENSITY_ENCODE_STATE_ERROR
} DENSITY_ENCODE_STATE;

typedef enum {
    DENSITY_ENCODE_PROCESS_WRITE_BLOCKS,
    DENSITY_ENCODE_PROCESS_WRITE_FOOTER,
    DENSITY_ENCODE_PROCESS_FINISHED
} DENSITY_ENCODE_PROCESS;

#pragma pack(push)
#pragma pack(4)
typedef struct {
    DENSITY_ENCODE_PROCESS process;
    DENSITY_COMPRESSION_MODE compressionMode;
    DENSITY_BLOCK_TYPE blockType;
    DENSITY_ENCODE_OUTPUT_TYPE encodeOutputType;
    const struct stat* fileAttributes;

    uint_fast64_t totalRead;
    uint_fast64_t totalWritten;

    density_block_encode_state blockEncodeState;
} density_encode_state;
#pragma pack(pop)

DENSITY_ENCODE_STATE density_encode_init(density_memory_location*, density_encode_state *, const DENSITY_COMPRESSION_MODE, const DENSITY_ENCODE_OUTPUT_TYPE, const DENSITY_BLOCK_TYPE);
DENSITY_ENCODE_STATE density_encode_process(density_memory_location*, density_memory_location*, density_encode_state *, const density_bool);
DENSITY_ENCODE_STATE density_encode_finish(density_memory_location*, density_encode_state *);

#endif