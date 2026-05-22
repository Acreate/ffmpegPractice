#ifdef __cplusplus
#define  EXTERN_C extern "C"
#else
#define  EXTERN_C
#endif

EXTERN_C {
	#define __STDC_CONSTANT_MACROS
#include <libavutil/avutil.h>
}
int main( int argc, char *argv[ ], char *envp[ ] ) {
	av_log( nullptr, AV_LOG_INFO, "Hello World\n" ); // 打印日志
	return 0;
}
