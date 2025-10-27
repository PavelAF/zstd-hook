#include <stdlib.h>
#include <dlfcn.h>
#include <sys/param.h>

#define DLL_Export __attribute__((visibility("default")))

typedef enum {
    ZSTD_c_compressionLevel = 100,
    ZSTD_c_nbWorkers = 400,
    ZSTD_c_enableLongDistanceMatching = 160,
    ZSTD_c_strategy    = 107,
    ZSTD_c_windowLog   = 101,
    ZSTD_c_chainLog    = 103,
    ZSTD_c_searchLog   = 104,
    ZSTD_c_overlapLog  = 402,
    ZSTD_c_ldmHashLog  = 161,
    ZSTD_c_ldmMinMatch = 162,
    ZSTD_c_jobSize     = 401,
} ZSTD_cParameter;

enum {
    ZSTD_MAX_CLEVEL = 22,
};

typedef struct ZSTD_CCtx_s ZSTD_CCtx;
typedef ZSTD_CCtx* (*ZSTD_createCCtx_t)(void);
typedef size_t (*ZSTD_CCtx_setParameter_t)(ZSTD_CCtx* cctx, ZSTD_cParameter param, int value);
typedef int (*ZSTD_maxCLevel_t)(void);

static void* zstd_handle;
static ZSTD_createCCtx_t orig_ZSTD_createCCtx;
static ZSTD_CCtx_setParameter_t orig_ZSTD_CCtx_setParameter;
static ZSTD_maxCLevel_t orig_ZSTD_maxCLevel;

void __attribute__((destructor)) fini_zstd_symbols()
{
    if (zstd_handle) {
        orig_ZSTD_createCCtx = NULL;
        orig_ZSTD_CCtx_setParameter = NULL;
        orig_ZSTD_maxCLevel = NULL;
        dlclose(zstd_handle);
    }
}

void __attribute__((constructor)) init_zstd_symbols()
{
    if (zstd_handle)
        return;

    if (!(zstd_handle = dlopen("libzstd.so.1", RTLD_LAZY))) {
        fini_zstd_symbols();
        return;
    }

    orig_ZSTD_createCCtx = (ZSTD_createCCtx_t) dlsym(
            zstd_handle, "ZSTD_createCCtx");

    orig_ZSTD_CCtx_setParameter = (ZSTD_CCtx_setParameter_t) dlsym(
            zstd_handle, "ZSTD_CCtx_setParameter");

    orig_ZSTD_maxCLevel = (ZSTD_maxCLevel_t) dlsym(
            zstd_handle, "ZSTD_maxCLevel");
}

DLL_Export ZSTD_CCtx* ZSTD_createCCtx(void)
{
    if (!orig_ZSTD_createCCtx)
        return NULL;

    ZSTD_CCtx* const cctx = orig_ZSTD_createCCtx();
    if (!cctx || !orig_ZSTD_CCtx_setParameter)
        return cctx;

    const char* v = getenv("ZSTD_CLEVEL");
    if (v && *v) {
        const int max_level = orig_ZSTD_maxCLevel ?
            orig_ZSTD_maxCLevel() : ZSTD_MAX_CLEVEL;

        const int level = MIN(atoi(v), max_level);
        if (level > 0)
            orig_ZSTD_CCtx_setParameter(cctx, ZSTD_c_compressionLevel, level);
    }

    v = getenv("ZSTD_THREADS");
    if (v && *v) {
        const int workers = atoi(v);
        if (workers > 0)
            orig_ZSTD_CCtx_setParameter(cctx, ZSTD_c_nbWorkers, workers);
    }

    v = getenv("ZSTD_LDM");
    if (v && *v) {
        const int ldm = atoi(v);
        if (ldm > 0)
            orig_ZSTD_CCtx_setParameter(cctx, ZSTD_c_enableLongDistanceMatching, 1);
    }

    v = getenv("ZSTD_STRATEGY");
    if (v && *v) {
        const int strategy = atoi(v);
        if (strategy > 0 && strategy < 10)
            orig_ZSTD_CCtx_setParameter(cctx, ZSTD_c_strategy, strategy);
    }

    return cctx;
}
// vim: set et ts=4 sts=4 sw=4:
