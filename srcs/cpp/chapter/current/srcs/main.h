#ifndef MAIN_H_H_HEAD__FILE__
#define MAIN_H_H_HEAD__FILE__

#ifndef EXTERN_C
	#ifdef __cplusplus
		#define  EXTERN_C extern "C"
	#else
		#define  EXTERN_C
	#endif
#endif

EXTERN_C {
	#define __STDC_CONSTANT_MACROS
	#include <libavutil/avutil.h>
}
#endif // MAIN_H_H_HEAD__FILE__
