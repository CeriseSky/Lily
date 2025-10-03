#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <vector.h>

typedef uint8_t u8;
VEC_PROTO(u8)
VEC_IMPL(u8)

int main() {
  vec_u8 myVec;
  vec_u8_new(&myVec);

  clock_t start = clock();

  constexpr size_t limit = 10'000'000'000;
  for(size_t i = 0; i < limit; i++)
    if(vec_u8_push(&myVec, (u8 *)&i) < 0) {
      fprintf(stderr, "Failed to push %luth element\n", i);
      vec_u8_delete(&myVec);
      return EXIT_FAILURE;
    }

  clock_t delta = clock() - start;
  double seconds = (double)delta / (double)CLOCKS_PER_SEC;
  constexpr double nsecs_per_sec = 1'000'000'000.0;

  printf("Info for %lu pushes:\n", limit);
  printf("Total CPU time: %.2fs (%.2fns avg.)\n", seconds,
         seconds * nsecs_per_sec / (double)limit,
         (double)limit / seconds);

  if(myVec.len != limit) {
    fprintf(stderr, "myVec.len != limit (expteced %lu, got %lu)\n",
            limit, myVec.len);
    vec_u8_delete(&myVec);
    return EXIT_FAILURE;
  }

  constexpr double bytesPerMb = 1024.0 * 1024.0;
  printf("Capacity used: %.2f%% (%.2fMB / %.2fMB)\n",
         (double)myVec.len / (double)myVec.cap * 100.0,
         (double)myVec.len / bytesPerMb, (double)myVec.cap / bytesPerMb);

  printf("Reallocations: %lu\n", myVec.reallocs);

  vec_u8_delete(&myVec);
  return EXIT_SUCCESS;
}

