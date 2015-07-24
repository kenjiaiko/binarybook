
■关于sample_mal

sample_mal会执行下面三个操作：

・文件
　−在启动文件夹中创建 0.exe 文件
　−在我的文档文件夹中创建 1.exe 文件
・注册表
　−创建Software\\Microsoft\\Windows\\CurrentVersion\\Run\sample_mal

chap01/sample_mal/sample_mal/sample_mal.cpp 中第226行对应该操作。

运行这个程序后的恢复方法在书中也有介绍，主要是下面三个步骤：

・文件
　−删除启动文件夹中的 0.exe 文件
　−删除我的文档文件夹中的 1.exe 文件
・注册表
　−删除Software\\Microsoft\\Windows\\CurrentVersion\\Run\sample_mal

■启动文件夹的位置

Windows7 or Vista
C:\Users\（用户名）\AppData\Roaming\
  Microsoft\Windows\Start Menu\Programs\Startup

WindowsXP
C:\Documents and Settings\（用户名）\「开始」菜单\
  程序\启动
