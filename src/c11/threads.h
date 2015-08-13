#ifndef GL3DS_THREADS
#define GL3DS_THREADS

#include <3ds.h>

typedef Handle mtx_t;

// TODO: real mutexes lol
#define mtx_lock(X)
#define mtx_unlock(X)
#define mtx_init(X, Y)
#define mtx_destroy(X)
#define mtx_plain

#endif
