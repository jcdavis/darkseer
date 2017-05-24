.global _Java_is_jcdav_darkseer_DarkSeer_start
.global _Java_is_jcdav_darkseer_DarkSeer_end

# The JNI function definitions that will be called.
# These just put the thread pointer(r15) into the expected parameter register.

_Java_is_jcdav_darkseer_DarkSeer_start:
    pushq %rbp
    movq %rsp, %rbp
    #rdi env
    #rsi klass
    movq %r15, %rdx
    callq _real_start
    popq %rbp
    retq

_Java_is_jcdav_darkseer_DarkSeer_end:
    pushq %rbp
    movq %rsp, %rbp
    #rdi env
    #rsi klass
    #rdx printLevel
    movq %r15, %rcx
    callq _real_end
    popq %rbp
    retq
