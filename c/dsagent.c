#include <assert.h>
#include <jvmti.h>
#include <stdio.h>

jvmtiEnv* jvmti;
char* start = 0;
short first = 1;

JNIEXPORT void JNICALL Java_is_jcdav_darkseer_DarkSeer_start(JNIEnv *env, jclass klass) {
  char* thread_ptr;
  asm("movq %%r15, %0;":"=r"(thread_ptr)::);
  start = *(char**)(thread_ptr + 0x60);
}

JNIEXPORT void JNICALL Java_is_jcdav_darkseer_DarkSeer_end(JNIEnv *env, jclass klass) {
  //To avoid printing class init-related allocations from the static init, skip printing
  if (first) {
    first = 0;
    return;
  }
  char* thread_ptr;
  asm("movq %%r15, %0;":"=r"(thread_ptr)::);
  char* end = *(char**)(thread_ptr + 0x60);
  long allocated = (long)end - (long)start;
  printf("%d\n", allocated);

  char* current = start;
  while (end > current) {
    jclass objKlass = (*env)->GetObjectClass(env, (jobject)&current);
    jlong size = -1;
    (*jvmti)->GetObjectSize(jvmti, (jobject)&current, &size);

    char* signature;
    char* generic_signature;
    (*jvmti)->GetClassSignature(jvmti, objKlass, &signature,
      &generic_signature);
    printf("%s: %d\n", signature, size);
    (*jvmti)->Deallocate(jvmti, signature);
    (*jvmti)->Deallocate(jvmti, generic_signature);
    assert(size > 0 && size % 8 == 0);
    current += size;
  }
}

JNIEXPORT jint JNICALL Agent_OnLoad(JavaVM *vm, char *options, void *reserved) {
  return (*vm)->GetEnv(vm, (void**)&jvmti, JVMTI_VERSION_1_0);
}
