---
layout: post
title: "收缩Mysql的ibdata1文件大小"
description: "收缩mysql的ibdata1文件大小"
category: database
tags: ["Mysql"]
---

如果你有使用InnoDB来存储你的Mysql表，使用默认设置应该会碰到个非常头疼的问题，在Mysql的数据目录里有个默认只有10MB叫ibdata1的文件没日没夜的增长让你烦透了吧？里面包含了InnoDB引擎存储的所有索引和数据信息，很可惜Mysql在设计的时候就没有收缩InnoDB表的功能，这就是为什么你在delete,truncate,drop这些表的时候这个文件大小没有丝毫要减少的原因，而且你还没办法直观的看到哪个数据库占用了大量的ibdata1，这个坑爹的问题在新版的Mysql中已经通过innodb_file_per_table这个选项来解决了，开启该选项后，每个InnoDB表的索引和数据都会按*.ibd命名存储到各个数据库中，但是这个选项默认是不开启的。

如上所说，你没法去收缩InnoDB数据文件，你也没办法在一台没有打开innodb_file_per_table选项的机器上直接加上该选项让他工作，你必须在安装完Mysql就加上这个选项，或者按照本文介绍的三种方式来操作，无论你做任何操作，请确保**你已经备份了整个数据库目录**，也别忘了停掉Mysql相关的一些服务。

前两个方法需要获取到Mysql数据库实例中所有InnoDB的清单，如果你使用的是5.0以上的版本，这一切都很容易：SELECT `TABLE_SCHEMA`,`TABLE_NAME` FROM TABLES WHERE ENGINE = 'InnoDB';


##修改表引擎

1. 对每个InnoDB表执行 ALTER TABLE `table_name` ENGINE=MyISAM;
2. 停止Mysql服务；
3. 移除InnoDB相关文件ibdata1等；
4. 修改my.cnf中的参数，添加innodb_file_per_table；
5. 启动Mysql服务；
6. 将刚才修改后的那些表改回InnoDB：ALTER TABLE `table_name` ENGINE=InnoDB；


##导出InnoDB表

1. 使用mysqldump命令导出所有的InnoDB表，例如:
mysqldump --add-drop-table --extended-insert --disable-keys --quick 'db_name' --tables 'tbl_name' > 'db_name.tbl_name.sql'
2. 删掉这些表：
	* SET FOREIGN_KEY_CHECKS=0;
	* DROP TABLE db_name.tbl_name;
	* DROP TABLE db_name1.tbl_name1;
	* –– DROP other tables here…
	* SET FOREIGN_KEY_CHECKS=1;
3. 停止Mysql服务；
4. 移除InnoDB相关文件ibdata1等；
5. 修改my.cnf中的参数，添加innodb_file_per_table；
6. 启动Mysql服务；
7. 在Mysql Console下导入表：
	* SET FOREIGN_KEY_CHECKS=0;
	* SOURCE db_name.tbl_name.sql;
	* SOURCE db_name1.tbl_name1.sql;
	* –– SOURCE other files here…
	* SET FOREIGN_KEY_CHECKS=1;


##导出整个数据库

这个是我常用的，虽然他和耗磁盘和时间，但是确实是最简便的：

1. 导出所有的数据：
/usr/bin/mysqldump ––extended-insert ––all-databases ––add-drop-database ––disable-keys ––flush-privileges ––quick ––routines ––triggers > all-databases.sql
2. 停止Mysql服务；
3. 重命名mysql数据文件夹；
4. 修改my.cnf中的参数，添加innodb_file_per_table；
5. mysql_install_db重新初始化mysqld；
6. 开启Mysql服务;
7. 进入Mysql Console执行：
	* SET FOREIGN_KEY_CHECKS=0;
	* SOURCE all-databases.sql;
	* SET FOREIGN_KEY_CHECKS=1;
8. 重启数据库测试OK就领赏去吧。
