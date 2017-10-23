cmake_minimum_required (VERSION 2.8)
#CMAKE_SOURCE_DIR PROJECT_SOURCE_DIR <projectname>_SOURCE_DIR


message(${CMAKE_SOURCE_DIR})
message(${CMAKE_CURRENT_SOURCE_DIR})
message(${CMAKE_CURRENT_LIST_FILE})
message(${CMAKE_CURRENT_LIST_LINE})
#/Users/yiqin/Documents/github/Android_Java/ndk/cmakeDemo
#/Users/yiqin/Documents/github/Android_Java/ndk/cmakeDemo
#/Users/yiqin/Documents/github/Android_Java/ndk/cmakeDemo/CMakeLists.txt
#7



#project(demo)



message(main1)
add_subdirectory(math)
message(main2)
aux_source_directory(. DIR_SRCS)

message(main3)

add_executable(demo ${DIR_SRCS})

message(main4)
target_link_libraries(demo MFun)
message(main5)