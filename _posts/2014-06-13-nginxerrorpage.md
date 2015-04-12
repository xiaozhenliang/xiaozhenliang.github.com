---
layout: post
title: "正确设置Nginx404错误页面"
description: "正确设置Nginx404错误页面"
category: webserver
tags: ["nginx"]
---

error_page指令可以让nginx响应指定的错误页面给客户端。

###网上流传通用配置

* http配置块加入：fastcgi_intercept_errors on;
* server配置块加入: error_page 404 = /404.html;
* 测试nginx配置并重启：nginx -t && nginx -s reload

###是我们需要的吗？

确实经过以上三步后测试一个不存在URL返回了正确的404页面。但是Spiders & Bots抓到的却不是404。我们使用curl -I来看究竟发生了什么。

<pre>
[root@localhost ~]# curl -I http://www.testwebsite.com/404notfound
HTTP/1.1 200 OK
Server: nginx
Date: Tue, 17 Jun 2014 00:51:22 GMT
Content-Type: text/html
Content-Length: 2883
Last-Modified: Tue, 10 Jun 2014 02:21:34 GMT
Connection: keep-alive
Vary: Accept-Encoding
ETag: "53966bae-b43"
Accept-Ranges: bytes
</pre>

很神奇对不对？明明返回了正常的404页面但是HTTP RESPONSE CODE确是200。如果一个站点里面死链过多导致访问404却错误的返回了200的结果给蜘蛛，将会极大地影响该站的SEO。

###问题在哪里？

根据 [官方在线文档](http://nginx.org/en/docs/http/ngx_http_core_module.html#error_page) 显示error_page指令的正确语法是：

<pre>
error_page code ... [=[response]] uri;
error_page 404             /404.html;
error_page 500 502 503 504 /50x.html;
</pre>

不是error_page 404 = /404.html而是error_page 404 /404.html，我们把配置做对应的修改后得到如下结果：

<pre>
[root@localhost ~]# curl -I http://www.testwebsite.com/404notfound
HTTP/1.1 404 Not Found
Server: nginx
Date: Tue, 17 Jun 2014 00:50:16 GMT
Content-Type: text/html
Content-Length: 2883
Connection: keep-alive
Vary: Accept-Encoding
ETag: "53966bae-b43"
</pre>

###等号是干啥的？

nginx中利用=号来强制指定HTTP RESPONSE CODE，比如一个404页面要让他返回301页面应该是：

<pre>
error_page 404 =301 http://www.testwebsite.com/notfound.html;
</pre>

这样404错误就会被301取代并返回http://www.testwebsite.com/notfound.html

###真相大白了...

在有等号的时候，nginx启用的是强制指定HTTP RESPONSE CODE功能，等号后面为空时默认返回为200，即按网上流传配置方式等价于：

<pre>
error_page 404 =200 /404.html;
</pre>
