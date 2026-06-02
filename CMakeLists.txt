cmake_minimum_required( VERSION 3.19 )

# # 加载模块
find_package( CmakeFuncs REQUIRED PATHS "${CMAKE_CURRENT_SOURCE_DIR}/srcs/cmake/cmakeFuncs/CmakeFuncs" )

# # 获取当前文件夹名称
get_current_dir_name( prject_name ${CMAKE_CURRENT_SOURCE_DIR} )
message( "============ ${prject_name}" )
message( "name = " ${prject_name} ) # # 当前文件名配置为项目名称
project( ${prject_name} )
message( "============ ${CURRENT_FOLDER}" )

get_path_cmake_dir_path( bin_list "${CMAKE_CURRENT_SOURCE_DIR}/srcs/cpp/ffmpeg_current_code" "CMakeLists.txt" )
list( APPEND out_path_list ${lib_list} ${bin_list} )
filter_path_repetition( list_result out_path_list )
append_sub_directory_cmake_project_path_list( list_result )

configure_all_target( )
