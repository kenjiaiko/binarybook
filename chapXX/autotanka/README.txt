ここにあるソースコードは

0と1のコンピュータ世界　バイナリで遊ぼう！
https://book.mynavi.jp/ec/products/detail/id=24980
http://www.amazon.co.jp/dp/4839951020

の書籍内にて使用されたもので
アセンブラ短歌の自動生成についてのコードとなります。

コンパイルには Udis86 が必要です。
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

アセンブラ短歌については
http://kozos.jp/asm-tanka/
こちらをご参照ください。
