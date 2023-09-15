#ifndef APIWRAPPER_READER_H
#define APIWRAPPER_READER_H

/* Generated by flatcc 0.6.2 FlatBuffers schema compiler for C by dvide.com */

#ifndef FLATBUFFERS_COMMON_READER_H
#include "flatbuffers_common_reader.h"
#endif
#include "flatcc/flatcc_flatbuffers.h"
#ifndef __alignas_is_defined
#include <stdalign.h>
#endif
#include "flatcc/flatcc_prologue.h"
#ifndef flatbuffers_identifier
#define flatbuffers_identifier 0
#endif
#ifndef flatbuffers_extension
#define flatbuffers_extension "bin"
#endif


typedef const struct FunctionAddresses_table *FunctionAddresses_table_t;
typedef struct FunctionAddresses_table *FunctionAddresses_mutable_table_t;
typedef const flatbuffers_uoffset_t *FunctionAddresses_vec_t;
typedef flatbuffers_uoffset_t *FunctionAddresses_mutable_vec_t;
#ifndef FunctionAddresses_file_identifier
#define FunctionAddresses_file_identifier 0
#endif
/* deprecated, use FunctionAddresses_file_identifier */
#ifndef FunctionAddresses_identifier
#define FunctionAddresses_identifier 0
#endif
#define FunctionAddresses_type_hash ((flatbuffers_thash_t)0x73e9a2df)
#define FunctionAddresses_type_identifier "\xdf\xa2\xe9\x73"
#ifndef FunctionAddresses_file_extension
#define FunctionAddresses_file_extension "bin"
#endif



struct FunctionAddresses_table { uint8_t unused__; };

static inline size_t FunctionAddresses_vec_len(FunctionAddresses_vec_t vec)
__flatbuffers_vec_len(vec)
static inline FunctionAddresses_table_t FunctionAddresses_vec_at(FunctionAddresses_vec_t vec, size_t i)
__flatbuffers_offset_vec_at(FunctionAddresses_table_t, vec, i, 0)
__flatbuffers_table_as_root(FunctionAddresses)

__flatbuffers_define_vector_field(0, FunctionAddresses, names, flatbuffers_string_vec_t, 0)
__flatbuffers_define_vector_field(1, FunctionAddresses, addrs, flatbuffers_uint64_vec_t, 0)


#include "flatcc/flatcc_epilogue.h"
#endif /* APIWRAPPER_READER_H */
