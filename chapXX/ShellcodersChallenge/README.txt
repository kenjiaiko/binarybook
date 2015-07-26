
■ShellcodersChallenge

这是安全竞赛 SECCON2013 中所使用的赛题源代码。
已在 Ubuntu12.04/x86 + Apache2 环境下确认可用。

关于 SECCON2013 请访问 http://2013.seccon.jp/
使用本代码的是 SECCON关西（大阪）赛区
http://2013.seccon.jp/blog/2013/12/seccon-2016.html
详情请参见上述网页。

这是一个以“在浏览器上执行x86指令”为目标而编写的一个技术试验品。

下面是其原理的讲解幻灯片（沙箱方式）。
http://www.slideshare.net/KenjiAiko/on-web-30208130

这个程序本身是有漏洞的，因此请大家在私有环境下进行尝试。

其中的加密部分使用了 PyCrypt：
https://www.dlitz.net/software/pycrypto/

其中的验证码使用 Captchator 服务：
http://captchator.com/
