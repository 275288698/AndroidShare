message(MFUN1)
aux_source_directory(. DIR_SRCS)
message(MFUN2)
ADD_LIBRARY(MFun SHARED ${DIR_SRCS})
#add_library(MFun shared ${DIR_SRCS})
message(MFUN3)

#/Users/yiqin/Documents/github/Android_Java/ndk/cmakeDemo
#/Users/yiqin/Documents/github/Android_Java/ndk/cmakeDemo/math
message(${CMAKE_SOURCE_DIR})
message(${CMAKE_CURRENT_SOURCE_DIR})

message(${CMAKE_CURRENT_LIST_FILE})
message(${CMAKE_CURRENT_LIST_LINE})
#/Users/yiqin/Documents/github/Android_Java/ndk/cmakeDemo
#/Users/yiqin/Documents/github/Android_Java/ndk/cmakeDemo/math
#/Users/yiqin/Documents/github/Android_Java/ndk/cmakeDemo/math/CMakeLists.txt
#13
