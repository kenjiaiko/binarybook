这里的源代码是在下面这本书中使用的：

0と1のコンピュータ世界　バイナリで遊ぼう！（0和1的计算机世界 玩转二进制！）
https://book.mynavi.jp/ec/products/detail/id=24980
http://www.amazon.co.jp/dp/4839951020

这些代码用于自动生成汇编短歌。

编译时需要依赖 Udis86。
http://udis86.sourceforge.net/

-----
gcc mt0.c -o mt0 -ludis86
gcc mt1.c -o mt1 -ludis86
gcc mt2.c -o mt2
gcc mt3.c -o mt3 -ludis86
gcc mt4.c -o mt4 -ludis86
gcc mt5.c -o mt5 -ludis86
gcc tanka.s -o tanka
-----

关于汇编短歌，请参见以下网页：
http://kozos.jp/asm-tanka/
