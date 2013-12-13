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
 * 18/10/13 22:41
 */

#ifndef DENSITY_API_H
#define DENSITY_API_H

#include <stdint.h>
#include <stdbool.h>



/***********************************************************************************************************************
 *                                                                                                                     *
 * Compilation switches (compression only)                                                                             *
 *                                                                                                                     *
 ***********************************************************************************************************************/

#define DENSITY_YES                                                 1
#define DENSITY_NO                                                  0

#define DENSITY_ENABLE_PARALLELIZABLE_DECOMPRESSIBLE_OUTPUT         DENSITY_NO          // No disables compression dictionary resets and improves compression ratio



/***********************************************************************************************************************
 *                                                                                                                     *
 * Structures useful for the API                                                                                       *
 *                                                                                                                     *
 ***********************************************************************************************************************/


typedef uint8_t density_byte;
typedef bool density_bool;
typedef struct {
    union {
        uint64_t as_uint64_t;
        density_byte as_bytes[8];
    };
} density_main_header_parameters;

typedef enum {
    DENSITY_COMPRESSION_MODE_COPY = 0,
    DENSITY_COMPRESSION_MODE_CHAMELEON_ALGORITHM = 1,
    DENSITY_COMPRESSION_MODE_MANDALA_ALGORITHM = 2,
} DENSITY_COMPRESSION_MODE;

typedef enum {
    DENSITY_ENCODE_OUTPUT_TYPE_DEFAULT = 0,
    DENSITY_ENCODE_OUTPUT_TYPE_WITHOUT_HEADER = 1,
    DENSITY_ENCODE_OUTPUT_TYPE_WITHOUT_FOOTER = 2,
    DENSITY_ENCODE_OUTPUT_TYPE_WITHOUT_HEADER_NOR_FOOTER = 3
} DENSITY_ENCODE_OUTPUT_TYPE;

typedef enum {
    DENSITY_BLOCK_TYPE_DEFAULT = 0,
    DENSITY_BLOCK_TYPE_NO_HASHSUM_INTEGRITY_CHECK = 1
} DENSITY_BLOCK_TYPE;

typedef enum {
    DENSITY_BUFFERS_STATE_OK = 0,                                       // ready to continue
    DENSITY_BUFFERS_STATE_ERROR_OUTPUT_BUFFER_TOO_SMALL,                // output buffer size is too small
    DENSITY_BUFFERS_STATE_ERROR_INVALID_STATE                           // error during processing
} DENSITY_BUFFERS_STATE;

typedef enum {
    DENSITY_STREAM_STATE_READY = 0,                                     // ready to continue
    DENSITY_STREAM_STATE_STALL_ON_INPUT_BUFFER,                         // input buffer has been completely read
    DENSITY_STREAM_STATE_STALL_ON_OUTPUT_BUFFER,                        // there is not enought space left in the output buffer to continue
    //DENSITY_STREAM_STATE_ERROR_INPUT_BUFFER_SIZE_NOT_MULTIPLE_OF_32,    // size of input buffer is not a multiple of 32
    DENSITY_STREAM_STATE_ERROR_OUTPUT_BUFFER_TOO_SMALL,                 // output buffer size is too small
    DENSITY_STREAM_STATE_ERROR_INVALID_INTERNAL_STATE                   // error during processing
} DENSITY_STREAM_STATE;

#pragma pack(push)
#pragma pack(4)
typedef struct {
    density_byte version[3];
    density_byte compressionMode;
    density_byte blockType;
    density_byte reserved[3];
    density_main_header_parameters parameters;
} density_main_header;

/*typedef struct {
    density_byte* pointer;
    uint_fast64_t position;
    uint_fast64_t size;
} density_byte_buffer;*/

typedef struct {
    density_byte* pointer;
    uint_fast64_t available_bytes;
} density_memory_location;

typedef struct {
    density_memory_location* in;
    uint_fast64_t* in_total_read;

    density_memory_location* out;
    uint_fast64_t* out_total_written;

    void* internal_state;
} density_stream;
#pragma pack(pop)



/***********************************************************************************************************************
 *                                                                                                                     *
 * Density version information                                                                                         *
 *                                                                                                                     *
 ***********************************************************************************************************************/

/*
 * Returns the major ssc version
 */
uint8_t density_version_major(void);

/*
 * Returns the minor ssc version
 */
uint8_t density_version_minor(void);

/*
 * Returns the ssc revision
 */
uint8_t density_version_revision(void);



/***********************************************************************************************************************
 *                                                                                                                     *
 * Density stream API functions                                                                                        *
 *                                                                                                                     *
 ***********************************************************************************************************************/

/*
 * Prepare a stream. This function *must* be called.
 *
 * @param stream the stream
 * @param mem_alloc a pointer to a memory allocation function. If NULL, the standard malloc(size_t) is used.
 * @param mem_free a pointer to a memory freeing function. If NULL, the standard free(void*) is used.
 */
DENSITY_STREAM_STATE density_stream_prepare(density_stream *stream, void *(*mem_alloc)(size_t), void (*mem_free)(void *));

/*
 * Initialize compression
 *
 * @param stream the stream
 * @param compression_mode the compression mode
 * @param output_type the format of data output by encoding.
 *      EXPERTS ONLY ! If unsure, use DENSITY_ENCODE_OUTPUT_TYPE_DEFAULT.
 *      Any other option will create output data which is *NOT* directly decompressible by the API. This can be used for parallelizing Density.
 * @param block_type the type of data blocks Density will generate.
 *      EXPERTS ONLY ! If you're unsure use DENSITY_BLOCK_TYPE_DEFAULT.
 *      The option DENSITY_BLOCK_TYPE_NO_HASHSUM_INTEGRITY_CHECK basically makes the block footer size zero, and removes data integrity checks in the encoded output.
 *      It can be useful in network streaming situations, where data integrity is already checked by the protocol (TCP/IP for example), and the flush option in density_stream_compress is often set,
 *      as the absence of block footer will enhance compression ratio.
 */
DENSITY_STREAM_STATE density_stream_compress_init(density_stream *stream, const DENSITY_COMPRESSION_MODE compression_mode, const DENSITY_ENCODE_OUTPUT_TYPE output_type, const DENSITY_BLOCK_TYPE block_type);

/*
 * Stream decompression initialization
 *
 * @param stream the stream
 */
DENSITY_STREAM_STATE density_stream_decompress_init(density_stream *stream);

/*
 * Stream compression function, has to be called repetitively.
 * When the dataset in the input buffer is the last, flush has to be true. Otherwise it should be false at all times.
 *
 * @param stream the stream
 *      Please note that the input buffer size, if flush is false, *must* be a multiple of 32 otherwise an error will be returned.
 * @param flush a boolean indicating flush behaviour
 *      If set to true, this will ensure that every byte from the input buffer will have its counterpart in the output buffer.
 *      flush has to be true when the presented data is the last (end of a file for example).
 *      It can also be set to true multiple times to handle network streaming for example. In that case, please also check
 *      the block_type parameter of density_stream_compress_init to enable a better compression ratio. It is also worth noting that
 *      the *best* input buffer size for compression ratio matters should be a multiple of 256, any other size will also work but will
 *      incur a less than optimal compression ratio.
 */
DENSITY_STREAM_STATE density_stream_compress(density_stream *stream, const density_bool flush);

/*
 * Stream decompression function, has to be called repetitively.
 * When the dataset in the input buffer is the last, flush has to be true. Otherwise it should be false at all times.
 *
 * @param stream the stream
 * @param flush a boolean indicating flush behaviour
 *      If set to true, this will ensure that every byte from the input buffer will have its counterpart in the output buffer.
 *      flush has to be true when the presented data is the last (end of a file for example)
 *      It can also be set to true multiple times to handle network streaming for example.
 */
DENSITY_STREAM_STATE density_stream_decompress(density_stream *stream, const density_bool flush);

/*
 * Call once processing is finished, to clear up the environment and release eventual allocated memory.
 *
 * @param stream the stream
 */
DENSITY_STREAM_STATE density_stream_compress_finish(density_stream *stream);

/*
 * Call once processing is finished, to clear up the environment and release eventual allocated memory.
 *
 * @param stream the stream
 */
DENSITY_STREAM_STATE density_stream_decompress_finish(density_stream *stream);

/*
 * Returns the header that was read during density_stream_decompress_init.
 *
 * @param stream the stream
 * @param header the header returned
 */
DENSITY_STREAM_STATE density_stream_decompress_utilities_get_header(density_stream* stream, density_main_header* header);



/***********************************************************************************************************************
 *                                                                                                                     *
 * Density buffers API functions                                                                                       *
 *                                                                                                                     *
 ***********************************************************************************************************************/

/*
 * Returns the max compressed length possible (with incompressible data)
 *
 * @param result a pointer to return the resulting length to
 * @param in_length, the length of the input data to compress
 * @param compression_mode the compression mode to be used
 */
DENSITY_BUFFERS_STATE density_buffers_max_compressed_length(uint_fast64_t * result, uint_fast64_t in_length, const DENSITY_COMPRESSION_MODE compression_mode);

/*
 * Buffers compression function
 *
 * @param total_written a pointer to return the total bytes written to, can be NULL for no return
 * @param in the input buffer to compress
 * @param in_size the size of the input buffer in bytes
 * @param out the output buffer
 * @param out_size the size of the output buffer in bytes
 * @param compression_mode the compression mode to use
 * @param output_type the output type to use (if unsure, DENSITY_ENCODE_OUTPUT_TYPE_DEFAULT), see the density_stream_compress documentation
 * @param block_type the block type to use (if unsure, DENSITY_BLOCK_TYPE_DEFAULT), see the density_stream_compress documentation
 * @param mem_alloc a pointer to a memory allocation function. If NULL, the standard malloc(size_t) is used.
 * @param mem_free a pointer to a memory freeing function. If NULL, the standard free(void*) is used.
 */
DENSITY_BUFFERS_STATE density_buffers_compress(uint_fast64_t* total_written, uint8_t *in, uint_fast64_t in_size, uint8_t *out, uint_fast64_t out_size, const DENSITY_COMPRESSION_MODE compression_mode, const DENSITY_ENCODE_OUTPUT_TYPE output_type, const DENSITY_BLOCK_TYPE block_type, void *(*mem_alloc)(size_t), void (*mem_free)(void *));

/*
 * Buffers decompression function
 *
 * @param total_written a pointer to return the total bytes written to, can be NULL for no return
 * @param header a pointer to return the header read to, can be NULL for no return
 * @param in the input buffer to decompress
 * @param in_size the size of the input buffer in bytes
 * @param out the output buffer
 * @param out_size the size of the output buffer in bytes
 * @param mem_alloc a pointer to a memory allocation function. If NULL, the standard malloc(size_t) is used.
 * @param mem_free a pointer to a memory freeing function. If NULL, the standard free(void*) is used.
 */
DENSITY_BUFFERS_STATE density_buffers_decompress(uint_fast64_t * total_written, density_main_header* header, uint8_t *in, uint_fast64_t in_size, uint8_t *out, uint_fast64_t out_size, void *(*mem_alloc)(size_t), void (*mem_free)(void *));

#endif