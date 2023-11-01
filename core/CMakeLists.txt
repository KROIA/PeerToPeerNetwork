cmake_minimum_required(VERSION 3.1.0)
# Neural net core 
set(P2PN_filename "P2PN")
set(P2PN_Include "inc")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()



MACRO(FILE_DIRECTORIES return_list ending)
    FILE(GLOB_RECURSE new_list ${ending})
    SET(dir_list "")
    FOREACH(file_path ${new_list})
        GET_FILENAME_COMPONENT(dir_path ${file_path} PATH)
        SET(dir_list ${dir_list} ${file_path})
    ENDFOREACH()
    LIST(REMOVE_DUPLICATES dir_list)
    SET(${return_list} ${dir_list})
ENDMACRO()


# QT settings
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
#set(CMAKE_AUTOUIC ON)

include(${QT_LOCATOR_CMAKE})
message("Search for QT5 finished")
# find_package(Qt5Widgets REQUIRED)
find_package(Qt5 COMPONENTS Network REQUIRED)
message("find_package for Qt5 finished")



# end QT settings


# External dependencies
include(FetchContent)
set(FETCHCONTENT_BASE_DIR "${CMAKE_SOURCE_DIR}/dependencies")

#  EasyProfiler
set(EASY_GIT_REPO  https://github.com/yse/easy_profiler.git)
set(EASY_GIT_TAG   v2.1.0)
set(BUILD_SHARED_LIBS OFF)
set(EASY_PROFILER_NO_SAMPLES True)
FetchContent_Declare(
 EASY_PROFILER
 GIT_REPOSITORY ${EASY_GIT_REPO}
 GIT_TAG ${EASY_GIT_TAG}
)
message("Download dependency: ${EASY_GIT_REPO}")

FetchContent_MakeAvailable(EASY_PROFILER)
#set(EASY_PROFILER_NO_GUI False)
set_target_properties(easy_profiler PROPERTIES CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/${RELATIVE_BUILD_FOLDER}")
set_target_properties(easy_profiler PROPERTIES CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/${RELATIVE_BUILD_FOLDER}")
set_target_properties(easy_profiler PROPERTIES CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/${RELATIVE_BUILD_FOLDER}")
set_target_properties(easy_profiler PROPERTIES DEBUG_POSTFIX "-d")

# End external dependencies

include_directories(inc)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

FILE_DIRECTORIES(H_FILES *.h)
FILE_DIRECTORIES(CPP_FILES *.cpp)
FILE_DIRECTORIES(UI_FILES *.ui)

qt5_wrap_cpp(CPP_MOC_FILES ${H_FILES})
qt5_wrap_ui(UIS_HDRS ${UI_FILES})


set(SOURCES
	${CPP_MOC_FILES}
    ${CPP_FILES}
    ${UIS_HDRS}
    ${H_FILES}
)

message("CMAKE_CURRENT_BINARY_DIR = " ${CMAKE_CURRENT_BINARY_DIR})

message("Create library with files:")
 FOREACH(file ${SOURCES})
    message("  File: " ${file})
 ENDFOREACH()



 set(P2PN_dependencies 
    Qt5::Network)

add_library(P2PN_shared SHARED ${SOURCES})
add_library(P2PN_static STATIC ${SOURCES})
add_library(P2PN_static_profile STATIC ${SOURCES})
 
target_compile_definitions(P2PN_shared PRIVATE  P2PN_LIB)
target_compile_definitions(P2PN_static PRIVATE  BUILD_STATIC P2PN_LIB)
target_compile_definitions(P2PN_static_profile PRIVATE  BUILD_STATIC P2PN_PROFILING P2PN_LIB)

set_target_properties(easy_profiler PROPERTIES DEBUG_POSTFIX "-d")
set_target_properties(P2PN_shared PROPERTIES LINKER_LANGUAGE CXX  OUTPUT_NAME "${P2PN_filename}" DEBUG_POSTFIX "-d")
set_target_properties(P2PN_static PROPERTIES LINKER_LANGUAGE CXX  OUTPUT_NAME "${P2PN_filename}-s" DEBUG_POSTFIX "-d")
set_target_properties(P2PN_static_profile PROPERTIES LINKER_LANGUAGE CXX  OUTPUT_NAME "${P2PN_filename}-s-p" DEBUG_POSTFIX "-d")


target_link_libraries(P2PN_shared ${P2PN_dependencies})
target_link_libraries(P2PN_static ${P2PN_dependencies})
target_link_libraries(P2PN_static_profile ${P2PN_dependencies} easy_profiler)


target_include_directories(P2PN_shared PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${P2PN_Include}>
    ${CMAKE_CURRENT_BINARY_DIR}
	# $<INSTALL_INTERFACE:inc>  # <prefix>/inc 
)
target_include_directories(P2PN_static PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${P2PN_Include}>
    ${CMAKE_CURRENT_BINARY_DIR}
	# $<INSTALL_INTERFACE:inc>  # <prefix>/inc 
)
target_include_directories(P2PN_static_profile PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${P2PN_Include}>
    ${CMAKE_CURRENT_BINARY_DIR}
	# $<INSTALL_INTERFACE:inc>  # <prefix>/inc 
)
# Define the install path
message("Installation to path: ${CMAKE_INSTALL_PREFIX}/bin")
message("Installation to path: ${CMAKE_INSTALL_PREFIX}/${RELATIVE_INSTALL_FOLDER}")
install(TARGETS P2PN_shared DESTINATION "${CMAKE_INSTALL_PREFIX}/bin")
install(TARGETS P2PN_shared DESTINATION "${CMAKE_INSTALL_PREFIX}/${RELATIVE_INSTALL_FOLDER}")
install(TARGETS P2PN_static DESTINATION "${CMAKE_INSTALL_PREFIX}/${RELATIVE_INSTALL_FOLDER}")
install(TARGETS P2PN_static_profile DESTINATION "${CMAKE_INSTALL_PREFIX}/${RELATIVE_INSTALL_FOLDER}")
install(TARGETS easy_profiler DESTINATION "${CMAKE_INSTALL_PREFIX}/${RELATIVE_INSTALL_FOLDER}")
