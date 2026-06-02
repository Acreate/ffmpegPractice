#ifdef __cplusplus
#define  EXTERN_C extern "C"
#else
#define  EXTERN_C
#endif
#define __STDC_CONSTANT_MACROS
EXTERN_C {
	#include <libavutil/avutil.h>
	#include <libavformat/avformat.h>
}

int main( int argc, char *argv[ ], char *envp[ ] ) {
	const char *filename = "2018.mp4";
	if( argc > 1 ) {
		filename = argv[ 1 ];
	}
	AVFormatContext *fmt_ctx = NULL;
	// 打开音视频文件
	int ret = avformat_open_input( &fmt_ctx, filename, NULL, NULL );
	if( ret < 0 ) {
		av_log( NULL, AV_LOG_ERROR, "Can't open file %s.\n", filename );
		return -1;
	}
	av_log( NULL, AV_LOG_INFO, "Success open input_file %s.\n", filename );
	// 查找音视频文件中的流信息
	ret = avformat_find_stream_info( fmt_ctx, NULL );
	if( ret < 0 ) {
		av_log( NULL, AV_LOG_ERROR, "Can't find stream information.\n" );
		return -1;
	}
	av_log( NULL, AV_LOG_INFO, "Success find stream information.\n" );
	// 格式化输出文件信息
	av_dump_format( fmt_ctx, 0, filename, 0 );
	av_log( NULL, AV_LOG_INFO, "duration=%d\n", fmt_ctx->duration ); // 持续时间，单位微秒
	av_log( NULL, AV_LOG_INFO, "bit_rate=%d\n", fmt_ctx->bit_rate ); // 比特率，单位比特每秒
	av_log( NULL, AV_LOG_INFO, "nb_streams=%d\n", fmt_ctx->nb_streams ); // 数据流的数量
	av_log( NULL, AV_LOG_INFO, "max_streams=%d\n", fmt_ctx->max_streams ); // 数据流的最大数量

	avformat_close_input( &fmt_ctx ); // 关闭音视频文件
	return 0;
}
