#ifdef __cplusplus
#define  EXTERN_C extern "C"
#else
#define  EXTERN_C
#endif
#define __STDC_CONSTANT_MACROS
EXTERN_C {
	#include <libavutil/avutil.h>
	#include <libavformat/avformat.h>
	#include <libavcodec/avcodec.h>
	#include <libavutil/audio_fifo.h>
}

AVFormatContext *in_fmt_ctx = NULL; // 输入文件的封装器实例
AVCodecContext *audio_decode_ctx = NULL; // 音频解码器的实例
int audio_index = -1; // 音频流的索引
AVCodecContext *audio_encode_ctx = NULL; // MP3编码器的实例

AVFormatContext *out_fmt_ctx; // 输出文件的封装器实例
AVAudioFifo *audiofifo = NULL;
AVFrame *encode_frame = NULL;
int cur_pts = 0;

// 打开输入文件
int open_input_file( const char *src_name ) {
	// 打开音视频文件
	int ret = avformat_open_input( &in_fmt_ctx, src_name, NULL, NULL );
	if( ret < 0 ) {
		av_log( NULL, AV_LOG_ERROR, "Can't open file %s.\n", src_name );
		return -1;
	}
	av_log( NULL, AV_LOG_INFO, "Success open input_file %s.\n", src_name );
	// 查找音视频文件中的流信息
	ret = avformat_find_stream_info( in_fmt_ctx, NULL );
	if( ret < 0 ) {
		av_log( NULL, AV_LOG_ERROR, "Can't find stream information.\n" );
		return -1;
	}
	// 找到音频流的索引
	audio_index = av_find_best_stream( in_fmt_ctx, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0 );
	if( audio_index >= 0 ) {
		AVStream *src_audio = in_fmt_ctx->streams[ audio_index ];
		enum AVCodecID audio_codec_id = src_audio->codecpar->codec_id;
		// 查找音频解码器
		AVCodec *audio_codec = ( AVCodec * ) avcodec_find_decoder( audio_codec_id );
		if( !audio_codec ) {
			av_log( NULL, AV_LOG_ERROR, "audio_codec not found\n" );
			return -1;
		}
		audio_decode_ctx = avcodec_alloc_context3( audio_codec ); // 分配解码器的实例
		if( !audio_decode_ctx ) {
			av_log( NULL, AV_LOG_ERROR, "audio_decode_ctx is null\n" );
			return -1;
		}
		// 把音频流中的编解码参数复制给解码器的实例
		avcodec_parameters_to_context( audio_decode_ctx, src_audio->codecpar );
		ret = avcodec_open2( audio_decode_ctx, audio_codec, NULL ); // 打开解码器的实例
		if( ret < 0 ) {
			av_log( NULL, AV_LOG_ERROR, "Can't open audio_decode_ctx.\n" );
			return -1;
		}
	} else {
		av_log( NULL, AV_LOG_ERROR, "Can't find audio stream.\n" );
		return -1;
	}
	return 0;
}

// 初始化MP3编码器的实例
int init_audio_encoder( void ) {
	// 查找MP3编码器
	AVCodec *audio_codec = ( AVCodec * ) avcodec_find_encoder( AV_CODEC_ID_MP3 );
	if( !audio_codec ) {
		av_log( NULL, AV_LOG_ERROR, "audio_codec not found\n" );
		return -1;
	}
	const enum AVSampleFormat *p = audio_codec->sample_fmts;
	while( *p != AV_SAMPLE_FMT_NONE ) { // 使用AV_SAMPLE_FMT_NONE作为结束符
		av_log( NULL, AV_LOG_INFO, "audio_codec support format %d\n", *p );
		p++;
	}
	// 获取编解码器上下文信息
	audio_encode_ctx = avcodec_alloc_context3( audio_codec );
	if( !audio_encode_ctx ) {
		av_log( NULL, AV_LOG_ERROR, "audio_encode_ctx is null\n" );
		return -1;
	}
	// lame库支持AV_SAMPLE_FMT_S16P、AV_SAMPLE_FMT_S32P、AV_SAMPLE_FMT_FLTP
	audio_encode_ctx->sample_fmt = audio_decode_ctx->sample_fmt; // 采样格式
	audio_encode_ctx->ch_layout = audio_decode_ctx->ch_layout; // 声道布局
	audio_encode_ctx->bit_rate = audio_decode_ctx->bit_rate; // 比特率，单位比特每秒
	audio_encode_ctx->sample_rate = audio_decode_ctx->sample_rate; // 采样率，单位次每秒
	int ret = avcodec_open2( audio_encode_ctx, audio_codec, NULL ); // 打开编码器的实例
	if( ret < 0 ) {
		av_log( NULL, AV_LOG_ERROR, "Can't open audio_encode_ctx.\n" );
		return -1;
	}

	// 初始化audiofifo
	audiofifo = av_audio_fifo_alloc( audio_encode_ctx->sample_fmt,
									audio_encode_ctx->ch_layout.nb_channels, audio_encode_ctx->frame_size );

	encode_frame = av_frame_alloc( );
	encode_frame->nb_samples = audio_encode_ctx->frame_size;
	encode_frame->sample_rate = audio_encode_ctx->sample_rate;
	//encode_frame->channel_layout = audio_encode_ctx->channel_layout;
	//encode_frame->channels = audio_encode_ctx->channels;
	encode_frame->ch_layout = audio_encode_ctx->ch_layout;
	encode_frame->format = audio_encode_ctx->sample_fmt;
	av_frame_get_buffer( encode_frame, 0 );

	return 0;
}

// 把音频帧保到MP3文件
int save_mp3_file2( FILE *fp_out, AVFrame *frame ) {
	AVPacket *packet = av_packet_alloc( ); // 分配一个数据包

	av_frame_make_writable( encode_frame );
	int cache_size = av_audio_fifo_size( audiofifo );
	av_log( NULL, AV_LOG_INFO, "cache_size: %d\n", cache_size );
	int ret = av_audio_fifo_realloc( audiofifo, cache_size + frame->nb_samples );
	av_audio_fifo_write( audiofifo, ( void ** ) ( frame->data ), frame->nb_samples );
	cache_size = av_audio_fifo_size( audiofifo );
	av_log( NULL, AV_LOG_INFO, "2 cache_size: %d\n", cache_size );
	if( cache_size < 1152 ) {
		return -1;
	}

	// todo 如果是冲刷最后几帧数据，不够的可以填充静音  av_samples_set_silence
	while( av_audio_fifo_size( audiofifo ) > audio_encode_ctx->frame_size ) {
		int ret = av_audio_fifo_read( audiofifo, ( void ** ) ( encode_frame->data ),
									audio_encode_ctx->frame_size );
		if( ret < 0 ) {
			av_log( NULL, AV_LOG_ERROR, "av_audio_fifo_size occur error %d.\n", ret );
			return -1;
		}
		cur_pts += encode_frame->nb_samples;
		encode_frame->pts = cur_pts;
		ret = avcodec_send_frame( audio_encode_ctx, encode_frame );
		if( ret < 0 ) {
			av_log( NULL, AV_LOG_ERROR, "avcodec_send_frame occur error %d.\n", ret );
			return -1;
		}
		while( ret == 0 ) {
			ret = avcodec_receive_packet( audio_encode_ctx, packet );
			if( ret == AVERROR( EAGAIN ) || ret == AVERROR_EOF ) {
				//av_log(NULL, AV_LOG_ERROR, "avcodec_receive_packet occur error %d.\n", ret);
				break;
			} else if( ret < 0 ) {
				av_log( NULL, AV_LOG_ERROR, "avcodec_receive_packet occur error %d.\n", ret );
				return -1;
			} else {
				// 把编码后的MP3数据包写入文件
				fwrite( packet->data, 1, packet->size, fp_out );
				//packet->stream_index = aac_index;
				//ret = av_write_frame(out_format_context, packet);
				//if (ret < 0) {
				//    av_log(NULL, AV_LOG_ERROR, "av_write_frame occur error %d.\n", ret);
				//    return;
				//}
			}
		}
	}

	return 0;
}

// 把音频帧保到MP3文件
int save_mp3_file( FILE *fp_out, AVFrame *frame ) {
	AVPacket *packet = av_packet_alloc( ); // 分配一个数据包
	// 把原始的数据帧发给编码器实例
	int ret = avcodec_send_frame( audio_encode_ctx, frame );
	while( ret == 0 ) {
		// 从编码器实例获取压缩后的数据包
		ret = avcodec_receive_packet( audio_encode_ctx, packet );
		if( ret == AVERROR( EAGAIN ) || ret == AVERROR_EOF ) {
			break;
		} else if( ret < 0 ) {
			av_log( NULL, AV_LOG_ERROR, "encode frame occur error %d.\n", ret );
			break;
		}
		// 把编码后的MP3数据包写入文件
		fwrite( packet->data, 1, packet->size, fp_out );
	}
	av_packet_free( &packet ); // 释放数据包资源
	return 0;
}

int main( int argc, char **argv ) {
	const char *src_name = "fuzhou.mp4";
	int save_index = 0;
	if( argc > 1 ) {
		src_name = argv[ 1 ];
	}
	const char *mp3_name = "output_savemp32.mp3";
	if( open_input_file( src_name ) < 0 ) { // 打开输入文件
		return -1;
	}
	if( init_audio_encoder( ) < 0 ) { // 初始化MP3编码器的实例
		return -1;
	}
	FILE *fp_out = fopen( mp3_name, "wb" ); // 以写方式打开输出文件
	if( !fp_out ) {
		av_log( NULL, AV_LOG_ERROR, "open mp3 file %s fail.\n", mp3_name );
		return -1;
	}
	av_log( NULL, AV_LOG_INFO, "target audio file is %s\n", mp3_name );

	int ret = -1;
	AVPacket *packet = av_packet_alloc( ); // 分配一个数据包
	AVFrame *frame = av_frame_alloc( ); // 分配一个数据帧
	while( av_read_frame( in_fmt_ctx, packet ) >= 0 ) { // 轮询数据包
		if( packet->stream_index == audio_index ) { // 视频包需要重新编码
			// 把未解压的数据包发给解码器实例
			ret = avcodec_send_packet( audio_decode_ctx, packet );
			if( ret == 0 ) {
				// 从解码器实例获取还原后的数据帧
				ret = avcodec_receive_frame( audio_decode_ctx, frame );
				if( ret == AVERROR( EAGAIN ) || ret == AVERROR_EOF ) {
					continue;
				} else if( ret < 0 ) {
					av_log( NULL, AV_LOG_ERROR, "decode frame occur error %d.\n", ret );
					continue;
				}
				//save_mp3_file(fp_out, frame); // 把音频帧保存到MP3文件
				save_mp3_file2( fp_out, frame ); // 把音频帧保存到MP3文件
			} else {
				av_log( NULL, AV_LOG_ERROR, "send packet occur error %d.\n", ret );
			}
		}
		av_packet_unref( packet ); // 清除数据包
	}
	save_mp3_file( fp_out, NULL ); // 传入一个空帧，冲走编码缓存
	av_log( NULL, AV_LOG_INFO, "Success save audio frame as mp3 file.\n", save_index );
	fclose( fp_out ); // 关闭输出文件

	//    save_mp3_file2(fp_out, NULL); // 传入一个空帧，冲走编码缓存
	//            ret = av_write_trailer(out_fmt_ctx);
	//        if (ret < 0) {
	//                av_log(NULL, AV_LOG_ERROR, "av_write_trailer occur error %d.\n", ret);
	//        }

	av_frame_free( &frame ); // 释放数据帧资源
	av_packet_free( &packet ); // 释放数据包资源
	avcodec_free_context( &audio_decode_ctx ); // 释放音频解码器的实例
	avcodec_free_context( &audio_encode_ctx ); // 释放音频编码器的实例
	avformat_close_input( &in_fmt_ctx ); // 关闭音视频文件
	return 0;
}
