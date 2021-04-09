/* -- eh_shmem3_mingw.c --
*
* See libgcc/eh_shmem3.h for a description of __EH_SHMEM.
*
* This is the MinGW/MinGW64 implementation of __EH_SHMEM, based in part on a
* mechanism originally developed by Thomas Pfaff and Adriano dos Santos
* Fernandes, reimplemented by J.M. Eubank as of 2021 with suggestions from
* Ralph Engels.
*
* This code is released into the public domain without warranty; it may be
* freely used and redistributed.
*/

#include "eh_shmem3.h"

#if USE_SHMEM3

#define WIN32_LEAN_AND_MEAN
/* #include <malloc.h> */
/* #include <stdio.h> */
#include <stdlib.h> /* For _onexit */
#include <windows.h>


/* This is actually required to break a circular dependency - this code is used
 * by the exception handler, so it can't depend on the exception handler.
 */
#pragma GCC optimize "no-exceptions"


__SHMEM_NAME(region_struct) __SHMEM_NAME(local_shmem_region) = {0};
static char full_atom_name[sizeof(SHMEM_VERSION_PREFIX) + (sizeof(void*) * 8) + 1];
static char mutex_name[sizeof(SHMEM_VERSION_PREFIX) + (sizeof(DWORD) * 2)];


static void shmem3_debugbreak(void)
{
    __asm__ __volatile__("int {$}3":);
}

/* bughunter, creates a breakpoint at the failed function and prints error */
static void __eh_shmem_get_error(const char *msg)
{
    DWORD err_code;
    if (__builtin_expect((err_code = GetLastError()) != NO_ERROR, 0))
    {
        /* maybe print error to screen ? */
        LPSTR err;
        if (FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
         NULL, err_code, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
         (LPSTR)&err, 0, NULL))
        {
            OutputDebugStringA(err); /* or otherwise log it */
            OutputDebugStringA(msg);
            LocalFree(err);
        }
        /* drop a breakpoint if the debugger is present */
        if (IsDebuggerPresent())
            shmem3_debugbreak();
    }
}


static void* __eh_shmem_get_ptr_from_atom(ATOM atom, char* name_buf, size_t name_buf_len, size_t ptr_offset)
{
    size_t ptr_len = sizeof(void*) * 8;
    if ((name_buf_len - ptr_offset - 1) < ptr_len)
    {
        __eh_shmem_get_error("string mismatch in atom\n");
        return 0;
    }
    if (!GetAtomNameA(atom, name_buf, (int)name_buf_len))
    {
        __eh_shmem_get_error("failed to get string from atom\n");
        return 0;
    }
    size_t ptr = 0;
    size_t i = 0;
    for (; i < ptr_len; ++i)
    {
        if (name_buf[ptr_offset + i] == 'A')
            ptr |= ((size_t)1u << (ptr_len - i));
    }
    return (void*)ptr;
}


int __SHMEM_NAME(cleanup_local_region)(void);
int __SHMEM_NAME(cleanup_local_region)(void)
{
    /* This mutex is needed to prevent another __eh_shmem region being added or removed while we are
     * traversing the list.
     */
    HANDLE hmutex = CreateMutexA(0, FALSE, mutex_name);
    if (WaitForSingleObject(hmutex, INFINITE) != WAIT_OBJECT_0)
    {
        __eh_shmem_get_error("failed to to lock cleanup mutex\n");
        CloseHandle(hmutex);
        return 0;
    }

    /* If there is at least one other region still in the list, remove ourselves from it */
    __SHMEM_NAME(region_struct)* runner = __SHMEM_NAME(local_shmem_region).head;
    if (runner->next)
    {
        if (runner == &(__SHMEM_NAME(local_shmem_region)))
        {
            /* We were the head of the list */
            runner->head = runner->next;
        }
        while (runner)
        {
            if (runner->next == &(__SHMEM_NAME(local_shmem_region)))
                runner->next = runner->next->next;
            else if (runner->next)
                runner->next->head = runner->head;
            runner = runner->next;
        }
    }
    /* Otherwise, remove the atom from the table so that the next instance to load will start a new list */
    else
    {
        ATOM atom = FindAtomA(full_atom_name);
        if (atom)
            DeleteAtom(atom);
    }

    ReleaseMutex(hmutex);
    CloseHandle(hmutex);

    return 0;
}

/* Don't depend on libgcc */
static void *memset0(void *dest, int c, size_t count)
{
    char *bytes = (char *)dest;
    while (count--)
        *bytes++ = (char)c;
    return dest;
}
static void *memcpy0(void *dest, const void *src, size_t count)
{
    char *dest8 = (char *)dest;
    const char *src8 = (const char *)src;
    while (count--)
        *dest8++ = *src8++;
    return dest;
}

__SHMEM_NAME(region_struct)* __SHMEM_NAME(init_local_region)(void)
{
    /* Initialize the mutex name as a printable representation of GetCurrentProcessId() + SHMEM_VERSION_PREFIX */
    const DWORD pid = GetCurrentProcessId();
    size_t i = 0;
    for (; i < sizeof(DWORD); ++i)
    {
        mutex_name[i * 2] = (*((const unsigned char*)&pid + i) >> 4) + 0x41;
        mutex_name[i * 2 + 1] = (*((const unsigned char*)&pid + i) & 0xF) + 0x61;
    }
    memcpy0(mutex_name + (sizeof(DWORD) * 2), SHMEM_VERSION_PREFIX, sizeof(SHMEM_VERSION_PREFIX));

    /* Initialize the atom name as SHMEM_VERSION_PREFIX + '-' + a bunch of 'a' */
    const size_t name_len = sizeof(SHMEM_VERSION_PREFIX) - 1;
    const size_t ptr_len = sizeof(void*) * 8;
    memcpy0(full_atom_name, SHMEM_VERSION_PREFIX, name_len);
    full_atom_name[name_len] = '-';
    memset0(full_atom_name + name_len + 1, 'a', ptr_len);
    full_atom_name[name_len + 1 + ptr_len] = 0;

    /* This mutex is needed to prevent another __eh_shmem region being added or removed in the time between
     * the FindAtomA call and when we finish traversing the linked list and adding ourselves to the end of
     * it.
     */
    HANDLE hmutex = CreateMutexA(0, FALSE, mutex_name);
    if (WaitForSingleObject(hmutex, INFINITE) != WAIT_OBJECT_0)
    {
        __eh_shmem_get_error("failed to to lock creation mutex\n");
        CloseHandle(hmutex);
        return 0;
    }

    __SHMEM_NAME(region_struct)* local_addr = &(__SHMEM_NAME(local_shmem_region));

    /* See if a named atom was already created */
    ATOM atom = FindAtomA(full_atom_name);
    if (!atom)
    {
        /* The uppercase/lowercase A's following the prefix represent bits - A for 1 and a for 0. */
        size_t i = 0;
        for (; i < ptr_len; ++i)
        {
            if ((((size_t)local_addr) >> (ptr_len - i)) & 1)
                full_atom_name[name_len + 1 + i] = 'A';
        }
        atom = AddAtomA(full_atom_name);
        if (!atom)
        {
            __eh_shmem_get_error("failed to add string to atom table\n");
            return 0;
        }
        __SHMEM_NAME(local_shmem_region).head = local_addr;
        _onexit(__SHMEM_NAME(cleanup_local_region));
    }
    else
    {
        __SHMEM_NAME(region_struct)* runner = (__SHMEM_NAME(region_struct)*)__eh_shmem_get_ptr_from_atom(atom,
         full_atom_name, name_len + 1 + ptr_len + 1, name_len + 1);
        __SHMEM_NAME(local_shmem_region).head = runner;
        /* Check if we're already part of the linked list; add ourselves if not */
        while (runner->next && runner != local_addr)
            runner = runner->next;
        if (runner != local_addr)
        {
            runner->next = local_addr;
            _onexit(__SHMEM_NAME(cleanup_local_region));
        }
    }

    ReleaseMutex(hmutex);
    CloseHandle(hmutex);

    return __SHMEM_NAME(local_shmem_region).head;
}

#endif /* USE_SHMEM3 */
