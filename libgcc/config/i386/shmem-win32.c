/* -- shmem-win32.c --
 *
 * See gcc/shmem.h for a description of __SHMEM.
 *
 * This is the win32 implementation of __SHMEM, based in part on a mechanism
 * originally developed by Thomas Pfaff and Adriano dos Santos Fernandes,
 * reimplemented by JohnE as of 2010.
 *
 * This code is released into the public domain without warranty; it may be
 * freely used and redistributed.
 */


#include "shmem.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <malloc.h>


static const char* shmem_version_prefix = "gcc-shmem-tdm2";


static void __w32sp_trap(void)
{
	asm("int $0x3");
}


static void* get_ptr_from_atom(ATOM atom, char* name_buf, int name_buf_len, int ptr_offset)
{
	int ptr_len = sizeof(void*) * 8;
	if ((name_buf_len - ptr_offset - 1) < ptr_len)
		__w32sp_trap();
	if (!GetAtomNameA(atom, name_buf, name_buf_len))
		__w32sp_trap();
	size_t ptr = 0;
	int i = 0;
	for (; i < ptr_len; ++i)
	{
		if (name_buf[ptr_offset + i] == 'A')
			ptr |= (1 << (ptr_len - i));
	}
	return (void*)ptr;
}


void* __shmem_grab(const char* name, int size, void (*initfunc)(void*))
{
	int prefix_len = strlen(shmem_version_prefix);
	int name_len = strlen(name);
	int ptr_len = sizeof(void*) * 8;

	char full_atom_name[prefix_len + 1 + name_len + 1 + ptr_len + 2];

	memcpy(full_atom_name, shmem_version_prefix, prefix_len);
	full_atom_name[prefix_len] = '-';
	memcpy(full_atom_name + prefix_len + 1, name, name_len);
	memset(full_atom_name + prefix_len + 1 + name_len + 1, 'a', ptr_len);
	full_atom_name[prefix_len + 1 + name_len + 1 + ptr_len] = 0;

	full_atom_name[prefix_len + 1 + name_len] = 0;
	HANDLE hmutex = CreateMutexA(0, FALSE, full_atom_name);
	full_atom_name[prefix_len + 1 + name_len] = '-';
	if (WaitForSingleObject(hmutex, INFINITE) != WAIT_OBJECT_0)
		__w32sp_trap();

	ATOM atom = FindAtomA(full_atom_name);

	void* ret = 0;

	if (atom)
	{
		ret = get_ptr_from_atom(atom, full_atom_name,
		 prefix_len + 1 + name_len + 1 + ptr_len + 1, prefix_len + 1 + name_len + 1);
	}
	else
	{
		void* shared_mem = malloc(size);

		int i = 0;
		for (; i < ptr_len; ++i)
		{
			if ((((size_t)shared_mem) >> (ptr_len - i)) & 1)
				full_atom_name[prefix_len + 1 + name_len + 1 + i] = 'A';
		}

		atom = AddAtomA(full_atom_name);
		if (!atom)
			__w32sp_trap();

		ret = get_ptr_from_atom(atom, full_atom_name,
		 prefix_len + 1 + name_len + 1 + ptr_len + 1,
		 prefix_len + 1 + name_len + 1);
		if (ret == shared_mem)
		{
			memset(ret, 0, size);
			if (initfunc)
				initfunc(ret);
		}
		else
			free(shared_mem);
	}

	ReleaseMutex(hmutex);
	CloseHandle(hmutex);

	return ret;
}
