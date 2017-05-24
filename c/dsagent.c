#include <assert.h>
#include <inttypes.h>
#include <jvmti.h>
#include <stdio.h>
#include <string.h>

/* Corresponds to start of the JVM's ThreadLocalAlloc class. See
 *  http://hg.openjdk.java.net/jdk8u/jdk8u/hotspot/file/020cb72be8b7/src/share/vm/memory/threadLocalAllocBuffer.hpp
 *  for more
 */
typedef struct {
  void* start;
  void* top;
  void* pf_top;
  void* end;
  size_t desired_size;
  size_t refill_waste_limit;
  size_t allocation_before_last_gc;
  // Static fields are not stored in the class
  unsigned number_of_refills;
  unsigned fast_refill_waste;
  unsigned slow_refill_waste;
  unsigned gc_waste;
  unsigned slow_allocations;
} TLAB;

jvmtiEnv* jvmti;
TLAB start;
short first = 1;

/* In the name of being paranoid, this is a #define so to make sure nothing wierd about calling semantics/
 * inlining might result in r15 being used before this point (however unlikely that is).
 * gcc supports the much nicer register variable pinning, eg
 * register void* thread_ptr asm("r15"),
 * However this is sadly not portable - clang silently ignores (see b50a8f5).
 * Clang also doesn't support -ffixed-<reg>, so we have to just hope for the best.
 */
#define COPY_TLAB(t) {\
  TLAB* __addr;\
  asm("lea 0x58(%%r15), %0;":"=r"(__addr)::);\
  memcpy(t, __addr, sizeof(TLAB));\
}


JNIEXPORT void JNICALL Java_is_jcdav_darkseer_DarkSeer_start(JNIEnv *env, jclass klass) {
  COPY_TLAB(&start);
}

JNIEXPORT void JNICALL Java_is_jcdav_darkseer_DarkSeer_end(JNIEnv *env, jclass klass, jint printLevel) {
  /* printValue will cause objects to be allocated, so must copy over the TLAB state before walking else we
   * will infinite loop into crashing.
   */
  TLAB end;
  COPY_TLAB(&end);

  //To avoid printing class init-related allocations from the static init, skip printing
  if (first) {
    first = 0;
    return;
  }

  if (start.start != end.start || start.end != end.end ||
    start.top > end.top) {
    printf("Detected a change in the TLAB due to a GC event, can't determine allocations\n");
    printf("       TLAB @ start     TLAB @ end\n");
    printf("start  %016" PRIxPTR " %016" PRIxPTR "\n",
      (uintptr_t)start.start, (uintptr_t)end.start);
    printf("top    %016" PRIxPTR " %016" PRIxPTR "\n",
      (uintptr_t)start.top, (uintptr_t)end.top);
    printf("pf_top %016" PRIxPTR " %016" PRIxPTR "\n",
      (uintptr_t)start.pf_top, (uintptr_t)end.pf_top);
    printf("end    %016" PRIxPTR " %016" PRIxPTR "\n",
      (uintptr_t)start.end, (uintptr_t)end.end);
    printf("If this happens regularly you may need to increase -XX:MinTLABSize\n");
    return;
  }
  jmethodID mid = (*env)->GetStaticMethodID(env, klass, "printValue", "(Ljava/lang/Object;I)V");
  if (!mid) {
    printf("I don't know how to JNI. printValues disabled\n");
    printLevel = 0;
  }

  if (start.slow_allocations != end.slow_allocations) {
    printf("Warning: missed non-TLAB allocation(s), likely large.\n");
    printf("This will not be reflected in this output\n");
    printf("start  %d    end %d\n", start.slow_allocations, end.slow_allocations);
  }

  long allocated = (long)end.top - (long)start.top;
  printf("%ld\n", allocated);

  char* current = (char*)start.top;
  while ((char*)end.top > current) {
    jclass objKlass = (*env)->GetObjectClass(env, (jobject)&current);
    jlong size = -1;
    (*jvmti)->GetObjectSize(jvmti, (jobject)&current, &size);

    char* signature;
    char* generic_signature;
    (*jvmti)->GetClassSignature(jvmti, objKlass, &signature,
      &generic_signature);
    printf("%s: %ld\n", signature, size);

    if (printLevel > 0) {
      (*env)->CallStaticVoidMethod(env, klass, mid, (jobject)&current, printLevel);
    }

    (*jvmti)->Deallocate(jvmti, (unsigned char*)signature);
    (*jvmti)->Deallocate(jvmti, (unsigned char*)generic_signature);
    assert(size > 0 && size % 8 == 0); //FIXME: this assert is not portable
    current += size;
  }
}

JNIEXPORT jint JNICALL Agent_OnLoad(JavaVM *vm, char *options, void *reserved) {
  return (*vm)->GetEnv(vm, (void**)&jvmti, JVMTI_VERSION_1_0);
}
