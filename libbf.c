#include <stdlib.h>
#include <string.h>

/* 確保されている領域の先頭へのポインタ */
char *alloc_ptr;
/* 確保されている領域のサイズ */
size_t sz;
/* Brainfuckのインストラクションポインタ */
char *bf_ptr;

/* 初期化 */
void bf_initialize(void)
{
    /* Brainfuckは最低でも30000バイトのゼロ埋めされた領域を持ち、
     * インストラクションポインタは初期状態で領域の先頭を指す[要出典] */
    bf_ptr = alloc_ptr = (char*)calloc(sz = 30000, 1);
}

/* 領域の再確保 */
void bf_realloc(void)
{
    /* インストラクションポインタが領域をはみ出した場合、
     * そのはみ出た分の2倍の領域を足し合わせて再確保する */
    const int offset = bf_ptr - alloc_ptr;
    if (0 <= offset && (size_t)offset < sz) {
        return;
    }
    const unsigned int margin = (offset < 0 ? abs(offset) : offset - sz + 1) * 2;
    char*const new_mem = (char*)malloc(sz + margin);
    memcpy(new_mem + (offset < 0 ? margin : 0), alloc_ptr, sz);
    memset(new_mem + (offset < 0 ? 0 : sz), '\0', margin);
    free(alloc_ptr);
    alloc_ptr = new_mem;
    sz += margin;
    bf_ptr = alloc_ptr + (offset < 0 ? margin : 0) + offset;
}

/* 終了処理 */
void bf_finalize(void)
{
    free(alloc_ptr);
}
