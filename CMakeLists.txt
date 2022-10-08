project(zstdhook)
cmake_minimum_required(VERSION 3.0)

add_library(zstdhook MODULE zstd_hook.c)
target_compile_options(zstdhook PRIVATE -Werror -Wall -Wextra)
target_link_libraries(zstdhook PRIVATE dl)
install(TARGETS zstdhook DESTINATION lib)
# vim: set et ts=4 sts=4 sw=4:
