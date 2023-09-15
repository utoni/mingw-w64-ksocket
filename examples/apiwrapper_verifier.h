#ifndef APIWRAPPER_VERIFIER_H
#define APIWRAPPER_VERIFIER_H

/* Generated by flatcc 0.6.2 FlatBuffers schema compiler for C by dvide.com */

#ifndef APIWRAPPER_READER_H
#include "apiwrapper_reader.h"
#endif
#include "flatcc/flatcc_verifier.h"
#include "flatcc/flatcc_prologue.h"

static int FunctionAddresses_verify_table(flatcc_table_verifier_descriptor_t *td);

static int FunctionAddresses_verify_table(flatcc_table_verifier_descriptor_t *td)
{
    int ret;
    if ((ret = flatcc_verify_string_vector_field(td, 0, 0) /* names */)) return ret;
    if ((ret = flatcc_verify_vector_field(td, 1, 0, 8, 8, INT64_C(536870911)) /* addrs */)) return ret;
    return flatcc_verify_ok;
}

static inline int FunctionAddresses_verify_as_root(const void *buf, size_t bufsiz)
{
    return flatcc_verify_table_as_root(buf, bufsiz, FunctionAddresses_identifier, &FunctionAddresses_verify_table);
}

static inline int FunctionAddresses_verify_as_typed_root(const void *buf, size_t bufsiz)
{
    return flatcc_verify_table_as_root(buf, bufsiz, FunctionAddresses_type_identifier, &FunctionAddresses_verify_table);
}

static inline int FunctionAddresses_verify_as_root_with_identifier(const void *buf, size_t bufsiz, const char *fid)
{
    return flatcc_verify_table_as_root(buf, bufsiz, fid, &FunctionAddresses_verify_table);
}

static inline int FunctionAddresses_verify_as_root_with_type_hash(const void *buf, size_t bufsiz, flatbuffers_thash_t thash)
{
    return flatcc_verify_table_as_typed_root(buf, bufsiz, thash, &FunctionAddresses_verify_table);
}

#include "flatcc/flatcc_epilogue.h"
#endif /* APIWRAPPER_VERIFIER_H */