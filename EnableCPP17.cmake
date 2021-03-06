function(checkflags flags ret)
    include(CheckCXXCompilerFlag)
    foreach(flag ${flags})
        check_cxx_compiler_flag("${flag}" SUPPORTS_FLAG)
        if(SUPPORTS_FLAG)
            message("-- Enabling ${flag}")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${flag}" PARENT_SCOPE)
            set(${ret} TRUE PARENT_SCOPE)
            return()
        endif()
    endforeach()
endfunction()

if(${CMAKE_CXX_COMPILER_ID} STREQUAL Clang OR CMAKE_COMPILER_IS_GNUCXX)
    set(CXX_FLAGS --std=c++17 --std=c++1z)
elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
    set(CXX_FLAGS /std:c++latest /std:c++17)
else()
    message(ERROR "Unknown compiler and/or C++17 flags. \
        Update EnableCPP17.cmake with compiler details")
endif()

set(ret FALSE)
checkflags("${CXX_FLAGS}" ret)
if(NOT ${ret})
    message(ERROR "Compiler does not support at least C++1z")
endif()