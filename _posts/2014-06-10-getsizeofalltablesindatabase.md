---
layout: post
title: "获取数据库中每个表的大小"
description: "获取数据库中每个表的大小"
category: scripts
tags: ["Mysql"]
---

获取数据库中每个表的大小

时有需求需要计算每个表占用的空间大小，如果有GUI可以直接点开查看属性，如果是成百上千个表又木有GUI中么办...

## MySQL:

<pre>
SELECT table_name AS "Table", 
round(((data_length + index_length) / 1024 / 1024), 2) "Size in MB" 
FROM information_schema.TABLES 
WHERE table_schema = "$DB_NAME"
 AND table_name = "$TABLE_NAME";
</pre>

## MSSQL:

<pre>
SELECT 
    t.NAME AS TableName,
    s.Name AS SchemaName,
    p.rows AS RowCounts,
    SUM(a.total_pages) * 8 AS TotalSpaceKB, 
    SUM(a.used_pages) * 8 AS UsedSpaceKB, 
    (SUM(a.total_pages) - SUM(a.used_pages)) * 8 AS UnusedSpaceKB
FROM 
    sys.tables t
INNER JOIN      
    sys.indexes i ON t.OBJECT_ID = i.object_id
INNER JOIN 
    sys.partitions p ON i.object_id = p.OBJECT_ID AND i.index_id = p.index_id
INNER JOIN 
    sys.allocation_units a ON p.partition_id = a.container_id
LEFT OUTER JOIN 
    sys.schemas s ON t.schema_id = s.schema_id
WHERE 
    t.NAME NOT LIKE 'dt%' 
    AND t.is_ms_shipped = 0
    AND i.OBJECT_ID > 255 
GROUP BY 
    t.Name, s.Name, p.Rows
ORDER BY 
    t.Name
</pre>
