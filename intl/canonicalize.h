#ifndef __CANONICALIZE_H__
#define __CANONICALIZE_H__ 1

#ifdef __cplusplus
extern "C" {
#endif

char *canonicalize_file_name (const char *name);

#ifdef __WIN32__
char *win2unixpath (char *path);
#endif

#ifdef __cplusplus
}
#endif

#endif /* __CANONICALIZE_H__ */
