# bfc
Brainf\*\*kのコンパイラです。
# ファイルの説明
## libbf.c
ライブラリです。いろいろな変数やら関数やらが定義されています。
## bf_to_asm
Brainf\*\*kソースを受け取り、それをアセンブリ言語(64ビットマシン用、AT&T記法)に翻訳したものを標準出力に出力します。
こんな感じで使えます。
```bash
$ echo ">>+<<" > sample.bf
$ ./bf_to_asm sample.bf
.globl main
main:
push %rbp
mov %rsp, %rbp
call bf_initialize
addq $2, bf_ptr(%rip)
mov alloc_ptr(%rip), %rax
add sz(%rip), %rax
cmp bf_ptr(%rip), %rax
jg .bfc_if_1
call bf_realloc
.bfc_if_1:
mov bf_ptr(%rip), %rax
addq $1, (%rax)
subq $2, bf_ptr(%rip)
mov alloc_ptr(%rip), %rax
cmp bf_ptr(%rip), %rax
jle .bfc_if_2
call bf_realloc
.bfc_if_2:
call bf_finalize
mov $0, %eax
pop %rbp
ret
```
## Makefile
bf_to_asmを使って実行ファイルを作るためのスクリプトの代わりです。
以下のようにするとhoge.bfからhoge.outが作られます。
```bash
$ make hoge.out
```
...ですが、このMakefile内に書かれている動的リンカへのパス(私の環境では/lib64/ld-linux-x86-64.so.2)が人によって違う場合があるかもしれません。
そもそも私以外の人に使ってもらう前提で作ったプログラムではありませんが、どうしても動かしたい場合はMakefileをいい感じに書き換えてください。

Makefile以外は特に書き換える必要はない...と信じたい。
# 使用例
[Branf\*\*kでマンデルブロ集合を描画するソースコード](https://github.com/fabianishere/brainfuck/blob/master/examples/mandelbrot/mandelbrot-tiny.bf)を動かしてみます。
```bash
$ make mandelbrot-tiny.out
$ ./mandelbrot-tiny.out
AAAAAAAABBBBBBBBCCCCCCCCCCCCCCCCCCDDDDEFEEDDDCCCCCBBBBBBBBBBBBBBB
AAAAAAABBBBBBCCCCCCCCCCCCCCCCCDDDDDDEEFIKGGGDDDDDCCCCBBBBBBBBBBBB
AAAAAABBBBCCCCCCCCCCCCCCCCCDDDDDDDEEEFGHKPIGFEDDDDDCCCCCBBBBBBBBB
AAAAABBBCCCCCCCCCCCCCCCCCDDDDDDDEEEFGPVT  Q[HEEEEDDDCCCCCCBBBBBBB
AAAABBCCCCCCCCCCCCCCCCDDDDDDDEEFFFGGHK      HGFFEEEDDDCCCCCBBBBBB
AAABBCCCCCCCCCCCCCCCDDDDDEEEFGK MJJ NR    YS L HHGIJFDDCCCCCCBBBB
AAABCCCCCCCCCCCCCDDDEEEEEEFFFHI                    MGEDDCCCCCCBBB
AABCCCCCCCCCCCDDEEEEEEEEFFFGY Q                   MHGEEDCCCCCCCBB
AACCCCCCDDDDDEEFLHGGHMHGGGHIR                      QLHEDDCCCCCCCB
ABCCDDDDDDEEEEFGIKU    RLJJL                        IFEDDCCCCCCCB
ACDDDDDDEEEEEGGHOS        QR                        JFEDDDCCCCCCC
ADDDDDEFFFGGHKOPS                                   GEEDDDCCCCCCC
A                                                PJGFEEDDDCCCCCCC
ADDDDDEFFFGGHKOPS                                   GEEDDDCCCCCCC
ACDDDDDDEEEEEGGHOS        QR                        JFEDDDCCCCCCC
ABCCDDDDDDEEEEFGIKU    RLJJL                        IFEDDCCCCCCCB
AACCCCCCDDDDDEEFLHGGHMHGGGHIR                      QLHEDDCCCCCCCB
AABCCCCCCCCCCCDDEEEEEEEEFFFGY Q                   MHGEEDCCCCCCCBB
AAABCCCCCCCCCCCCCDDDEEEEEEFFFHI                    MGEDDCCCCCCBBB
AAABBCCCCCCCCCCCCCCCDDDDDEEEFGK MJJ NR    YS L HHGIJFDDCCCCCCBBBB
AAAABBCCCCCCCCCCCCCCCCDDDDDDDEEFFFGGHK      HGFFEEEDDDCCCCCBBBBBB
AAAAABBBCCCCCCCCCCCCCCCCCDDDDDDDEEEFGPVT  Q[HEEEEDDDCCCCCCBBBBBBB
AAAAAABBBBCCCCCCCCCCCCCCCCCDDDDDDDEEEFGHKPIGFEDDDDDCCCCCBBBBBBBBB
AAAAAAABBBBBBCCCCCCCCCCCCCCCCCDDDDDDEEFIKGGGDDDDDCCCCBBBBBBBBBBBB