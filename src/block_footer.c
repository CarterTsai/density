/*
 * Centaurean Density
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
 * 18/10/13 23:58
 */

#include "block_footer.h"

DENSITY_FORCE_INLINE uint_fast32_t density_block_footer_read(density_memory_location *restrict in, density_block_footer *restrict blockFooter) {
    blockFooter->hashsum1 = DENSITY_LITTLE_ENDIAN_64(*(uint64_t *) in->pointer);
    in->pointer += sizeof(uint64_t);
    blockFooter->hashsum2 = DENSITY_LITTLE_ENDIAN_64(*(uint64_t *) in->pointer);
    in->pointer += sizeof(uint64_t);

    in->available_bytes -= sizeof(density_block_footer);

    return sizeof(density_block_footer);
}

DENSITY_FORCE_INLINE uint_fast32_t density_block_footer_write(density_memory_location *out, const uint_fast64_t hashsum1, const uint_fast64_t hashsum2) {
    *(uint64_t *) out->pointer = DENSITY_LITTLE_ENDIAN_64(hashsum1);
    out->pointer += sizeof(uint64_t);
    *(uint64_t *) out->pointer = DENSITY_LITTLE_ENDIAN_64(hashsum2);
    out->pointer += sizeof(uint64_t);

    out->available_bytes -= sizeof(density_block_footer);

    return sizeof(density_block_footer);
}