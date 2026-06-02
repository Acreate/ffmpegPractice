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


set( CMAKE_AUTOMOC ON )
set( CMAKE_AUTOUIC ON )
set( CMAKE_AUTORCC ON )
set( CMAKE_VERBOSE_MAKEFILE FALSE )

include( GenerateExportHeader )
include( GNUInstallDirs )


get_cmake_separator( _cmake_sep )
init_std_builder_path( )

set( qt_find_mode_list Widgets Core LinguistTools Network Multimedia OpenGLWidgets )

find_package( Qt6 QUIET COMPONENTS ${qt_find_mode_list} )

if( NOT Qt6_FOUND OR NOT EXISTS "${Qt6_DIR}" )
	# # 使用 qt6
	set( Current_Qt_Version 6.11.1 )
	set( Qt6_DIR "C:/Qt/${Current_Qt_Version}/msvc2022_64/lib/cmake/QT6/" )
	init_qt_dir_event( "${Qt6_DIR}" )
	find_package( Qt6 REQUIRED COMPONENTS ${qt_find_mode_list} )
else( )
	init_qt_dir_event( "${Qt6_DIR}" )
endif( )

printf_qt_cmake_out( )

qt_standard_project_setup( )


# # supper_cmake_builder_language( _cmake_supper_language_list ) # # 语言支持列表
append_source_file_extensions( LANGUAGES C CXX SUFFIXS h hpp hc H HPP HC ui UI qrc ) # # 追加后缀名

get_cmake_separator( cmake_sep_char )

get_path_sources( project_src_file PATHS "${CMAKE_CURRENT_LIST_DIR}/srcs" LANGUAGES CXX C )

list( APPEND image_sources
	#	"${CMAKE_CURRENT_LIST_DIR}/qrc/resource.qrc"
)
qt_add_resources(
	resource_qrc
	${image_sources}
)

list( APPEND project_src_file ${image_sources} ${resource_qrc} )

set( app_run_bin "${Project_Run_Bin_Path}/${prject_name}" )
set( app_static_lib_bin "${Project_Run_Static_Lib_Path}/${prject_name}" )
set( app_pdb_bin "${Project_Run_Pbd_Path}/${prject_name}" )

if( ${QT_VERSION_MAJOR} GREATER_EQUAL 6 )
	# cancel_target_PDB_propertie( "${prject_name}" )
	qt_add_executable( ${prject_name}
		MANUAL_FINALIZATION
		${project_src_file}
	)
	set_target_bin_out_path_property( "${prject_name}" "${app_run_bin}" )
	set_target_static_lib_out_path_property( "${prject_name}" "${app_static_lib_bin}" )
	set_target_pdb_out_path_property( "${prject_name}" "${app_pdb_bin}" )
	
	# target_obj_PDB_propertie( "${prject_name}" )
else( )
	if( ANDROID )
		add_library( ${prject_name} SHARED
			${project_src_file}
		)
	else( )
		add_executable( ${prject_name}
			${project_src_file}
		)
	endif( )
endif( )

target_link_libraries( ${prject_name} PRIVATE Qt6::Widgets Qt6::Core Qt6::Multimedia Qt6::OpenGLWidgets )

if( ${QT_VERSION} VERSION_LESS 6.1.0 )
	set( BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.${prject_name} )
endif( )

set_target_properties( "${prject_name}" PROPERTIES
	${BUNDLE_ID_OPTION}
	MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
	MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
	MACOSX_BUNDLE TRUE
	WIN32_EXECUTABLE FALSE
)

if( QT_VERSION_MAJOR EQUAL 6 )
	# # qt_finalize_executable( ${prject_name} )
	qt_finalize_target( "${prject_name}" )
endif( )

qt_generate_deploy_cmake_script_inatll_job( scrpt_path_ "${prject_name}" )

# # 生成文件
set( cmake_definitions_env_out "${CMAKE_CURRENT_LIST_DIR}/auto_generate_files/macro/" )
configure_temp_files( ${cmake_definitions_env_out} "${prject_name}" )

if( MINGW )
	# # target_compile_options( "${prject_name}" PRIVATE "-rdynamic" )
endif( )

if( MSVC )
	target_compile_options( "${prject_name}" PRIVATE "/utf-8" )
	un_show_include_info( ${prject_name} )
endif( )

target_include_directories( "${prject_name}" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/srcs" )

set( find_path "${CMAKE_HOME_DIRECTORY}/srcs/lib/ffmpeg_8.1.1_full_build_shared" )
file( GLOB dll_file_s "${find_path}/bin/*.dll" )
copy_replace_mul_dir_path_cmake_file_command( FALSE "${app_run_bin}" dll_file_s )
target_include_directories( ${prject_name} PRIVATE "${find_path}/include" )
file( GLOB lib_file_s "${find_path}/lib/*.lib" )
target_link_libraries( ${prject_name} PRIVATE ${lib_file_s} )
file( GLOB lib_file_s "${CMAKE_HOME_DIRECTORY}/lib/*.a" )
target_link_libraries( ${prject_name} PRIVATE ${lib_file_s} )

copy_replace_dir_path_cmake_file_command( "${CMAKE_HOME_DIRECTORY}/srcs/cpp/FFmpeg resources/fuzhou.mp4" "${app_run_bin}" FALSE )
