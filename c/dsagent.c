#include <assert.h>
#include <inttypes.h>
#include <jvmti.h>
#include <stdio.h>
#include <string.h>

// Corresopnds to start of the JVM's ThreadLocalAlloc class
typedef struct {
  void* start;
  void* top;
  void* pf_top;
  void* end;
} TLAB;

jvmtiEnv* jvmti;
TLAB start;
short first = 1;

JNIEXPORT void JNICALL Java_is_jcdav_darkseer_DarkSeer_start(JNIEnv *env, jclass klass) {
  TLAB* s;
  asm("lea 0x58(%%r15), %0;":"=r"(s)::);
  memcpy(&start, s, sizeof(TLAB));
}

JNIEXPORT void JNICALL Java_is_jcdav_darkseer_DarkSeer_end(JNIEnv *env, jclass klass) {
  //To avoid printing class init-related allocations from the static init, skip printing
  if (first) {
    first = 0;
    return;
  }
  TLAB* end;
  asm("lea 0x58(%%r15), %0;":"=r"(end)::);
  if (start.start != end->start || start.end != end->end ||
    start.top > end->top) {
    printf("Detected a change in the TLAB due to a GC event, can't determine allocations\n");
    printf("       TLAB @ start     TLAB @ end\n");
    printf("start  %016" PRIxPTR " %016" PRIxPTR "\n",
      (uintptr_t)start.start, (uintptr_t)end->start);
    printf("top    %016" PRIxPTR " %016" PRIxPTR "\n",
      (uintptr_t)start.top, (uintptr_t)end->top);
    printf("pf_top %016" PRIxPTR " %016" PRIxPTR "\n",
      (uintptr_t)start.pf_top, (uintptr_t)end->pf_top);
    printf("end    %016" PRIxPTR " %016" PRIxPTR "\n",
      (uintptr_t)start.end, (uintptr_t)end->end);
    printf("If this happens regularly you may need to increase -XX:MinTLABSize\n");
    return;
  }
  long allocated = (long)end->top - (long)start.top;
  printf("%ld\n", allocated);

  char* current = (char*)start.top;
  while (end->top > current) {
    jclass objKlass = (*env)->GetObjectClass(env, (jobject)&current);
    jlong size = -1;
    (*jvmti)->GetObjectSize(jvmti, (jobject)&current, &size);

    char* signature;
    char* generic_signature;
    (*jvmti)->GetClassSignature(jvmti, objKlass, &signature,
      &generic_signature);
    printf("%s: %ld\n", signature, size);
    (*jvmti)->Deallocate(jvmti, signature);
    (*jvmti)->Deallocate(jvmti, generic_signature);
    assert(size > 0 && size % 8 == 0); //FIXME: this assert is not portable
    current += size;
  }
}

JNIEXPORT jint JNICALL Agent_OnLoad(JavaVM *vm, char *options, void *reserved) {
  return (*vm)->GetEnv(vm, (void**)&jvmti, JVMTI_VERSION_1_0);
}
