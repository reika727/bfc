#include <stdlib.h>

/* 確保されている領域の先頭へのポインタ */
char *alloc_ptr;
/* 確保されている領域のサイズ */
u_int64_t sz;
/* Brainfuckのインストラクションポインタ */
char *bf_ptr;

static int64_t bfc_abs(int64_t n)
{
    return n >= 0 ? n : -n;
}

static void bfc_memcpy(void *dst, const void *src, u_int64_t n)
{
    u_int64_t i;
    for (i = 0; i < n; ++i) {
        *(char*)dst = *(char*)src;
        dst = (void*)((char*)dst + 1);
        src = (void*)((char*)src + 1);
    }
}

static void bfc_memset(void *buf, char ch, u_int64_t n)
{
    u_int64_t i;
    for (i = 0; i < n; ++i) {
        *(char*)buf = ch;
        buf = (void*)((char*)buf + 1);
    }
}

/* 初期化 */
void bfc_initialize(void)
{
    /* Brainfuckは最低でも30000バイトのゼロ埋めされた領域を持ち、
     * インストラクションポインタは初期状態で領域の先頭を指す[要出典] */
    bf_ptr = alloc_ptr = (char*)malloc(sz = 30000);
    bfc_memset(alloc_ptr, '\0', 30000);
}

/* 終了処理 */
void bfc_finalize(void)
{
    free(alloc_ptr);
}

/* 領域の再確保 */
void bfc_realloc(void)
{
    /* インストラクションポインタが領域をはみ出した場合、
     * そのはみ出た分の2倍の領域を足し合わせて再確保する */
    const int64_t offset = bf_ptr - alloc_ptr;
    if (0 <= offset && (u_int64_t)offset < sz) {
        return;
    }
    const u_int64_t margin = (offset < 0 ? bfc_abs(offset) : offset - sz + 1) * 2;
    char*const new_mem = (char*)malloc(sz + margin);
    bfc_memcpy(new_mem + (offset < 0 ? margin : 0), alloc_ptr, sz);
    bfc_memset(new_mem + (offset < 0 ? 0 : sz), '\0', margin);
    free(alloc_ptr);
    alloc_ptr = new_mem;
    sz += margin;
    bf_ptr = alloc_ptr + (offset < 0 ? margin : 0) + offset;
}
