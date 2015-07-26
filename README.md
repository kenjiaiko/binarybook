###有趣的二进制
这里发布了《有趣的二进制》一书中所涉及的程序代码，点击右侧“Download ZIP”可打包下载所有的文件。  
  
出版社网页  
http://gihyo.jp/book/2013/978-4-7741-5918-8

中文版网页  
http://www.ituring.com.cn/book/1500
  
作者博客（里面有一些没能写进书中的内容）  
http://07c00.hatenablog.com/
  
####VisualC++ 2010 Express版的编译方法
扩展名为.sln的文件是VC++的solution文件。如果已经安装好VC++，直接双击就可以打开，然后点击菜单中的 生成⇒生成解决方案，在Release或Debug目录中就会生成可执行文件。  

####第1〜2章示例源代码的编译注意事项
chap01和chap02中的EXE文件都禁用了ASLR。不过项目文件（源代码）中的设置是启用ASLR的，因此，如果大家打开项目文件，然后用源代码直接生成EXE文件，就有可能和书中所描述的状态产生差异。（如果使用预编译的EXE文件则没有问题）
  
如果要生成禁用ASLR的EXE文件，请参见下面这篇博客（日文）：  
http://07c00.hatenablog.com/entry/2013/08/07/033443

【译者注】  
简单来说，就是在 项目属性→链接器→高级→随机基址 中进行设置。
  
####自己编写进程转储工具
第2章中介绍了以Dr. Watson为代表的进程转储工具，大家也可以自己编写一个这样的工具。

编写进程转储工具的方法和源代码可参见下面这篇博客（日文）：  
http://07c00.hatenablog.com/entry/2013/09/07/015519

####关于第3章中的FreeBSD和Ubuntu环境
如果大家需要这两个操作系统，可以从下面的地址下载：
  
FreeBSD-8.3  
http://07c00.com/tmp/FreeBSD_8.3_binbook.zip
  
Ubuntu-12.04  
http://07c00.com/tmp/Ubuntu-12.04_binbook.zip
  
登录用户名和密码请参见书的第3章。Ubuntu中需要先用guest账号登录，然后用执行`sudo su`命令才能切换为root身份。
  
上面两个镜像需要用VMware Player来运行：  
http://www.vmware.com/products/player/
  
####关于3.1最后的专栏
关于3.1最后的专栏“printf类函数的字符串格式化bug”，在下面的博客（日文）中有更详细的说明，有兴趣的话可以读一读。  
  
尝试字符串格式化bug（format bug）  
http://07c00.hatenablog.com/entry/2013/08/10/112404
  
####关于第4章中提到的编写调试器以及API钩子
关于第4章中提到的编写调试器以及API钩子，在Linux系统中的运用可以参考下面两篇博客（日文）：  
http://07c00.hatenablog.com/entry/2013/08/31/142001  
http://07c00.hatenablog.com/entry/2013/09/02/003629
  
####5.1中提到的CVE-2009-0927的分析视频
在本书正文中没有对这个漏洞的攻击进行讲解，在下面这个视频中对这个漏洞进行了简单的分析，有兴趣的话可以看一看。  
  
Introducing of Security Camp 2013 "Software security class"  
http://www.youtube.com/watch?v=eUslcqff6_k
  
####关于5.1中的专栏
“使用msfpayload生成Windows环境的shellcode”这张图片中的文字可能看不清楚，原图在这里：  
![System Console](pics_for_readme/1.png)
  
