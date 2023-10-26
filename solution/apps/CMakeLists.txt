set(MAIN_SOURCE main.c)
set(PASS_SOURCE pass.cpp)

add_executable(application ${MAIN_SOURCE})
target_link_libraries(application PUBLIC life sim)


set(CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} "-c -flto")
add_executable(applicationir ${MAIN_SOURCE})
target_link_libraries(applicationir PUBLIC life sim)
target_compile_options(applicationir PRIVATE -c -S -emit-llvm)
