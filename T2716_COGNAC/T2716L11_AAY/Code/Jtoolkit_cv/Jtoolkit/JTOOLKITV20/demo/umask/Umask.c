#include "Umask.h"
#include <sys/types.h>
#include <sys/stat.h>

JNIEXPORT void JNICALL Java_Umask_umaskSet
(JNIEnv *env, jclass cls, jint mode)
{
	umask((mode_t) mode);
}
JNIEXPORT jint JNICALL Java_Umask_umaskGet
(JNIEnv *env, jclass cls)
{
	/* Get old mask, temporarily setting the mask to 0.*/
	mode_t oldMask = umask((mode_t) 0);
	/* setting the old mask again */
	umask(oldMask);
	return (jint)oldMask;

}
