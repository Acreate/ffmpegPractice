
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
}

int main( int argc, char *argv[ ] ) {
	av_log( nullptr, AV_LOG_INFO, "Hello world\n" );
	return 0;
}
