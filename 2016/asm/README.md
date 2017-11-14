# hctf2016-asm
hctf2016-Pwn asm

asm的解释器

附带字节码生成

好像大家都是先leak出libc，再做的，但其实并不用leak，替换strcmp@plt 为 malloc@plt即可call system
