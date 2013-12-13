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
 * 07/12/13 15:40
 *
 * -----------------
 * Mandala algorithm
 * -----------------
 *
 * Author(s)
 * Guillaume Voirin (https://github.com/gpnuma)
 * Piotr Tarsa (https://github.com/tarsa)
 *
 * Description
 * Very fast two level dictionary hash algorithm derived from Chameleon, with predictions lookup
 */

#ifndef DENSITY_MANDALA_DECODE_H
#define DENSITY_MANDALA_DECODE_H

#include "byte_buffer.h"
#include "kernel_mandala_dictionary.h"
#include "kernel_mandala.h"
#include "block.h"
#include "kernel_decode.h"
#include "density_api.h"

#define DENSITY_MANDALA_DECODE_MINIMUM_INPUT_LOOKAHEAD               (sizeof(density_mandala_signature) + sizeof(uint32_t) * 4 * sizeof(density_mandala_signature))
#define DENSITY_MANDALA_DECODE_MINIMUM_OUTPUT_LOOKAHEAD              (sizeof(uint32_t) * 4 * sizeof(density_mandala_signature))

typedef enum {
    DENSITY_MANDALA_DECODE_PROCESS_SIGNATURES_AND_DATA_FAST,
    DENSITY_MANDALA_DECODE_PROCESS_SIGNATURE_SAFE,
    DENSITY_MANDALA_DECODE_PROCESS_DATA_FAST,
    DENSITY_MANDALA_DECODE_PROCESS_DATA_SAFE,
    DENSITY_MANDALA_DECODE_PROCESS_FINISH
} DENSITY_MANDALA_DECODE_PROCESS;

#pragma pack(push)
#pragma pack(4)
typedef struct {
    DENSITY_MANDALA_DECODE_PROCESS process;

    density_main_header_parameters parameters;
    uint_fast64_t resetCycle;

    density_mandala_signature signature;
    uint_fast32_t shift;
    uint_fast32_t signaturesCount;
    uint_fast8_t efficiencyChecked;

    uint_fast64_t endDataOverhead;

    union {
        density_byte as_bytes[8];
        uint64_t as_uint64_t;
    } partialSignature;
    union {
        density_byte as_bytes[4];
        uint32_t as_uint32_t;
    } partialUncompressedChunk;

    uint_fast64_t signatureBytes;
    uint_fast64_t uncompressedChunkBytes;

    uint32_t lastHash;

    density_mandala_dictionary dictionary;
} density_mandala_decode_state;
#pragma pack(pop)

DENSITY_KERNEL_DECODE_STATE density_mandala_decode_init(density_mandala_decode_state *, const density_main_header_parameters parameters, const uint_fast32_t);
DENSITY_KERNEL_DECODE_STATE density_mandala_decode_process(density_memory_location *, density_memory_location *, density_mandala_decode_state *, const density_bool);
DENSITY_KERNEL_DECODE_STATE density_mandala_decode_finish(density_mandala_decode_state *);

#endif