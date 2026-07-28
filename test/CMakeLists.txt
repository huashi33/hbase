
add_executable(hlog_test hlog_test.c)
target_link_libraries(hlog_test PUBLIC hcore)


add_executable(hcfg_test hcfg_test.c)
target_link_libraries(hcfg_test PUBLIC hcore)


add_executable(hhash_test hhash_test.c)
target_link_libraries(hhash_test PUBLIC hcore)
