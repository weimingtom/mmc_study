一、
libmemcached_functions_mysql.dll
拷贝到
C:\Program Files (x86)\MySQL\MySQL Server 5.1\lib\plugin
或者根据show variables like "plugin_dir";判断插件位置

二、
需要使用的dll
libmemcached.dll
libmysql.dll
pthreadVC2.dll
复制到
C:\Program Files (x86)\MySQL\MySQL Server 5.1\bin

三、
在命令行use <数据库>
然后执行install_functions_win32.sql的内容

四、
http://blog.sina.com.cn/s/blog_499740cb0100g45p.html

select name, dl from mysql.func;

显示

+------------------------------+----------------------------------+
| name                         | dl                               |
+------------------------------+----------------------------------+
| memc_add                     | libmemcached_functions_mysql.dll |
| memc_add_by_key              | libmemcached_functions_mysql.dll |
| memc_servers_set             | libmemcached_functions_mysql.dll |
| memc_server_count            | libmemcached_functions_mysql.dll |
| memc_set                     | libmemcached_functions_mysql.dll |
| memc_set_by_key              | libmemcached_functions_mysql.dll |
| memc_cas                     | libmemcached_functions_mysql.dll |
| memc_cas_by_key              | libmemcached_functions_mysql.dll |
| memc_get                     | libmemcached_functions_mysql.dll |
| memc_get_by_key              | libmemcached_functions_mysql.dll |
| memc_get_cas                 | libmemcached_functions_mysql.dll |
| memc_get_cas_by_key          | libmemcached_functions_mysql.dll |
| memc_delete                  | libmemcached_functions_mysql.dll |
| memc_delete_by_key           | libmemcached_functions_mysql.dll |
| memc_append                  | libmemcached_functions_mysql.dll |
| memc_append_by_key           | libmemcached_functions_mysql.dll |
| memc_prepend                 | libmemcached_functions_mysql.dll |
| memc_prepend_by_key          | libmemcached_functions_mysql.dll |
| memc_increment               | libmemcached_functions_mysql.dll |
| memc_decrement               | libmemcached_functions_mysql.dll |
| memc_replace                 | libmemcached_functions_mysql.dll |
| memc_replace_by_key          | libmemcached_functions_mysql.dll |
| memc_servers_behavior_set    | libmemcached_functions_mysql.dll |
| memc_servers_behavior_get    | libmemcached_functions_mysql.dll |
| memc_behavior_set            | libmemcached_functions_mysql.dll |
| memc_behavior_get            | libmemcached_functions_mysql.dll |
| memc_list_behaviors          | libmemcached_functions_mysql.dll |
| memc_list_hash_types         | libmemcached_functions_mysql.dll |
| memc_list_distribution_types | libmemcached_functions_mysql.dll |
| memc_udf_version             | libmemcached_functions_mysql.dll |
| memc_libmemcached_version    | libmemcached_functions_mysql.dll |
| memc_stats                   | libmemcached_functions_mysql.dll |
| memc_stat_get_keys           | libmemcached_functions_mysql.dll |
| memc_stat_get_value          | libmemcached_functions_mysql.dll |
+------------------------------+----------------------------------+



bug

C:\Program Files (x86)\MySQL\MySQL Server 5.1\bin>mysqld --console
100805  3:39:22 [Warning] '--default-character-set' is deprecated and will be re
moved in a future release. Please use '--character-set-server' instead.
100805  3:39:22 [Warning] '--log' is deprecated and will be removed in a future
release. Please use ''--general_log'/'--general_log_file'' instead.
100805  3:39:22 [Note] Plugin 'FEDERATED' is disabled.
InnoDB: The log sequence number in ibdata files does not match
InnoDB: the log sequence number in the ib_logfiles!
100805  3:39:22  InnoDB: Database was not shut down normally!
InnoDB: Starting crash recovery.
InnoDB: Reading tablespace information from the .ibd files...
InnoDB: Restoring possible half-written data pages from the doublewrite
InnoDB: buffer...
InnoDB: Last MySQL binlog file position 0 1336, file name C:\Program Files (x86)
\MySQL\MySQL Server 5.1\log-bin.000041
100805  3:39:22  InnoDB: Started; log sequence number 0 24397877
100805  3:39:22 [Note] Recovering after a crash using C:/Program Files (x86)/MyS
QL/MySQL Server 5.1/log-bin
100805  3:39:22 [Note] Starting crash recovery...
100805  3:39:22 [Note] Crash recovery finished.
100805  3:39:23 [Note] Event Scheduler: Loaded 0 events
100805  3:39:23 [Note] mysqld: ready for connections.
Version: '5.1.46-community-log'  socket: ''  port: 3306  MySQL Community Server
(GPL)
rc 0
min args 2 max args 3expiration 0
prepare_args finished
100805  3:40:49 - mysqld got exception 0x80000003 ;
This could be because you hit a bug. It is also possible that this binary
or one of the libraries it was linked against is corrupt, improperly built,
or misconfigured. This error can also be caused by malfunctioning hardware.
We will try our best to scrape up some info that will hopefully help diagnose
the problem, but since we have already crashed, something is definitely wrong
and this may fail.

key_buffer_size=25165824
read_buffer_size=65536
max_used_connections=1
max_threads=100
threads_connected=1
It is possible that mysqld could use up to
key_buffer_size + (read_buffer_size + sort_buffer_size)*max_threads = 57207 K
bytes of memory
Hope that's ok; if not, decrease some variables in the equation.

thd: 0x4617698
Attempting backtrace. You can use the following information to find out
where mysqld died. If you see no messages after this, something went
terribly wrong...
663F50A0    MSVCR90D.dll!_close()
6696D6BB    libmemcached.dll!memcached_io_close()[memcached_io.c:290]
6696EA5B    libmemcached.dll!memcached_quit_server()[memcached_quit.c:45]
6696D158    libmemcached.dll!memcached_io_read()[memcached_io.c:158]
6696EA49    libmemcached.dll!memcached_quit_server()[memcached_quit.c:43]
6696EC18    libmemcached.dll!memcached_quit()[memcached_quit.c:68]
66963DE7    libmemcached.dll!memcached_free()[memcached.c:44]
68457A23    libmemcached_functions_mysql.dll!memc_set_deinit()[set.c:82]
004A4EAE    mysqld.exe!udf_handler::cleanup()[item_func.cc:2793]
004A4EEE    mysqld.exe!Item_udf_func::cleanup()[item_func.cc:3103]
0044750F    mysqld.exe!Query_arena::free_items()[sql_class.cc:2411]
00449486    mysqld.exe!THD::cleanup_after_query()[sql_class.cc:1207]
0045C608    mysqld.exe!mysql_parse()[sql_parse.cc:5993]
0045CFC3    mysqld.exe!dispatch_command()[sql_parse.cc:1235]
0045DA07    mysqld.exe!do_command()[sql_parse.cc:878]
0047E290    mysqld.exe!handle_one_connection()[sql_connect.cc:1127]
0067F1CB    mysqld.exe!pthread_start()[my_winthread.c:85]
0065E6A3    mysqld.exe!_callthreadstart()[thread.c:293]
0065E73C    mysqld.exe!_threadstart()[thread.c:275]
76673677    kernel32.dll!BaseThreadInitThunk()
772F9D42    ntdll.dll!RtlInitializeExceptionChain()
772F9D15    ntdll.dll!RtlInitializeExceptionChain()
Trying to get some variables.
Some pointers may be invalid and cause the dump to abort...
thd->query at 08293358=select memc_set('mysql:doc1', bcol) from t1
thd->thread_id=1
thd->killed=NOT_KILLED
The manual page at http://dev.mysql.com/doc/mysql/en/crashing.html contains
information that should help you find out what is causing the crash.


bug->
memcached_io.c
#ifdef _WIN32

->

#ifndef _WIN32
        data_read= read(ptr->fd, ptr->read_buffer, MEMCACHED_MAX_BUFFER);
#else

