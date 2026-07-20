
add_executable(hlog_test hlog_test.c)
target_link_libraries(hlog_test PUBLIC hbase zlog iniparser )


add_executable(hcfg_test hcfg_test.c)
target_link_libraries(hcfg_test PUBLIC hbase zlog iniparser )
