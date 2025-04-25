# 妞ゅ湱娲伴崥宥囆�
project (sunnet)
# CMake閺堚偓娴ｅ海澧楅張锟介崣鐤�锟戒焦鐪�
cmake_minimum_required (VERSION 2.8)
# 婢跺瓨鏋冩禒鍓佹窗瑜帮拷
include_directories(include)
# 閺屻儲澹�./src閻╋拷瑜版洑绗呴惃鍕�澧嶉張澶嬬爱閺傚洣娆㈤敍灞肩箽鐎涙ê鍩孌IR_SRCS閸欐﹢鍣�
aux_source_directory(./src DIR_SRCS)
# 閻⑩€�++11
add_definitions(-std=c++11)
# 閹稿洤鐣鹃悽鐔稿灇閻╋拷閺嶅洦鏋冩禒锟�
add_executable(sunnet ${DIR_SRCS})
# 鎼存挻鏋冩禒锟�
find_package (Threads)
target_link_libraries (sunnet ${CMAKE_THREAD_LIBS_INIT})