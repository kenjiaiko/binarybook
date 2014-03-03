
■ShellcodersChallenge

セキュリティコンテスト SECCON2013 において
使用された問題のソースコード一式です。
Ubuntu12.04/x86 + Apache2 にて動作確認しています。

SECCON2013については http://2013.seccon.jp/ をご参照ください。
使用されたのは SECCON関西（大阪）大会 であり、
http://2013.seccon.jp/blog/2013/12/seccon-2016.html
こちらのページに詳細が載っています。

技術的には「ブラウザ上からx86命令を実行できる環境を作ってみる」を
コンセプトに作ってみたものになります。

以下が動作を解説したスライドです（サンドボックス方式）。
http://www.slideshare.net/KenjiAiko/on-web-30208130

このままだと脆弱性があるプログラムとなっていますので
あくまで自身のみがアクセスできる環境でお試しください。

一部暗号化処理のためにPyCryptを使っています。
https://www.dlitz.net/software/pycrypto/

一部認証のために Captchaサービス を使っています。
http://captchator.com/
