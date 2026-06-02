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

# qt 资源自动编译
set( CMAKE_AUTOMOC ON )
set( CMAKE_AUTOUIC ON )
set( CMAKE_AUTORCC ON )

get_cmake_separator( _cmake_sep )
init_std_builder_path( )


set( qt_find_mode_list Widgets Core LinguistTools Network Multimedia OpenGLWidgets )
set( qt_link_lib_list Qt6::Widgets Qt6::Core Qt6::Multimedia Qt6::OpenGLWidgets )

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

append_source_file_extensions( LANGUAGES C CXX SUFFIXS h hpp hc H HPP HC )

get_cmake_separator( cmake_sep_char )

get_path_sources( project_src_file PATHS "${CMAKE_CURRENT_LIST_DIR}/srcs" LANGUAGES CXX C )

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

target_link_libraries( ${prject_name} PRIVATE ${qt_link_lib_list} )

if( ${QT_VERSION} VERSION_LESS 6.1.0 )
	set( BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.${prject_name} )
endif( )

if( QT_VERSION_MAJOR EQUAL 6 )
	# # qt_finalize_executable( ${prject_name} )
	qt_finalize_target( "${prject_name}" )
endif( )

qt_generate_deploy_cmake_script_inatll_job( scrpt_path_ "${prject_name}" )


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
