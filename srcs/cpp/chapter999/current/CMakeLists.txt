cmake_minimum_required( VERSION 3.16 )

get_absolute_path( prject_name "." )
get_current_dir_name( prject_name ${prject_name} )

# # set( prject_name demo001 )
message( "============ ${prject_name}" )
message( "name = " ${prject_name} ) # # 当前文件名配置为项目名称
project( ${prject_name} VERSION 0.1 )
message( "============ ${CURRENT_FOLDER}" )

set( CMAKE_AUTOMOC ON )
set( CMAKE_AUTOUIC ON )
set( CMAKE_AUTORCC ON )
set( CMAKE_VERBOSE_MAKEFILE FALSE )
# 寻找多媒体组件
find_package( Qt6 REQUIRED COMPONENTS Multimedia OpenGLWidgets )

get_path_sources( project_src_file PATHS "${CMAKE_CURRENT_LIST_DIR}/srcs" LANGUAGES CXX C )

list( APPEND image_sources
	"${CMAKE_CURRENT_LIST_DIR}/qrc/resource.qrc"
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

# # 需要生成的 *.ts 文件保存到 translation_file_list 变量当中



set( translation_file_list_auto

	# # 美语

	"${PROJECT_SOURCE_DIR}/cmake_generate/translations/auto/${prject_name}_en.ts"

	# # 汉语
	"${PROJECT_SOURCE_DIR}/cmake_generate/translations/auto/${prject_name}_zh_CN.ts"
)

# # 需要生成的 *.ts 文件保存到 translation_file_list 变量当中



set( translation_app_default_file

	# # 默认加载的文件
	"${PROJECT_SOURCE_DIR}/cmake_generate/translations/default/${prject_name}.ts"
)

set( app_resouce_path "${app_run_bin}/program/" )
get_filename_component( app_resouce_path "${app_resouce_path}" ABSOLUTE ) # 全路径
set( app_resouce_path "${app_resouce_path}/" )
set( copy_file_src_dir "${CMAKE_CURRENT_SOURCE_DIR}/resources" )

# # 设置 *.pm 的输出位置
set_source_files_properties( ${translation_file_list_auto} PROPERTIES OUTPUT_LOCATION "${app_resouce_path}/translations/auto" )
set_source_files_properties( ${translation_app_default_file} PROPERTIES OUTPUT_LOCATION "${app_resouce_path}/translations/default" )

# # 生成 *.ts 和 *.pm 的脚本任务
qt_add_translations(
	"${prject_name}"
	TS_FILES ${translation_file_list_auto} ${translation_app_default_file}
	SOURCES ${project_src_file}
	LUPDATE_OPTIONS -no-obsolete
)

copy_dir_path_cmake_file_command( "${copy_file_src_dir}" "${app_resouce_path}" )

set_property( TARGET "${prject_name}" PROPERTY appResoucePath "${app_resouce_path}" )

set( find_path "${CMAKE_HOME_DIRECTORY}/srcs/lib/ffmpeg_8.1.1_full_build_shared" )
file( GLOB dll_file_s "${find_path}/bin/*.dll" )
copy_replace_mul_dir_path_cmake_file_command( FALSE "${app_run_bin}" dll_file_s )
target_include_directories( ${prject_name} PRIVATE "${find_path}/include" )
file( GLOB lib_file_s "${find_path}/lib/*.lib" )
target_link_libraries( ${prject_name} PRIVATE ${lib_file_s} )
file( GLOB lib_file_s "${CMAKE_HOME_DIRECTORY}/lib/*.a" )
target_link_libraries( ${prject_name} PRIVATE ${lib_file_s} )

set( find_path "${CMAKE_HOME_DIRECTORY}/srcs/lib/iconv_1.19" )
file( GLOB dll_file_s "${find_path}/x64/Debug/*.dll" )
copy_replace_mul_dir_path_cmake_file_command( FALSE "${app_run_bin}" dll_file_s )
target_include_directories( ${prject_name} PRIVATE "${find_path}/include" )
file( GLOB lib_file_s "${find_path}/x64/Debug/*.lib" )
target_link_libraries( ${prject_name} PRIVATE ${lib_file_s} )
file( GLOB lib_file_s "${find_path}/x64/Debug/*.a" )
target_link_libraries( ${prject_name} PRIVATE ${lib_file_s} )


set( find_path "${CMAKE_HOME_DIRECTORY}/srcs/lib/SDL2-2.32.10" )
target_include_directories( ${prject_name} PRIVATE "${find_path}/include" )
file( GLOB lib_file_s "${find_path}/lib/x64/*.lib" )
target_link_libraries( ${prject_name} PRIVATE ${lib_file_s} )
file( GLOB lib_file_s "${CMAKE_HOME_DIRECTORY}/lib/x64/*.a" )
target_link_libraries( ${prject_name} PRIVATE ${lib_file_s} )
file( GLOB dll_file_s "${find_path}/lib/x64/*.dll" )
copy_replace_mul_dir_path_cmake_file_command( FALSE "${app_run_bin}" dll_file_s )

copy_replace_dir_path_cmake_file_command( "${CMAKE_HOME_DIRECTORY}/srcs/cpp/FFmpeg resources/simsun.ttc" "${app_run_bin}" FALSE )
copy_replace_dir_path_cmake_file_command( "${CMAKE_HOME_DIRECTORY}/srcs/cpp/FFmpeg resources/fuzhou.mp4" "${app_run_bin}" FALSE )
copy_replace_dir_path_cmake_file_command( "${CMAKE_HOME_DIRECTORY}/srcs/cpp/FFmpeg resources/fuzhous.mp4" "${app_run_bin}" FALSE )
copy_replace_dir_path_cmake_file_command( "${CMAKE_HOME_DIRECTORY}/srcs/cpp/FFmpeg resources/seas.mp4" "${app_run_bin}" FALSE )
copy_replace_dir_path_cmake_file_command( "${CMAKE_HOME_DIRECTORY}/srcs/cpp/FFmpeg resources/sea.mp4" "${app_run_bin}" FALSE )
copy_replace_dir_path_cmake_file_command( "${CMAKE_HOME_DIRECTORY}/srcs/cpp/FFmpeg resources/plum.mp3" "${app_run_bin}" FALSE )
copy_replace_dir_path_cmake_file_command( "${CMAKE_HOME_DIRECTORY}/srcs/cpp/FFmpeg resources/2018.mp3" "${app_run_bin}" FALSE )
copy_replace_dir_path_cmake_file_command( "${CMAKE_HOME_DIRECTORY}/srcs/cpp/FFmpeg resources/ship.mp3" "${app_run_bin}" FALSE )
