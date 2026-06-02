#ifndef EXTERN_C
	#ifdef __cplusplus
		#define  EXTERN_C extern "C"
	#else
		#define  EXTERN_C
	#endif
#endif
#define __STDC_CONSTANT_MACROS

EXTERN_C {
	#include <libavutil/avutil.h>
	#include <libavformat/avformat.h>
}

int main( int argc, char *argv[ ] ) {
	// 目标文件
	const char *fileName = "fuzhou.mp4";
	if( argc > 1 )
		fileName = argv[ 1 ];
	AVFormatContext *fmtCtx = nullptr;
	// 打开音频文件
	int ret = avformat_open_input( &fmtCtx, fileName, nullptr, nullptr );
	if( ret < 0 ) {
		av_log( nullptr,AV_LOG_ERROR, "Can't open file %s.\n", fileName );
		return -1;
	}
	av_log( nullptr,AV_LOG_INFO, "Sucess open input_file %s.\n", fileName );
	// 查找音视频文件中的信息
	ret = avformat_find_stream_info( fmtCtx, nullptr );
	if( ret < 0 ) {
		av_log( nullptr,AV_LOG_ERROR, "Can't find stream infomation.\n" );
		return -1;
	}
	av_log( nullptr, AV_LOG_INFO, "Success find strean infomation.\n" );
	const AVInputFormat *iformat = fmtCtx->iformat;
	av_log( nullptr,AV_LOG_INFO, "format name is %s.\n", iformat->name );
	av_log( nullptr, AV_LOG_INFO, "format long_name is %s.\n", iformat->long_name );
	// 关闭音视频文件
	avformat_close_input( &fmtCtx );
	return 0;
}
