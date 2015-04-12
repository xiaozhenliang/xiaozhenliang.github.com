---
layout: post
title: "谷歌域名黑名单批量检测"
description: "谷歌域名黑名单批量检测"
category: scripts
tags: ["ruby"]
---

因海外Cpanel主机上有大量google黑名单域名机房要求处理，使用ll -alh /usr/local/apache/conf/sites | awk '{print $9}' | perl -p -i -e 's/\.conf//g' >~/domainlist.txt批量导出域名列表作为参数输入到脚本即可。

<pre>
#!/usr/bin/env ruby
#encoding:utf-8
require 'uri'
require 'net/http'
require 'open-uri'

def help()
	print 
"
==========================================================
Google Website Status Checking Tool Version 0.Any
1 Question Please Mail To Xiaozhl@35.cn
Usege: GoogleSiteStatus.rb SourceDomainList OutputFilename
==========================================================
"
	exit
end

def checkcode(name)
	begin
		concat_name = "http:\/\/safebrowsing.clients.google.com\/safe
		browsing\/diagnostic?client=Firefox&hl=en-US&site=http:\/\/#{name}"
		open("#{concat_name}",
			 "User-Agent" =>"Mozilla/5.0 (Windows; U; Windows NT 6.1; 
			 zh-CN; rv:1.9.2.15) Gecko/20110303 Firefox/3.6.15",
			) { |f|
			res = f.read
			suspicious_checking = !res.index("Site is listed as suspicious").nil?
			malicious_checking = !res.index("Malicious software includes").nil?
			res = ''
			if suspicious_checking or malicious_checking
				status = "bad"
			else
				status = "good"
			end
			return "#{status},#{suspicious_checking},#{malicious_checking}"
			}
	rescue Exception => ex
		p ex
		return "000"
	end
end

def response()
	print "\nJob Starting...\n\n"  
	File.foreach($*[0]){ |line|
		line = line.chomp
		return_code = checkcode(line)
		print line,"'s RESPONSE STRING is ",return_code,"\n"
		check_status = !return_code.index("bad").nil?
		if !check_status
			file_handle = File.new($*[1],"a+")
			file_handle.print(line,"|",return_code,"\n")
			file_handle.close
		end
	}  
	print "\nJob Done.\n"
end

if $0 == __FILE__
  begin
    $*[2].nil? ? response : help
  rescue 
    help
  end
end
</pre>
