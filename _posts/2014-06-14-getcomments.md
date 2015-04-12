---
layout: post
title: "批量采集ShopEX评论"
description: "批量采集ShopEX评论"
category: scripts
tags: ["python"]
---

<pre>
#!/usr/bin/env python
#coding:utf8
#采集shopex对应产品评论
#导入所需库并重载文件为utf8编码
import sys
import urllib
import re
if sys.getdefaultencoding() != 'utf-8':
	reload(sys)
	sys.setdefaultencoding('utf-8')

#产品id列表
productids = [8,11,14,16,17,20,21,22,23]
#预编译内容替换正则
srcStr1 = re.compile(' <div>')
srcStr2 = re.compile('</div> ')

#获取指定页面评论内容
def getComments(dest_url):
	content_url = dest_url
	content = urllib.urlopen(content_url).read()
	result = re.findall(" <div>.*?</div> ", content)
	lens = len(result)
	#返回本页评论个数以及内容
	return lens,result

for pid in productids:
	for innerpage in xrange(1,50):
		content_url = "http://www.example.com/comment-%s-discuss-%s-commentlist.html" % (pid,innerpage)
		res = getComments(content_url)
		if res[0] < 15 :
			break
		else:
			for item in res[1]:
				item = srcStr1.sub('',item)
				item = srcStr2.sub('',item)
				print item
</pre>
