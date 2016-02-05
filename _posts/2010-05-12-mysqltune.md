---
layout: post
title: "Mysql服务器安装后要调整什么？"
description: "安装后的参数调整"
category: database
tags: ["Mysql"]
---


时常被问及Mysql服务器安装完后的参数调整以及如何优化Mysql性能等，其实还是有很大一部分SA在安装完Mysql直接使用默认的my.cnf参数，当然在大多数情况下是没有问题的。想要通过调整参数获得性能上的提升以及资源的合理利用，请关注下面这几个参数：

## key_buffer_size
这个参数对于MyISAM引擎来说是非常重要的，如果你的服务器主要是用MyISAM引擎的，建议将该值设置为内存的30%-40%.很多优化工具是把系统当前的索引总大小计算得出的一个值，需要注意的是该值只缓存索引数据，而MyISAM使用操作系统页面缓存来缓存元数据，所以你要预留出足够的内存空间给OS级缓存，当然这个指不是越大越好，一般如果索引将近1G的时候可以分配4G左右的key_buffer_size，再多就浪费了，如果你的数据库只是少量的表使用到MyISAM可以将该值保持在16-32M，看情况增加该值，一旦索引数据超出你的设定值Mysql将会在磁盘上创建临时表来满足它的需求。

## innodb_buffer_pool_size
这个参数对于InnoDB引擎是相当重要的，跟MyISAM引擎的key_buffer_size相比InnoDB对这个参数可谓敏感的多，即便key_buffer_size设置错了也无伤大雅，MyISAM还是能正常工作，InnoDB可比它难伺候多了，主要是因为InnoDB把数据和索引缓存都放在了Buffer Pool，可以尽可能的大，比如总空余内存的70-80%。Mysql会自动将满足条件的脏页合并写入磁盘，tps会有些许波动。

## innodb_additional_mem_pool_size
这玩意对性能提升不是那么明显，内存实在宽裕的话给个20MB的吧。

## innodb_log_file_size
很重要，redo log文件的大小，建议设置为innodb_buffer_pool_size的1/4，并非越大越好，太大了会导致Mysql崩溃重启的时候恢复时间超级长...我一般设置为64-512M。

## innodb_log_buffer_size
默认是8M,不建议将该值调整的过大，一般数据在这里停留顶多就是1秒钟就被刷到磁盘了，如果事务普遍涉及的数据块较大，可以适当增加该值，一般8-16M。

## innodb_flush_log_at_trx_commit
有三个值0、1、2，默认是1，0是指在事务提交的时候不刷数据到log buffer，1是指强制刷到log buffer同时调用OS的fsync()强制刷到磁盘，2是指只调用write()不调用fsync()，0是不可取的，除非这个表不重要，1和2相当于鱼和熊掌，1能够确保数据库在崩溃的时候事务的完整性，因为log buffer是在内存中的，1是强制fsync()到磁盘上，2只推送到内存由系统自行选择什么时候写入磁盘；所以1对磁盘造成的压力可想而知，在木有SSD或者Raid10的系统上还是磁盘IO和数据完整二者选其一吧。

## table_cache
打开一个表的系统开销是很庞大的，例如MyISAM表需要频繁打开MYI文件并在其头部标识该表正在被使用中，当你的系统有大量的表被同时访问的时候，你一定不希望每次都从磁盘中打开这个表吧？设置table_cache的意义在于尽可能的把表缓存，减少磁盘压力的同时提高系统性能，这个值一般设置为1024或更高，如果设置成1024更高的值时要注意系统的最大打开文件描述符的数量，设置open_files_limit以及系统的ulimit -SHn 65535。

## thread_cache
不多说，一般是一个逻辑CPU让他负责两个线程，cores * 2

## innodb_doublewrite
doublewrite buffer，是innodb表空间在物理磁盘上的一块区域，InnoDB每次同步dirty pages是先从内存同步到doublewrite buffer,再同步到物理磁盘的实际位置，不建议关闭，对系统性能损失不大。

其他诸如query_cache_size,sync_binlog等按默认来就好了。
