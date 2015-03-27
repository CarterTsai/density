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
 * 11/10/13 02:01
 */

#ifndef DENSITY_GLOBALS_H
#define DENSITY_GLOBALS_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stddef.h>

#if defined(__INTEL_COMPILER)
#define DENSITY_FORCE_INLINE __forceinline
#elif defined(_MSC_VER)
#define DENSITY_FORCE_INLINE __forceinline
#elif defined(__GNUC__)
#define DENSITY_FORCE_INLINE inline __attribute__((always_inline))
#elif defined(__clang__)
#define DENSITY_FORCE_INLINE inline __attribute__((always_inline))
#else
#warning Impossible to force functions inlining. Expect performance issues.
#endif

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define DENSITY_LITTLE_ENDIAN_64(b)   ((uint64_t)b)
#define DENSITY_LITTLE_ENDIAN_32(b)   ((uint32_t)b)
#define DENSITY_LITTLE_ENDIAN_16(b)   ((uint16_t)b)
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#if __GNUC__ * 100 + __GNUC_MINOR__ >= 403
#define DENSITY_LITTLE_ENDIAN_64(b)   __builtin_bswap64(b)
#define DENSITY_LITTLE_ENDIAN_32(b)   __builtin_bswap32(b)
#define DENSITY_LITTLE_ENDIAN_16(b)   __builtin_bswap16(b)
#else
#warning Using bulk byte swap routines. Expect performance issues.
#define DENSITY_LITTLE_ENDIAN_64(b)   ((((b) & 0xFF00000000000000ull) >> 56) | (((b) & 0x00FF000000000000ull) >> 40) | (((b) & 0x0000FF0000000000ull) >> 24) | (((b) & 0x000000FF00000000ull) >> 8) | (((b) & 0x00000000FF000000ull) << 8) | (((b) & 0x0000000000FF0000ull) << 24ull) | (((b) & 0x000000000000FF00ull) << 40) | (((b) & 0x00000000000000FFull) << 56))
#define DENSITY_LITTLE_ENDIAN_32(b)   ((((b) & 0xFF000000) >> 24) | (((b) & 0x00FF0000) >> 8) | (((b) & 0x0000FF00) << 8) | (((b) & 0x000000FF) << 24))
#define DENSITY_LITTLE_ENDIAN_16(b)   ((((b) & 0xFF00) >> 8) | (((b) & 0x00FF) << 8))
#endif
#else
#error Unknow endianness
#endif

#define DENSITY_FORMAT(v)               0##v##llu

#define DENSITY_ISOLATE(b, p)           ((DENSITY_FORMAT(b) / p) & 0x1)

#define DENSITY_BINARY_TO_UINT(b)        ((DENSITY_ISOLATE(b, 1llu) ? 0x1 : 0)\
                                        + (DENSITY_ISOLATE(b, 8llu) ? 0x2 : 0)\
                                        + (DENSITY_ISOLATE(b, 64llu) ? 0x4 : 0)\
                                        + (DENSITY_ISOLATE(b, 512llu) ? 0x8 : 0)\
                                        + (DENSITY_ISOLATE(b, 4096llu) ? 0x10 : 0)\
                                        + (DENSITY_ISOLATE(b, 32768llu) ? 0x20 : 0)\
                                        + (DENSITY_ISOLATE(b, 262144llu) ? 0x40 : 0)\
                                        + (DENSITY_ISOLATE(b, 2097152llu) ? 0x80 : 0)\
                                        + (DENSITY_ISOLATE(b, 16777216llu) ? 0x100 : 0)\
                                        + (DENSITY_ISOLATE(b, 134217728llu) ? 0x200 : 0)\
                                        + (DENSITY_ISOLATE(b, 1073741824llu) ? 0x400 : 0)\
                                        + (DENSITY_ISOLATE(b, 8589934592llu) ? 0x800 : 0)\
                                        + (DENSITY_ISOLATE(b, 68719476736llu) ? 0x1000 : 0)\
                                        + (DENSITY_ISOLATE(b, 549755813888llu) ? 0x2000 : 0)\
                                        + (DENSITY_ISOLATE(b, 4398046511104llu) ? 0x4000 : 0)\
                                        + (DENSITY_ISOLATE(b, 35184372088832llu) ? 0x8000 : 0)\
                                        + (DENSITY_ISOLATE(b, 281474976710656llu) ? 0x10000 : 0)\
                                        + (DENSITY_ISOLATE(b, 2251799813685248llu) ? 0x20000 : 0))

#define DENSITY_UNROLL_2(op)     op; op
#define DENSITY_UNROLL_4(op)     DENSITY_UNROLL_2(op);    DENSITY_UNROLL_2(op)
#define DENSITY_UNROLL_8(op)     DENSITY_UNROLL_4(op);    DENSITY_UNROLL_4(op)
#define DENSITY_UNROLL_16(op)    DENSITY_UNROLL_8(op);    DENSITY_UNROLL_8(op)
#define DENSITY_UNROLL_32(op)    DENSITY_UNROLL_16(op);   DENSITY_UNROLL_16(op)
#define DENSITY_UNROLL_64(op)    DENSITY_UNROLL_32(op);   DENSITY_UNROLL_32(op)

#define DENSITY_CASE_GENERATOR_2(op_a, flag_a, op_b, flag_b, op_mid, shift)\
    case ((flag_b << shift) | flag_a):\
        op_a;\
        op_mid;\
        op_b;\
        break;

#define DENSITY_CASE_GENERATOR_4(op_a, flag_a, op_b, flag_b, op_c, flag_c, op_d, flag_d, op_mid, shift)\
    case ((flag_d << (shift * 3)) | (flag_c << (shift * 2)) | (flag_b << shift) | flag_a):\
        op_a;\
        op_mid;\
        op_b;\
        op_mid;\
        op_c;\
        op_mid;\
        op_d;\
        break;

#define DENSITY_CASE_GENERATOR_8(op_a, flag_a, op_b, flag_b, op_c, flag_c, op_d, flag_d, op_e, flag_e, op_f, flag_f, op_g, flag_g, op_h, flag_h, op_mid, shift)\
    case ((flag_h << (shift * 7)) | (flag_g << (shift * 6)) | (flag_f << (shift * 5)) | (flag_e << (shift * 4)) | (flag_d << (shift * 3)) | (flag_c << (shift * 2)) | (flag_b << shift) | flag_a):\
        op_a;\
        op_mid;\
        op_b;\
        op_mid;\
        op_c;\
        op_mid;\
        op_d;\
        op_mid;\
        op_e;\
        op_mid;\
        op_f;\
        op_mid;\
        op_g;\
        op_mid;\
        op_h;\
        break;

#define DENSITY_CASE_GENERATOR_4_2_LAST_1_COMBINED(op_1, flag_1, op_a, flag_a, op_b, flag_b, op_c, flag_c, op_d, flag_d, op_mid, shift)\
    DENSITY_CASE_GENERATOR_2(op_1, flag_1, op_a, flag_a, op_mid, shift);\
    DENSITY_CASE_GENERATOR_2(op_1, flag_1, op_b, flag_b, op_mid, shift);\
    DENSITY_CASE_GENERATOR_2(op_1, flag_1, op_c, flag_c, op_mid, shift);\
    DENSITY_CASE_GENERATOR_2(op_1, flag_1, op_d, flag_d, op_mid, shift);

#define DENSITY_CASE_GENERATOR_4_2_COMBINED(op_a, flag_a, op_b, flag_b, op_c, flag_c, op_d, flag_d, op_mid, shift)\
    DENSITY_CASE_GENERATOR_4_2_LAST_1_COMBINED(op_a, flag_a, op_a, flag_a, op_b, flag_b, op_c, flag_c, op_d, flag_d, op_mid, shift);\
    DENSITY_CASE_GENERATOR_4_2_LAST_1_COMBINED(op_b, flag_b, op_a, flag_a, op_b, flag_b, op_c, flag_c, op_d, flag_d, op_mid, shift);\
    DENSITY_CASE_GENERATOR_4_2_LAST_1_COMBINED(op_c, flag_c, op_a, flag_a, op_b, flag_b, op_c, flag_c, op_d, flag_d, op_mid, shift);\
    DENSITY_CASE_GENERATOR_4_2_LAST_1_COMBINED(op_d, flag_d, op_a, flag_a, op_b, flag_b, op_c, flag_c, op_d, flag_d, op_mid, shift);

#define DENSITY_CASE_GENERATOR_2_4_LAST_1_COMBINED(op_1, flag_1, op_2, flag_2, op_3, flag_3, op_a, flag_a, op_b, flag_b, op_mid, shift)\
    DENSITY_CASE_GENERATOR_4(op_1, flag_1, op_2, flag_2, op_3, flag_3, op_a, flag_a, op_mid, shift);\
    DENSITY_CASE_GENERATOR_4(op_1, flag_1, op_2, flag_2, op_3, flag_3, op_b, flag_b, op_mid, shift);

#define DENSITY_CASE_GENERATOR_2_4_LAST_2_COMBINED(op_1, flag_1, op_2, flag_2, op_a, flag_a, op_b, flag_b, op_mid, shift)\
    DENSITY_CASE_GENERATOR_2_4_LAST_1_COMBINED(op_1, flag_1, op_2, flag_2, op_a, flag_a, op_a, flag_a, op_b, flag_b, op_mid, shift);\
    DENSITY_CASE_GENERATOR_2_4_LAST_1_COMBINED(op_1, flag_1, op_2, flag_2, op_b, flag_b, op_a, flag_a, op_b, flag_b, op_mid, shift);

#define DENSITY_CASE_GENERATOR_2_4_LAST_3_COMBINED(op_1, flag_1, op_a, flag_a, op_b, flag_b, op_mid, shift)\
    DENSITY_CASE_GENERATOR_2_4_LAST_2_COMBINED(op_1, flag_1, op_a, flag_a, op_a, flag_a, op_b, flag_b, op_mid, shift);\
    DENSITY_CASE_GENERATOR_2_4_LAST_2_COMBINED(op_1, flag_1, op_b, flag_b, op_a, flag_a, op_b, flag_b, op_mid, shift);

#define DENSITY_CASE_GENERATOR_2_4_COMBINED(op_a, flag_a, op_b, flag_b, op_mid, shift)\
    DENSITY_CASE_GENERATOR_2_4_LAST_3_COMBINED(op_a, flag_a, op_a, flag_a, op_b, flag_b, op_mid, shift);\
    DENSITY_CASE_GENERATOR_2_4_LAST_3_COMBINED(op_b, flag_b, op_a, flag_a, op_b, flag_b, op_mid, shift);

#define DENSITY_CASE_GENERATOR_2_8_LAST_1_COMBINED(op_1, flag_1, op_2, flag_2, op_3, flag_3, op_4, flag_4, op_5, flag_5, op_6, flag_6, op_7, flag_7, op_a, flag_a, op_b, flag_b, op_mid, shift)\
    DENSITY_CASE_GENERATOR_8(op_1, flag_1, op_2, flag_2, op_3, flag_3, op_4, flag_4, op_5, flag_5, op_6, flag_6, op_7, flag_7, op_a, flag_a, op_mid, shift);\
    DENSITY_CASE_GENERATOR_8(op_1, flag_1, op_2, flag_2, op_3, flag_3, op_4, flag_4, op_5, flag_5, op_6, flag_6, op_7, flag_7, op_b, flag_b, op_mid, shift);

#define DENSITY_CASE_GENERATOR_2_8_LAST_2_COMBINED(op_1, flag_1, op_2, flag_2, op_3, flag_3, op_4, flag_4, op_5, flag_5, op_6, flag_6, op_a, flag_a, op_b, flag_b, op_mid, shift)\
    DENSITY_CASE_GENERATOR_2_8_LAST_1_COMBINED(op_1, flag_1, op_2, flag_2, op_3, flag_3, op_4, flag_4, op_5, flag_5, op_6, flag_6, op_a, flag_a, op_a, flag_a, op_b, flag_b, op_mid, shift);\
    DENSITY_CASE_GENERATOR_2_8_LAST_1_COMBINED(op_1, flag_1, op_2, flag_2, op_3, flag_3, op_4, flag_4, op_5, flag_5, op_6, flag_6, op_b, flag_b, op_a, flag_a, op_b, flag_b, op_mid, shift);

#define DENSITY_CASE_GENERATOR_2_8_LAST_3_COMBINED(op_1, flag_1, op_2, flag_2, op_3, flag_3, op_4, flag_4, op_5, flag_5, op_a, flag_a, op_b, flag_b, op_mid, shift)\
    DENSITY_CASE_GENERATOR_2_8_LAST_2_COMBINED(op_1, flag_1, op_2, flag_2, op_3, flag_3, op_4, flag_4, op_5, flag_5, op_a, flag_a, op_a, flag_a, op_b, flag_b, op_mid, shift);\
    DENSITY_CASE_GENERATOR_2_8_LAST_2_COMBINED(op_1, flag_1, op_2, flag_2, op_3, flag_3, op_4, flag_4, op_5, flag_5, op_b, flag_b, op_a, flag_a, op_b, flag_b, op_mid, shift);

#define DENSITY_CASE_GENERATOR_2_8_LAST_4_COMBINED(op_1, flag_1, op_2, flag_2, op_3, flag_3, op_4, flag_4, op_a, flag_a, op_b, flag_b, op_mid, shift)\
    DENSITY_CASE_GENERATOR_2_8_LAST_3_COMBINED(op_1, flag_1, op_2, flag_2, op_3, flag_3, op_4, flag_4, op_a, flag_a, op_a, flag_a, op_b, flag_b, op_mid, shift);\
    DENSITY_CASE_GENERATOR_2_8_LAST_3_COMBINED(op_1, flag_1, op_2, flag_2, op_3, flag_3, op_4, flag_4, op_b, flag_b, op_a, flag_a, op_b, flag_b, op_mid, shift);

#define DENSITY_CASE_GENERATOR_2_8_LAST_5_COMBINED(op_1, flag_1, op_2, flag_2, op_3, flag_3, op_a, flag_a, op_b, flag_b, op_mid, shift)\
    DENSITY_CASE_GENERATOR_2_8_LAST_4_COMBINED(op_1, flag_1, op_2, flag_2, op_3, flag_3, op_a, flag_a, op_a, flag_a, op_b, flag_b, op_mid, shift);\
    DENSITY_CASE_GENERATOR_2_8_LAST_4_COMBINED(op_1, flag_1, op_2, flag_2, op_3, flag_3, op_b, flag_b, op_a, flag_a, op_b, flag_b, op_mid, shift);

#define DENSITY_CASE_GENERATOR_2_8_LAST_6_COMBINED(op_1, flag_1, op_2, flag_2, op_a, flag_a, op_b, flag_b, op_mid, shift)\
    DENSITY_CASE_GENERATOR_2_8_LAST_5_COMBINED(op_1, flag_1, op_2, flag_2, op_a, flag_a, op_a, flag_a, op_b, flag_b, op_mid, shift);\
    DENSITY_CASE_GENERATOR_2_8_LAST_5_COMBINED(op_1, flag_1, op_2, flag_2, op_b, flag_b, op_a, flag_a, op_b, flag_b, op_mid, shift);

#define DENSITY_CASE_GENERATOR_2_8_LAST_7_COMBINED(op_1, flag_1, op_a, flag_a, op_b, flag_b, op_mid, shift)\
    DENSITY_CASE_GENERATOR_2_8_LAST_6_COMBINED(op_1, flag_1, op_a, flag_a, op_a, flag_a, op_b, flag_b, op_mid, shift);\
    DENSITY_CASE_GENERATOR_2_8_LAST_6_COMBINED(op_1, flag_1, op_b, flag_b, op_a, flag_a, op_b, flag_b, op_mid, shift);

#define DENSITY_CASE_GENERATOR_2_8_COMBINED(op_a, flag_a, op_b, flag_b, op_mid, shift)\
    DENSITY_CASE_GENERATOR_2_8_LAST_7_COMBINED(op_a, flag_a, op_a, flag_a, op_b, flag_b, op_mid, shift);\
    DENSITY_CASE_GENERATOR_2_8_LAST_7_COMBINED(op_b, flag_b, op_a, flag_a, op_b, flag_b, op_mid, shift);

#define DENSITY_CASE_GENERATOR_4_4_LAST_1_COMBINED(op_1, flag_1, op_2, flag_2, op_3, flag_3, op_a, flag_a, op_b, flag_b, op_c, flag_c, op_d, flag_d, op_mid, shift)\
    DENSITY_CASE_GENERATOR_4(op_1, flag_1, op_2, flag_2, op_3, flag_3, op_a, flag_a, op_mid, shift);\
    DENSITY_CASE_GENERATOR_4(op_1, flag_1, op_2, flag_2, op_3, flag_3, op_b, flag_b, op_mid, shift);\
    DENSITY_CASE_GENERATOR_4(op_1, flag_1, op_2, flag_2, op_3, flag_3, op_c, flag_c, op_mid, shift);\
    DENSITY_CASE_GENERATOR_4(op_1, flag_1, op_2, flag_2, op_3, flag_3, op_d, flag_d, op_mid, shift);

#define DENSITY_CASE_GENERATOR_4_4_LAST_2_COMBINED(op_1, flag_1, op_2, flag_2, op_a, flag_a, op_b, flag_b, op_c, flag_c, op_d, flag_d, op_mid, shift)\
    DENSITY_CASE_GENERATOR_4_4_LAST_1_COMBINED(op_1, flag_1, op_2, flag_2, op_a, flag_a, op_a, flag_a, op_b, flag_b, op_c, flag_c, op_d, flag_d, op_mid, shift);\
    DENSITY_CASE_GENERATOR_4_4_LAST_1_COMBINED(op_1, flag_1, op_2, flag_2, op_b, flag_b, op_a, flag_a, op_b, flag_b, op_c, flag_c, op_d, flag_d, op_mid, shift);\
    DENSITY_CASE_GENERATOR_4_4_LAST_1_COMBINED(op_1, flag_1, op_2, flag_2, op_c, flag_c, op_a, flag_a, op_b, flag_b, op_c, flag_c, op_d, flag_d, op_mid, shift);\
    DENSITY_CASE_GENERATOR_4_4_LAST_1_COMBINED(op_1, flag_1, op_2, flag_2, op_d, flag_d, op_a, flag_a, op_b, flag_b, op_c, flag_c, op_d, flag_d, op_mid, shift);

#define DENSITY_CASE_GENERATOR_4_4_LAST_3_COMBINED(op_1, flag_1, op_a, flag_a, op_b, flag_b, op_c, flag_c, op_d, flag_d, op_mid, shift)\
    DENSITY_CASE_GENERATOR_4_4_LAST_2_COMBINED(op_1, flag_1, op_a, flag_a, op_a, flag_a, op_b, flag_b, op_c, flag_c, op_d, flag_d, op_mid, shift);\
    DENSITY_CASE_GENERATOR_4_4_LAST_2_COMBINED(op_1, flag_1, op_b, flag_b, op_a, flag_a, op_b, flag_b, op_c, flag_c, op_d, flag_d, op_mid, shift);\
    DENSITY_CASE_GENERATOR_4_4_LAST_2_COMBINED(op_1, flag_1, op_c, flag_c, op_a, flag_a, op_b, flag_b, op_c, flag_c, op_d, flag_d, op_mid, shift);\
    DENSITY_CASE_GENERATOR_4_4_LAST_2_COMBINED(op_1, flag_1, op_d, flag_d, op_a, flag_a, op_b, flag_b, op_c, flag_c, op_d, flag_d, op_mid, shift);

#define DENSITY_CASE_GENERATOR_4_4_COMBINED(op_a, flag_a, op_b, flag_b, op_c, flag_c, op_d, flag_d, op_mid, shift)\
    DENSITY_CASE_GENERATOR_4_4_LAST_3_COMBINED(op_a, flag_a, op_a, flag_a, op_b, flag_b, op_c, flag_c, op_d, flag_d, op_mid, shift);\
    DENSITY_CASE_GENERATOR_4_4_LAST_3_COMBINED(op_b, flag_b, op_a, flag_a, op_b, flag_b, op_c, flag_c, op_d, flag_d, op_mid, shift);\
    DENSITY_CASE_GENERATOR_4_4_LAST_3_COMBINED(op_c, flag_c, op_a, flag_a, op_b, flag_b, op_c, flag_c, op_d, flag_d, op_mid, shift);\
    DENSITY_CASE_GENERATOR_4_4_LAST_3_COMBINED(op_d, flag_d, op_a, flag_a, op_b, flag_b, op_c, flag_c, op_d, flag_d, op_mid, shift);

#define DENSITY_DICTIONARY_PREFERRED_RESET_CYCLE_SHIFT    6
#define DENSITY_DICTIONARY_PREFERRED_RESET_CYCLE          (1 << DENSITY_DICTIONARY_PREFERRED_RESET_CYCLE_SHIFT)

#define density_likely(x)                         __builtin_expect(!!(x), 1)
#define density_unlikely(x)                       __builtin_expect(!!(x), 0)

#define density_bitsizeof(x) (8 * sizeof(x))


/**********************************************************************************************************************
 *                                                                                                                    *
 * Global compile-time switches                                                                                       *
 *                                                                                                                    *
 **********************************************************************************************************************/

#define DENSITY_YES 1
#define DENSITY_NO  0

#define DENSITY_MAJOR_VERSION   0
#define DENSITY_MINOR_VERSION   12
#define DENSITY_REVISION        1

/*
 * Compile-time switches useful for pure data encoding and decoding
 * They enable/disable writing of the main headers and footers
 */
#define DENSITY_WRITE_MAIN_HEADER   DENSITY_YES
#define DENSITY_WRITE_MAIN_FOOTER   DENSITY_YES

/*
 * If set to yes, created output's decompression is parallelizable.
 * If set to no, compression dictionary resets are disabled and therefore compression ratio is improved
 */
#define DENSITY_ENABLE_PARALLELIZABLE_DECOMPRESSIBLE_OUTPUT DENSITY_NO



#endif
