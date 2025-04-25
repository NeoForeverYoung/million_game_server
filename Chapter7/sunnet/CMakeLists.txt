# 妞ゅ湱娲伴崥宥囆�
project (sunnet)
# CMake閺堚偓娴ｅ海澧楅張锟介崣鐤�锟戒焦鐪�
cmake_minimum_required (VERSION 2.8)
# 婢跺瓨鏋冩禒鍓佹窗瑜帮拷
include_directories(include)
# 閺屻儲澹�./src閻╋拷瑜版洑绗呴惃鍕�澧嶉張澶嬬爱閺傚洣娆㈤敍灞肩箽鐎涙ê鍩孌IR_SRCS閸欐﹢鍣�
aux_source_directory(./src DIR_SRCS)
# 濞ｈ�插�炴惔鎾存瀮娴犳儼鐭惧�帮拷
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/3rd/lua-5.3.5/src/)
# 閻⑩€�++11
add_definitions(-std=c++11)
# 閹稿洤鐣鹃悽鐔稿灇閻╋拷閺嶅洦鏋冩禒锟�
add_executable(sunnet ${DIR_SRCS})
# 鎼存挻鏋冩禒锟�
find_package (Threads)
target_link_libraries (sunnet ${CMAKE_THREAD_LIBS_INIT})
#lua婢跺瓨鏋冩禒韬测偓浣哥氨
include_directories(./3rd/lua-5.3.5/src)
target_link_libraries(sunnet liblua.a)
target_link_libraries(sunnet dl)