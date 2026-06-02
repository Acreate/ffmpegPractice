cmake_minimum_required( VERSION 3.16 )

get_absolute_path( prject_name "." )
get_current_dir_name( prject_name ${prject_name} )

# # set( prject_name demo001 )
message( "============ ${prject_name}" )
message( "name = " ${prject_name} ) # # 当前文件名配置为项目名称
project( ${prject_name} VERSION 0.1 )
message( "============ ${CURRENT_FOLDER}" )


set( CMAKE_C_STANDARD 17 )
set( CMAKE_C_STANDARD_REQUIRED ON )
set( CMAKE_C_VISIBILITY_PRESET hidden )
set( CMAKE_C_EXTENSIONS ON )
set( CMAKE_CXX_STANDARD 23 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )
set( CMAKE_CXX_VISIBILITY_PRESET hidden )
set( CMAKE_CXX_EXTENSIONS ON )


get_cmake_separator( _cmake_sep )
init_std_builder_path( )


append_source_file_extensions( LANGUAGES C CXX SUFFIXS h hpp hc H HPP HC )

get_cmake_separator( cmake_sep_char )

get_path_sources( project_src_file PATHS "${CMAKE_CURRENT_LIST_DIR}/srcs" LANGUAGES CXX C )

set( app_run_bin "${Project_Run_Bin_Path}/${prject_name}" )
set( app_static_lib_bin "${Project_Run_Static_Lib_Path}/${prject_name}" )
set( app_pdb_bin "${Project_Run_Pbd_Path}/${prject_name}" )
add_executable( ${prject_name} ${project_src_file} )
set_target_bin_out_path_property( "${prject_name}" "${app_run_bin}" )
set_target_static_lib_out_path_property( "${prject_name}" "${app_static_lib_bin}" )
set_target_pdb_out_path_property( "${prject_name}" "${app_pdb_bin}" )

set_target_properties( "${prject_name}" PROPERTIES
	${BUNDLE_ID_OPTION}
	MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
	MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
	MACOSX_BUNDLE TRUE
	WIN32_EXECUTABLE FALSE
)


if( MSVC )
	target_compile_options( "${prject_name}" PRIVATE "/utf-8" )
	un_show_include_info( ${prject_name} )
endif( )

target_include_directories( "${prject_name}" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/srcs" )

set( find_path "${CMAKE_HOME_DIRECTORY}/srcs/lib/ffmpeg_8.1.1_full_build_shared" )
file( GLOB dll_file_s "${find_path}/bin/*.dll" )
copy_replace_mul_dir_path_cmake_file_command( FALSE "${app_run_bin}" dll_file_s )
file( GLOB dll_file_s "${find_path}/bin/*.so" )
copy_replace_mul_dir_path_cmake_file_command( FALSE "${app_run_bin}" dll_file_s )
target_include_directories( ${prject_name} PRIVATE "${find_path}/include" )
file( GLOB lib_file_s "${find_path}/lib/*.lib" )
target_link_libraries( ${prject_name} PRIVATE ${lib_file_s} )
file( GLOB lib_file_s "${CMAKE_HOME_DIRECTORY}/lib/*.a" )
target_link_libraries( ${prject_name} PRIVATE ${lib_file_s} )

copy_replace_dir_path_cmake_file_command( "${CMAKE_HOME_DIRECTORY}/srcs/resources/FFmpeg resources/2018.mp4" "${app_run_bin}" FALSE )
