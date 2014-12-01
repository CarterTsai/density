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
 * 18/10/13 00:03
 */

#include "block_encode.h"

DENSITY_FORCE_INLINE DENSITY_BLOCK_ENCODE_STATE density_block_encode_write_block_header(density_memory_location* restrict out, density_block_encode_state *restrict state) {
    if (sizeof(density_block_header) > out->available_bytes)
        return DENSITY_BLOCK_ENCODE_STATE_STALL_ON_OUTPUT_BUFFER;

    state->currentMode = state->targetMode;

    state->currentBlockData.inStart = state->totalRead;
    state->currentBlockData.outStart = state->totalWritten;

    state->totalWritten += density_block_header_write(out);

    state->process = DENSITY_BLOCK_ENCODE_PROCESS_WRITE_DATA;

    return DENSITY_BLOCK_ENCODE_STATE_READY;
}

DENSITY_FORCE_INLINE DENSITY_BLOCK_ENCODE_STATE density_block_encode_write_block_footer(density_memory_location* restrict out, density_block_encode_state *restrict state) {
    if (sizeof(density_block_footer) > out->available_bytes)
        return DENSITY_BLOCK_ENCODE_STATE_STALL_ON_OUTPUT_BUFFER;

    state->totalWritten += density_block_footer_write(out, 0);

    return DENSITY_BLOCK_ENCODE_STATE_READY;
}

DENSITY_FORCE_INLINE DENSITY_BLOCK_ENCODE_STATE density_block_encode_write_mode_marker(density_memory_location* restrict out, density_block_encode_state *restrict state) {
    if (sizeof(density_mode_marker) > out->available_bytes)
        return DENSITY_BLOCK_ENCODE_STATE_STALL_ON_OUTPUT_BUFFER;

    switch (state->blockType) {
        case DENSITY_BLOCK_MODE_COPY:
            break;

        default:
            if (state->totalWritten > state->totalRead)
                state->blockType = DENSITY_BLOCK_MODE_COPY;
            break;
    }

    state->totalWritten += density_block_mode_marker_write(out, state->blockType);

    state->process = DENSITY_BLOCK_ENCODE_PROCESS_WRITE_DATA;

    return DENSITY_BLOCK_ENCODE_STATE_READY;
}

DENSITY_FORCE_INLINE void density_block_encode_update_totals(density_memory_location* restrict in, density_memory_location* restrict out, density_block_encode_state *restrict state, const uint_fast64_t availableInBefore, const uint_fast64_t availableOutBefore) {
    state->totalRead += availableInBefore - in->available_bytes;
    state->totalWritten += availableOutBefore - out->available_bytes;
}

DENSITY_FORCE_INLINE DENSITY_BLOCK_ENCODE_STATE density_block_encode_init(density_block_encode_state *restrict state, const DENSITY_COMPRESSION_MODE mode, const DENSITY_BLOCK_TYPE blockType, void *kernelState, DENSITY_KERNEL_ENCODE_STATE (*kernelInit)(void *), DENSITY_KERNEL_ENCODE_STATE (*kernelProcess)(density_memory_location*, density_memory_location*, void *, const density_bool), DENSITY_KERNEL_ENCODE_STATE (*kernelFinish)(void *)) {
    state->process = DENSITY_BLOCK_ENCODE_PROCESS_WRITE_BLOCK_HEADER;
    state->blockType = blockType;
    state->targetMode = mode;
    state->currentMode = mode;

    state->totalRead = 0;
    state->totalWritten = 0;

    switch (mode) {
        case DENSITY_BLOCK_MODE_KERNEL:
            state->kernelEncodeState = kernelState;
            state->kernelEncodeInit = kernelInit;
            state->kernelEncodeProcess = kernelProcess;
            state->kernelEncodeFinish = kernelFinish;

            state->kernelEncodeInit(state->kernelEncodeState);
            break;
        default:
            break;
    }

    return DENSITY_BLOCK_ENCODE_STATE_READY;
}

DENSITY_FORCE_INLINE DENSITY_BLOCK_ENCODE_STATE density_block_encode_process(density_memory_location* restrict in, density_memory_location* restrict out, density_block_encode_state *restrict state, const density_bool flush) {
    DENSITY_BLOCK_ENCODE_STATE encodeState;
    DENSITY_KERNEL_ENCODE_STATE kernelEncodeState;
    uint_fast64_t availableInBefore;
    uint_fast64_t availableOutBefore;
    uint_fast64_t blockRemaining;
    uint_fast64_t inRemaining;
    uint_fast64_t outRemaining;

    while (true) {
        switch (state->process) {
            case DENSITY_BLOCK_ENCODE_PROCESS_WRITE_BLOCK_HEADER:
                if ((encodeState = density_block_encode_write_block_header(out, state)))
                    return encodeState;
                break;

            case DENSITY_BLOCK_ENCODE_PROCESS_WRITE_BLOCK_FOOTER:
                if (state->blockType == DENSITY_BLOCK_TYPE_DEFAULT) if ((encodeState = density_block_encode_write_block_footer(out, state)))
                    return encodeState;
                state->process = DENSITY_BLOCK_ENCODE_PROCESS_WRITE_BLOCK_HEADER;
                break;

            case DENSITY_BLOCK_ENCODE_PROCESS_WRITE_LAST_BLOCK_FOOTER:
                if (state->blockType == DENSITY_BLOCK_TYPE_DEFAULT) if ((encodeState = density_block_encode_write_block_footer(out, state)))
                    return encodeState;
                state->process = DENSITY_BLOCK_ENCODE_PROCESS_FINISHED;
                return DENSITY_BLOCK_ENCODE_STATE_READY;

            case DENSITY_BLOCK_ENCODE_PROCESS_WRITE_BLOCK_MODE_MARKER:
                if ((encodeState = density_block_encode_write_mode_marker(out, state)))
                    return encodeState;
                break;

            case DENSITY_BLOCK_ENCODE_PROCESS_WRITE_DATA:
                availableInBefore = in->available_bytes;
                availableOutBefore = out->available_bytes;

                switch (state->currentMode) {
                    case DENSITY_BLOCK_MODE_COPY:
                        blockRemaining = (uint_fast64_t) DENSITY_PREFERRED_COPY_BLOCK_SIZE - (state->totalRead - state->currentBlockData.inStart);
                        inRemaining = in->available_bytes;
                        outRemaining = out->available_bytes;

                        if (in->available_bytes <= outRemaining) {
                            if (blockRemaining <= inRemaining)
                                goto copy_until_end_of_block;
                            else {
                                memcpy(out, in, (size_t) inRemaining);
                                in->pointer += inRemaining;
                                in->available_bytes = 0;
                                out->pointer += inRemaining;
                                out->available_bytes -= inRemaining;
                                density_block_encode_update_totals(in, out, state, availableInBefore, availableOutBefore);
                                if (flush)
                                    state->process = DENSITY_BLOCK_ENCODE_PROCESS_WRITE_LAST_BLOCK_FOOTER;
                                else
                                    return DENSITY_BLOCK_ENCODE_STATE_STALL_ON_INPUT_BUFFER;
                            }
                        } else {
                            if (blockRemaining <= outRemaining)
                                goto copy_until_end_of_block;
                            else {
                                memcpy(out, in, (size_t) outRemaining);
                                in->pointer += outRemaining;
                                in->available_bytes -= outRemaining;
                                out->pointer += outRemaining;
                                out->available_bytes = 0;
                                density_block_encode_update_totals(in, out, state, availableInBefore, availableOutBefore);
                                return DENSITY_BLOCK_ENCODE_STATE_STALL_ON_OUTPUT_BUFFER;
                            }
                        }
                        goto exit;

                    copy_until_end_of_block:
                        memcpy(out, in, (size_t) blockRemaining);
                        in->pointer += blockRemaining;
                        in->available_bytes -= blockRemaining;
                        out->pointer += blockRemaining;
                        out->available_bytes -= blockRemaining;
                        density_block_encode_update_totals(in, out, state, availableInBefore, availableOutBefore);
                        if (flush && !in->available_bytes)
                            state->process = DENSITY_BLOCK_ENCODE_PROCESS_WRITE_LAST_BLOCK_FOOTER;
                        else {
                            state->process = DENSITY_BLOCK_ENCODE_PROCESS_WRITE_BLOCK_FOOTER;
                            if (!in->available_bytes)
                                return DENSITY_BLOCK_ENCODE_STATE_STALL_ON_INPUT_BUFFER;
                            else if (!out->available_bytes)
                                return DENSITY_BLOCK_ENCODE_STATE_STALL_ON_OUTPUT_BUFFER;
                        }

                    exit:
                        break;

                    case DENSITY_BLOCK_MODE_KERNEL:
                        kernelEncodeState = state->kernelEncodeProcess(in, out, state->kernelEncodeState, flush);
                        density_block_encode_update_totals(in, out, state, availableInBefore, availableOutBefore);

                        switch (kernelEncodeState) {
                            case DENSITY_KERNEL_ENCODE_STATE_STALL_ON_INPUT_BUFFER:
                                return DENSITY_BLOCK_ENCODE_STATE_STALL_ON_INPUT_BUFFER;

                            case DENSITY_KERNEL_ENCODE_STATE_STALL_ON_OUTPUT_BUFFER:
                                return DENSITY_BLOCK_ENCODE_STATE_STALL_ON_OUTPUT_BUFFER;

                            case DENSITY_KERNEL_ENCODE_STATE_INFO_NEW_BLOCK:
                                state->process = DENSITY_BLOCK_ENCODE_PROCESS_WRITE_BLOCK_FOOTER;
                                break;

                            case DENSITY_KERNEL_ENCODE_STATE_INFO_EFFICIENCY_CHECK:
                                state->process = DENSITY_BLOCK_ENCODE_PROCESS_WRITE_BLOCK_MODE_MARKER;
                                break;

                            case DENSITY_KERNEL_ENCODE_STATE_FINISHED:
                                state->process = DENSITY_BLOCK_ENCODE_PROCESS_WRITE_LAST_BLOCK_FOOTER;
                                break;

                            case DENSITY_KERNEL_ENCODE_STATE_READY:
                                break;

                            default:
                                return DENSITY_BLOCK_ENCODE_STATE_ERROR;
                        }
                        break;

                    default:
                        return DENSITY_BLOCK_ENCODE_STATE_ERROR;
                }
                break;

            default:
                return DENSITY_BLOCK_ENCODE_STATE_ERROR;
        }
    }
}

DENSITY_FORCE_INLINE DENSITY_BLOCK_ENCODE_STATE density_block_encode_finish(density_block_encode_state *restrict state) {
    if (state->process ^ DENSITY_BLOCK_ENCODE_PROCESS_FINISHED)
        return DENSITY_BLOCK_ENCODE_STATE_ERROR;

    state->kernelEncodeFinish(state->kernelEncodeState);

    return DENSITY_BLOCK_ENCODE_STATE_READY;
}