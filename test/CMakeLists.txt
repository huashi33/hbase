
add_executable(hlog_test hlog_test.c)
target_link_libraries(hlog_test PUBLIC
    zlog
    hbase
)
