#include <cmake_include_to_c_cpp_header_env.h>
#ifdef __cplusplus
#define  EXTERN_C extern "C"
#else
#define  EXTERN_C
#endif
EXTERN_C {
	#include <libavformat/avformat.h>
	#include <libavcodec/avcodec.h>
	#include <libavutil/avutil.h>
	#include <libavutil/imgutils.h>
	#include <libswresample/swresample.h>
	#include <libswscale/swscale.h>
	#include <libavformat/avformat.h>
	#include <libavcodec/avcodec.h>
	#include <libavfilter/avfilter.h>
	#include <libavfilter/buffersink.h>
	#include <libavfilter/buffersrc.h>
	#include <libavutil/avutil.h>
	#include <libavutil/opt.h>
	#include <libavutil/pixdesc.h>
	#include <libavutil/time.h>
}
#define __STDC_CONSTANT_MACROS
EXTERN_C {
}
#include <stdio.h>
#include <SDL.h>
// 引入SDL要增加下面的声明#undef main，否则编译会报错“undefined reference to `WinMain'”
#undef main

// 分线程的任务处理
int thread_work( void *arg ) {
	int loop_count = *( int * ) arg; // 取出线程的输入参数
	int i = 0;
	while( ++i < loop_count + 1 ) {
		av_log( NULL, AV_LOG_INFO, "The thread work on %d seconds\n", i );
		SDL_Delay( 1000 ); // 延迟若干时间，单位毫秒
	}
	return 1; // 返回线程的结束标志
}

int main( int argc, char **argv ) {
	int loop_count = 3;
	if( argc > 2 ) {
		loop_count = atoi( argv[ 2 ] );
	}
	// 创建SDL线程，指定任务处理函数，并返回线程编号
	SDL_Thread *sdl_thread = SDL_CreateThread( thread_work, "thread_work", &loop_count );
	if( !sdl_thread ) {
		av_log( NULL, AV_LOG_ERROR, "sdl create thread occur error\n" );
		return -1;
	}
	// 线程分离之后，即使调用SDL_WaitThread函数也不会等待线程结束
	//    SDL_DetachThread(sdl_thread);
	int finish_status; // 线程的结束标志
	SDL_WaitThread( sdl_thread, &finish_status ); // 等待线程结束，结束标志在status字段返回
	av_log( NULL, AV_LOG_INFO, "sdl_thread finish_status=%d\n", finish_status );
	return 0;
}
