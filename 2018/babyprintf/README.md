本意是想让选手绕printf_chk，但没限制好（orz

思路大概和phrack的文章差不多，在printf的时候覆盖file结构的flag2，只不过他用的是printf自身的洞，这边用stdout的缓存去覆盖

exp 用printf_chk leak + exit handle getshell，可能麻烦了点。

有师傅用%a来leak，很强（这是真没想到，这样理论上不需要code段地址

最后控制rip使用 printf触发malloc 也是比较简单的方法
