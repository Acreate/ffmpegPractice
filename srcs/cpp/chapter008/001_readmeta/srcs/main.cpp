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
}
#define __STDC_CONSTANT_MACROS
EXTERN_C {
}

int main( int argc, char **argv ) {
	const char *filename = "fuzhou.mp4";
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
	const AVDictionaryEntry *tag = NULL;
	// 遍历音视频文件的元数据
	while( ( tag = av_dict_get( fmt_ctx->metadata, "", tag, AV_DICT_IGNORE_SUFFIX ) ) ) {
		av_log( NULL, AV_LOG_INFO, "metadata %s=%s\n", tag->key, tag->value );
	}
	avformat_close_input( &fmt_ctx ); // 关闭音视频文件
	return 0;
}
