#ifndef MONSTER_READER_H
#define MONSTER_READER_H

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

typedef struct MyGame_Sample_Vec3 MyGame_Sample_Vec3_t;
typedef const MyGame_Sample_Vec3_t *MyGame_Sample_Vec3_struct_t;
typedef MyGame_Sample_Vec3_t *MyGame_Sample_Vec3_mutable_struct_t;
typedef const MyGame_Sample_Vec3_t *MyGame_Sample_Vec3_vec_t;
typedef MyGame_Sample_Vec3_t *MyGame_Sample_Vec3_mutable_vec_t;

typedef const struct MyGame_Sample_Monster_table *MyGame_Sample_Monster_table_t;
typedef struct MyGame_Sample_Monster_table *MyGame_Sample_Monster_mutable_table_t;
typedef const flatbuffers_uoffset_t *MyGame_Sample_Monster_vec_t;
typedef flatbuffers_uoffset_t *MyGame_Sample_Monster_mutable_vec_t;
typedef const struct MyGame_Sample_Weapon_table *MyGame_Sample_Weapon_table_t;
typedef struct MyGame_Sample_Weapon_table *MyGame_Sample_Weapon_mutable_table_t;
typedef const flatbuffers_uoffset_t *MyGame_Sample_Weapon_vec_t;
typedef flatbuffers_uoffset_t *MyGame_Sample_Weapon_mutable_vec_t;
#ifndef MyGame_Sample_Vec3_file_identifier
#define MyGame_Sample_Vec3_file_identifier 0
#endif
/* deprecated, use MyGame_Sample_Vec3_file_identifier */
#ifndef MyGame_Sample_Vec3_identifier
#define MyGame_Sample_Vec3_identifier 0
#endif
#define MyGame_Sample_Vec3_type_hash ((flatbuffers_thash_t)0x950fd0dc)
#define MyGame_Sample_Vec3_type_identifier "\xdc\xd0\x0f\x95"
#ifndef MyGame_Sample_Vec3_file_extension
#define MyGame_Sample_Vec3_file_extension "bin"
#endif
#ifndef MyGame_Sample_Monster_file_identifier
#define MyGame_Sample_Monster_file_identifier 0
#endif
/* deprecated, use MyGame_Sample_Monster_file_identifier */
#ifndef MyGame_Sample_Monster_identifier
#define MyGame_Sample_Monster_identifier 0
#endif
#define MyGame_Sample_Monster_type_hash ((flatbuffers_thash_t)0xd5be61b)
#define MyGame_Sample_Monster_type_identifier "\x1b\xe6\x5b\x0d"
#ifndef MyGame_Sample_Monster_file_extension
#define MyGame_Sample_Monster_file_extension "bin"
#endif
#ifndef MyGame_Sample_Weapon_file_identifier
#define MyGame_Sample_Weapon_file_identifier 0
#endif
/* deprecated, use MyGame_Sample_Weapon_file_identifier */
#ifndef MyGame_Sample_Weapon_identifier
#define MyGame_Sample_Weapon_identifier 0
#endif
#define MyGame_Sample_Weapon_type_hash ((flatbuffers_thash_t)0x22292e53)
#define MyGame_Sample_Weapon_type_identifier "\x53\x2e\x29\x22"
#ifndef MyGame_Sample_Weapon_file_extension
#define MyGame_Sample_Weapon_file_extension "bin"
#endif

typedef int8_t MyGame_Sample_Color_enum_t;
__flatbuffers_define_integer_type(MyGame_Sample_Color, MyGame_Sample_Color_enum_t, 8)
#define MyGame_Sample_Color_Red ((MyGame_Sample_Color_enum_t)INT8_C(0))
#define MyGame_Sample_Color_Green ((MyGame_Sample_Color_enum_t)INT8_C(1))
#define MyGame_Sample_Color_Blue ((MyGame_Sample_Color_enum_t)INT8_C(2))

static inline const char *MyGame_Sample_Color_name(MyGame_Sample_Color_enum_t value)
{
    switch (value) {
    case MyGame_Sample_Color_Red: return "Red";
    case MyGame_Sample_Color_Green: return "Green";
    case MyGame_Sample_Color_Blue: return "Blue";
    default: return "";
    }
}

static inline int MyGame_Sample_Color_is_known_value(MyGame_Sample_Color_enum_t value)
{
    switch (value) {
    case MyGame_Sample_Color_Red: return 1;
    case MyGame_Sample_Color_Green: return 1;
    case MyGame_Sample_Color_Blue: return 1;
    default: return 0;
    }
}


struct MyGame_Sample_Vec3 {
    alignas(4) float x;
    alignas(4) float y;
    alignas(4) float z;
};
static_assert(sizeof(MyGame_Sample_Vec3_t) == 12, "struct size mismatch");

static inline const MyGame_Sample_Vec3_t *MyGame_Sample_Vec3__const_ptr_add(const MyGame_Sample_Vec3_t *p, size_t i) { return p + i; }
static inline MyGame_Sample_Vec3_t *MyGame_Sample_Vec3__ptr_add(MyGame_Sample_Vec3_t *p, size_t i) { return p + i; }
static inline MyGame_Sample_Vec3_struct_t MyGame_Sample_Vec3_vec_at(MyGame_Sample_Vec3_vec_t vec, size_t i)
__flatbuffers_struct_vec_at(vec, i)
static inline size_t MyGame_Sample_Vec3__size(void) { return 12; }
static inline size_t MyGame_Sample_Vec3_vec_len(MyGame_Sample_Vec3_vec_t vec)
__flatbuffers_vec_len(vec)
__flatbuffers_struct_as_root(MyGame_Sample_Vec3)

__flatbuffers_define_struct_scalar_field(MyGame_Sample_Vec3, x, flatbuffers_float, float)
__flatbuffers_define_struct_scalar_field(MyGame_Sample_Vec3, y, flatbuffers_float, float)
__flatbuffers_define_struct_scalar_field(MyGame_Sample_Vec3, z, flatbuffers_float, float)

typedef uint8_t MyGame_Sample_Equipment_union_type_t;
__flatbuffers_define_integer_type(MyGame_Sample_Equipment, MyGame_Sample_Equipment_union_type_t, 8)
__flatbuffers_define_union(flatbuffers_, MyGame_Sample_Equipment)
#define MyGame_Sample_Equipment_NONE ((MyGame_Sample_Equipment_union_type_t)UINT8_C(0))
#define MyGame_Sample_Equipment_Weapon ((MyGame_Sample_Equipment_union_type_t)UINT8_C(1))

static inline const char *MyGame_Sample_Equipment_type_name(MyGame_Sample_Equipment_union_type_t type)
{
    switch (type) {
    case MyGame_Sample_Equipment_NONE: return "NONE";
    case MyGame_Sample_Equipment_Weapon: return "Weapon";
    default: return "";
    }
}

static inline int MyGame_Sample_Equipment_is_known_type(MyGame_Sample_Equipment_union_type_t type)
{
    switch (type) {
    case MyGame_Sample_Equipment_NONE: return 1;
    case MyGame_Sample_Equipment_Weapon: return 1;
    default: return 0;
    }
}


struct MyGame_Sample_Monster_table { uint8_t unused__; };

static inline size_t MyGame_Sample_Monster_vec_len(MyGame_Sample_Monster_vec_t vec)
__flatbuffers_vec_len(vec)
static inline MyGame_Sample_Monster_table_t MyGame_Sample_Monster_vec_at(MyGame_Sample_Monster_vec_t vec, size_t i)
__flatbuffers_offset_vec_at(MyGame_Sample_Monster_table_t, vec, i, 0)
__flatbuffers_table_as_root(MyGame_Sample_Monster)

__flatbuffers_define_struct_field(0, MyGame_Sample_Monster, pos, MyGame_Sample_Vec3_struct_t, 0)
__flatbuffers_define_scalar_field(1, MyGame_Sample_Monster, mana, flatbuffers_int16, int16_t, INT16_C(150))
__flatbuffers_define_scalar_field(2, MyGame_Sample_Monster, hp, flatbuffers_int16, int16_t, INT16_C(100))
__flatbuffers_define_string_field(3, MyGame_Sample_Monster, name, 0)
/* Skipping deprecated field: 'MyGame_Sample_Monster_friendly' */

__flatbuffers_define_vector_field(5, MyGame_Sample_Monster, inventory, flatbuffers_uint8_vec_t, 0)
__flatbuffers_define_scalar_field(6, MyGame_Sample_Monster, color, MyGame_Sample_Color, MyGame_Sample_Color_enum_t, INT8_C(2))
__flatbuffers_define_vector_field(7, MyGame_Sample_Monster, weapons, MyGame_Sample_Weapon_vec_t, 0)
__flatbuffers_define_union_field(flatbuffers_, 9, MyGame_Sample_Monster, equipped, MyGame_Sample_Equipment, 0)

struct MyGame_Sample_Weapon_table { uint8_t unused__; };

static inline size_t MyGame_Sample_Weapon_vec_len(MyGame_Sample_Weapon_vec_t vec)
__flatbuffers_vec_len(vec)
static inline MyGame_Sample_Weapon_table_t MyGame_Sample_Weapon_vec_at(MyGame_Sample_Weapon_vec_t vec, size_t i)
__flatbuffers_offset_vec_at(MyGame_Sample_Weapon_table_t, vec, i, 0)
__flatbuffers_table_as_root(MyGame_Sample_Weapon)

__flatbuffers_define_string_field(0, MyGame_Sample_Weapon, name, 0)
__flatbuffers_define_scalar_field(1, MyGame_Sample_Weapon, damage, flatbuffers_int16, int16_t, INT16_C(0))


#include "flatcc/flatcc_epilogue.h"
#endif /* MONSTER_READER_H */
