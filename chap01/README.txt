
■sample_malについて

sample_malは以下の3つの動作を行います。

・ファイル系
　－スタートアップフォルダ以下に 0.exe というファイルを作成
　－マイドキュメントフォルダ以下に 1.exe というファイルを作成
・レジストリ系
　－Software\\Microsoft\\Windows\\CurrentVersion\\Run\sample_malを作成

chap01/sample_mal/sample_mal/sample_mal.cpp の 226行目以降が該当処理です。

実行後の環境の戻し方は書籍にも書かれてありますが、次の3つを確認してください。

・ファイル系
　－スタートアップフォルダから 0.exe を削除
　－マイドキュメントフォルダから 1.exe を削除
・レジストリ系
　－Software\\Microsoft\\Windows\\CurrentVersion\\Run\sample_malを削除

■スタートアップフォルダの場所

Windows7 or Vista
C:\Users\（ユーザー名）\AppData\Roaming\
  Microsoft\Windows\Start Menu\Programs\Startup

WindowsXP
C:\Documents and Settings\（ユーザー名）\スタート メニュー\
  プログラム\スタートアップ
