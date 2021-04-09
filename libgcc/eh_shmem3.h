/* -- eh_shmem3.h --
 *
 * The __EH_SHMEM mechanism is for sharing named pointers among the instances of a
 * static library compiled into separate modules (binaries or shared libraries)
 * in one runtime program. It's used in libgcc and libstdc++ to be able to
 * propagate exceptions out of shared libraries even when libgcc and libstdc++
 * are compiled in statically.
 *
 * This code is released into the public domain without warranty; it may be
 * freely used and redistributed.
 */


#ifndef __EH_SHMEM3_H_INCLUDED
#define __EH_SHMEM3_H_INCLUDED

#if (defined(_WIN32) || defined(_WIN64)) && !defined(SHARED) && !defined(DLL_EXPORT) && !defined(USE_SHMEM3)
#define USE_SHMEM3 1
#elif !defined (USE_SHMEM3)
#define USE_SHMEM3 0
#endif

#if USE_SHMEM3


#ifdef __cplusplus
#define __SHMEM_CLINK extern "C"
#else
#define __SHMEM_CLINK
#endif


#define __CONCAT2_INDIR(a, b) a ## b
#define __SHMEM_CONCAT2(a, b) __CONCAT2_INDIR(a, b)


#define __SHMEM_NAME(name) __SHMEM_CONCAT2(__eh_shmem3_gcc_tdm_, name)
#define SHMEM_VERSION_PREFIX "__eh_shmem3_gcc_tdm_"


#define __SHMEM_GET_NAMED_PTR(name) \
 ( \
 __SHMEM_NAME(local_shmem_region).head \
 ? \
 __SHMEM_NAME(local_shmem_region).head->name \
 : \
 __SHMEM_NAME(init_local_region)()->name \
 )


#define __SHMEM_DEFINE(type, name) \
 type __SHMEM_CONCAT2(__shmem_, name); \
 __SHMEM_CLINK type* __SHMEM_CONCAT2(__shmem_init_, name)(void); \
 __SHMEM_CLINK type* __SHMEM_CONCAT2(__shmem_init_, name)(void) \
 { \
   __SHMEM_NAME(local_shmem_region).head->name = &__SHMEM_CONCAT2(__shmem_, name); \
   return &__SHMEM_CONCAT2(__shmem_, name); \
 }

#define __SHMEM_DEFINE_INIT(type, name, initval) \
 type __SHMEM_CONCAT2(__shmem_, name) = initval; \
 __SHMEM_CLINK type* __SHMEM_CONCAT2(__shmem_init_, name)(void); \
 __SHMEM_CLINK type* __SHMEM_CONCAT2(__shmem_init_, name)(void) \
 { \
   __SHMEM_NAME(local_shmem_region).head->name = &__SHMEM_CONCAT2(__shmem_, name); \
   return &__SHMEM_CONCAT2(__shmem_, name); \
 }

#define __SHMEM_DEFINE_ARRAY(type, name, size) \
 type __SHMEM_CONCAT2(__shmem_, name)[size]; \
 __SHMEM_CLINK type* __SHMEM_CONCAT2(__shmem_init_, name)(void); \
 __SHMEM_CLINK type* __SHMEM_CONCAT2(__shmem_init_, name)(void) \
 { \
   __SHMEM_NAME(local_shmem_region).head->name = __SHMEM_CONCAT2(__shmem_, name); \
   return __SHMEM_CONCAT2(__shmem_, name); \
 }


#define __SHMEM_DECLARE(type, name) \
 __SHMEM_CLINK type* __SHMEM_CONCAT2(__shmem_init_, name)(void);


#define __SHMEM_GET(type, name) \
 (*(type*)( \
 __SHMEM_GET_NAMED_PTR(name) \
 ? \
 __SHMEM_GET_NAMED_PTR(name) \
 : \
 __SHMEM_CONCAT2(__shmem_init_, name)() \
 ))

#define __SHMEM_GET_ARRAY(type, name) \
 ((type*)( \
 __SHMEM_GET_NAMED_PTR(name) \
 ? \
 __SHMEM_GET_NAMED_PTR(name) \
 : \
 __SHMEM_CONCAT2(__shmem_init_, name)() \
 ))


#ifdef __cplusplus
extern "C" {
#endif

struct __SHMEM_NAME(region_struct_name)
{
    struct __SHMEM_NAME(region_struct_name)* head;
    struct __SHMEM_NAME(region_struct_name)* next;
    void* unwind_dw2_dwarf_reg_size_table;
    void* unwind_dw2_fde_object_mutex;
    void* unwind_dw2_fde_seen_objects;
    void* unwind_dw2_fde_unseen_objects;
    void* unwind_dw2_fde_marker;
    void* unwind_dw2_fde_dw2_once;
    void* unwind_dw2_once_regsizes;
    void* unwind_sjlj_fc_static;
    void* unwind_sjlj_fc_key;
    void* unwind_sjlj_use_fc_key;
    void* unwind_sjlj_once;
    void* eh_globals_eh_globals;
    void* eh_globals_init;
    void* eh_term_handler__terminate_handler;
    void* eh_unex_handler__unexpected_handler;
};

typedef struct __SHMEM_NAME(region_struct_name) __SHMEM_NAME(region_struct);

extern __SHMEM_NAME(region_struct) __SHMEM_NAME(local_shmem_region);

__SHMEM_CLINK __SHMEM_NAME(region_struct)* __SHMEM_NAME(init_local_region)(void);

#ifdef __cplusplus
}
#endif


#else /* ! USE_SHMEM3 */


#define __SHMEM_DEFINE(type, name) type name;
#define __SHMEM_DEFINE_INIT(type, name, initval) type name = initval;
#define __SHMEM_DEFINE_ARRAY(type, name, size) type name[size];
#define __SHMEM_DECLARE(type, name) extern type name;
#define __SHMEM_GET(type, name) name
#define __SHMEM_GET_ARRAY(type, name) name


#endif /* ! USE_SHMEM3 */

#endif /* ! __EH_SHMEM3_H_INCLUDED */
