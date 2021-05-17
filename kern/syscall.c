/* See COPYRIGHT for copyright information. */

#include <inc/x86.h>
#include <inc/error.h>
#include <inc/string.h>
#include <inc/assert.h>

#include <kern/console.h>
#include <kern/env.h>
#include <kern/kclock.h>
#include <kern/pmap.h>
#include <kern/sched.h>
#include <kern/syscall.h>
#include <kern/trap.h>
#include <kern/traceopt.h>

/* Print a string to the system console.
 * The string is exactly 'len' characters long.
 * Destroys the environment on memory errors. */
static int
sys_cputs(const char *s, size_t len) {
    // LAB 8: Your code here
    user_mem_assert(curenv, s, len, PTE_U);

    /* Check that the user has permission to read memory [s, s+len).
    * Destroy the environment if not. */
    for (size_t i = 0; i < len; ++i) {
        cputchar(s[i]);
    }

    return 0;
}

/* Read a character from the system console without blocking.
 * Returns the character, or 0 if there is no input waiting. */
static int
sys_cgetc(void) {
    // LAB 8: Your code here

    return cons_getc();
}

/* Returns the current environment's envid. */
static envid_t
sys_getenvid(void) {
    // LAB 8: Your code here
    return curenv->env_id;
}

/* Destroy a given environment (possibly the currently running environment).
 *
 *  Returns 0 on success, < 0 on error.  Errors are:
 *  -E_BAD_ENV if environment envid doesn't currently exist,
 *      or the caller doesn't have permission to change envid.
 */
static int
sys_env_destroy(envid_t envid) {
    // LAB 8: Your code here.
    int res = 0;
    struct Env *env;
    res = envid2env(envid, &env, 1);
    if (res < 0) {
        return -E_BAD_ENV;
    }

#if 1 /* TIP: Use this snippet to log required for passing grade tests info */
    if (trace_envs) {
        cprintf(env == curenv ?
                        "[%08x] exiting gracefully\n" :
                        "[%08x] destroying %08x\n",
                curenv->env_id, env->env_id);
    }
#endif

    env_destroy(env);
    return 0;
}

/* Dispatches to the correct kernel function, passing the arguments. */
uintptr_t
syscall(uintptr_t syscallno, uintptr_t a1, uintptr_t a2, uintptr_t a3, uintptr_t a4, uintptr_t a5, uintptr_t a6) {
    /* Call the function corresponding to the 'syscallno' parameter.
     * Return any appropriate return value. */

    // LAB 8: Your code here
    if (syscallno == SYS_cputs) {
        return sys_cputs((const char *)a1, (size_t)a2);
    } else if (syscallno == SYS_cgetc) {
        return sys_cgetc();
    } else if (syscallno == SYS_getenvid) {
        return sys_getenvid();
    } else if (syscallno == SYS_env_destroy) {
        return sys_env_destroy((envid_t)a1);
    }
    return -E_NO_SYS;
}
