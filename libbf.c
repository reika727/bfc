#include <sys/types.h>

static int64_t bfc_abs(int64_t n)
{
    return n >= 0 ? n : -n;
}

static void bfc_memset(void *buf, char ch, u_int64_t n)
{
    u_int64_t i;
    for (i = 0; i < n; ++i) {
        *(char*)buf = ch;
        buf = (void*)((char*)buf + 1);
    }
}

static void* bfc_sbrk(int64_t inc)
{
    static char *current_brk = 0;
    char *p = current_brk;
    if (!p) {
        asm volatile (
            "mov $12, %%rax\n\t"
            "mov $0, %%rdi\n\t"
            "syscall\n\t"
            "mov %%rax, %0"
            :"=c"(p)
        );
    }
    asm volatile (
        "mov $12, %%rax\n\t"
        "mov %1, %%rdi\n\t"
        "syscall\n\t"
        "mov %%rax, %0"
        :"=c"(current_brk)
        :"d"(p + inc)
    );
    return p;
}

/* 確保されている領域の先頭へのポインタ */
char *alloc_ptr;
/* 確保されている領域のサイズ */
u_int64_t sz;
/* Brainfuckのインストラクションポインタ */
char *bf_ptr;

/* 初期化 */
void bfc_initialize(void)
{
    /* Brainfuckは最低でも30000バイトのゼロ埋めされた領域を持ち、
     * インストラクションポインタは初期状態で領域の先頭を指す[要出典] */
    bf_ptr = alloc_ptr = (char*)bfc_sbrk(0);
    bfc_sbrk(sz = 30000);
    bfc_memset(alloc_ptr, '\0', sz);
}

/* 終了処理 */
void bfc_finalize(void)
{
    bfc_sbrk(-(int64_t)sz);
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
    const u_int64_t margin = (offset >= 0 ? offset - (int64_t)sz + 1 : bfc_abs(offset)) * 2;
    bfc_sbrk(margin);
    if (offset >= 0) {
        bfc_memset(alloc_ptr + sz, '\0', margin);
    } else {
        int64_t i;
        for (i = sz - 1; i >= 0; --i) {
            alloc_ptr[i + margin] = alloc_ptr[i];
        }
        bfc_memset(alloc_ptr, '\0', margin);
        bf_ptr += margin;
    }
    sz += margin;
}
