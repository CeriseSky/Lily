#ifndef _VECTOR_H_
#define _VECTOR_H_

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Did I need to implement vectors from scratch in C? No...
// Did I do it anyway? Yes!

// usage is pretty simple, in a header file you can define prototype
// functions for vec_(type) with VEC_PROTO(type)
// After that, if you are confident that no other source file will try to
// create the implementations, use VEC_IMPL(type) directly. If not, define
// VEC_STATIC_IMPL before including this file. Then using it in source is a
// matter of:
//   vec_type myVec;
//   vec_type_new(&myVec);
//   type value = /* ... */;
//   vec_type_push(&myVec, &value);

// getting the length of the vector is done by directly accessing the len
// field of the struct; likewise, accessing elements is done by vec.storage[i]
// This could cause issues for multithreading but I'm
// not going to change it unless that becomes an issue for Lily's code.
// This is a chess engine, not a feature-complete C vector library.

// the following vectors are implemented by default (this can be disabled
// by defining VEC_HEADER_ONLY before including this file):
// - vec_charptr

#ifdef VEC_STATIC_IMPL
#define VEC_API static
#else
#define VEC_API
#endif

// if creating prototypes for a pointer type,
// create a typedef like "typedef char *charptr" and then
// VEC_PROTO(charptr)
#define VEC_PROTO(T) \
  VEC_API typedef struct {\
    size_t len; \
    size_t cap; \
    T *storage; \
    size_t reallocs; \
  } vec_##T; \
  \
  /* new is the "constructor" of the vector.
   * If the vector is already constructed, it is deleted and reconstructed */\
  VEC_API void vec_##T##_new(vec_##T *); \
  \
  /* push accepts a pointer to T, which is copied into the vec.
   * This is to allow for vecs of structure types.
   * Returns the new length of the vector on success; -1 otherwise */ \
  VEC_API long vec_##T##_push(vec_##T *, T *); \
  \
  /* returns the new capacity of the vector on success; -1 otherwise */ \
  VEC_API long vec_##T##_reserve(vec_##T *, size_t); \
  \
  /* Reallocates the buffer if it is not large enough */ \
  VEC_API long vec##T##_resize(vec_##T *, size_t); \
  /* frees the underlying storage buffer. vec_new can be called after on the
   * same vector. */ \
  VEC_API void vec_##T##_delete(vec_##T *);

#define VEC_IMPL(T) \
  VEC_API void vec_##T##_new(vec_##T *self) { \
    self->len = 0; \
    self->cap = 0; \
    self->storage = nullptr; \
    self->reallocs = 0; \
  } \
  \
  VEC_API long vec_##T##_push(vec_##T *self, T *val) { \
    if(self->cap < ++self->len) {\
      if(vec_##T##_reserve(self, self->cap > 0 ? self->cap * 3/2 : 16) < 0) \
        return -1; \
      self->reallocs++; \
    }\
    memcpy(&self->storage[self->len - 1], val, sizeof(T)); \
    return self->len; \
  } \
  \
  VEC_API long vec_##T##_reserve(vec_##T *self, size_t cap) { \
    if(self->cap < cap) { \
      self->cap = cap; \
      self->storage = realloc(self->storage, cap * sizeof(T)); \
    } \
    return self->storage ? self->cap : -1; \
  } \
  \
  VEC_API long vec_##T##_resize(vec_##T *self, size_t len) { \
    if(self->cap < len) \
      if(vec_##T##_reserve(self, len) < 0) \
        return -1; \
    self->len = len; \
    return self->len; \
  } \
  \
  VEC_API void vec_##T##_delete(vec_##T *self) { \
    free(self->storage); \
    self->storage = nullptr; \
    self->len = 0; \
    self->cap = 0; \
  }

#ifndef VEC_HEADER_ONLY
typedef char *charptr;
VEC_PROTO(charptr)
#endif

#endif

