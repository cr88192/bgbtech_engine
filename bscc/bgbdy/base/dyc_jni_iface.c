#include <bgbdy.h>
#include <jni.h>

typedef struct JNINativeInterface_ JNIEnv_t;

jint JNICALL dyc_jni_GetVersion(JNIEnv *env);

jclass JNICALL dyc_jni_DefineClass(JNIEnv *env, const char *name,
	jobject loader, const jbyte *buf, jsize len);

jclass JNICALL dyc_jni_FindClass(JNIEnv *env, const char *name);

jmethodID JNICALL dyc_jni_FromReflectedMethod(JNIEnv *env, jobject method);

jfieldID JNICALL dyc_jni_FromReflectedField(JNIEnv *env, jobject field);

jobject JNICALL dyc_jni_ToReflectedMethod(JNIEnv *env, jclass cls,
	jmethodID methodID, jboolean isStatic);

jclass JNICALL dyc_jni_GetSuperclass(JNIEnv *env, jclass sub);

jboolean JNICALL dyc_jni_IsAssignableFrom(JNIEnv *env, jclass sub, jclass sup);

jobject JNICALL dyc_jni_ToReflectedField(JNIEnv *env, jclass cls,
	jfieldID fieldID, jboolean isStatic);

jint JNICALL dyc_jni_Throw(JNIEnv *env, jthrowable obj);
jint JNICALL dyc_jni_ThrowNew(JNIEnv *env, jclass clazz, const char *msg);
jthrowable JNICALL dyc_jni_ExceptionOccurred(JNIEnv *env);
void JNICALL dyc_jni_ExceptionDescribe(JNIEnv *env);
void JNICALL dyc_jni_ExceptionClear(JNIEnv *env);
void JNICALL dyc_jni_FatalError(JNIEnv *env, const char *msg);

jint JNICALL dyc_jni_PushLocalFrame(JNIEnv *env, jint cap);
jobject JNICALL dyc_jni_PopLocalFrame(JNIEnv *env, jobject res);

jobject JNICALL dyc_jni_NewGlobalRef(JNIEnv *env, jobject lobj);
void JNICALL dyc_jni_DeleteGlobalRef(JNIEnv *env, jobject gref);
void JNICALL dyc_jni_DeleteLocalRef(JNIEnv *env, jobject obj);
jboolean JNICALL dyc_jni_IsSameObject(JNIEnv *env, jobject obj1, jobject obj2);

jobject JNICALL dyc_jni_NewLocalRef(JNIEnv *env, jobject ref);
jint JNICALL dyc_jni_EnsureLocalCapacity(JNIEnv *env, jint i);

jobject JNICALL dyc_jni_AllocObject(JNIEnv *env, jclass clazz);
jobject JNICALL dyc_jni_NewObject(JNIEnv *env, jclass clazz, jmethodID methodID, ...);
jobject JNICALL dyc_jni_NewObjectV(JNIEnv *env, jclass clazz, jmethodID methodID, va_list args);
jobject JNICALL dyc_jni_NewObjectA(JNIEnv *env, jclass clazz, jmethodID methodID, jvalue *args);

jclass JNICALL dyc_jni_GetObjectClass(JNIEnv *env, jobject obj);
jboolean JNICALL dyc_jni_IsInstanceOf(JNIEnv *env, jobject obj, jclass clazz);

jmethodID JNICALL dyc_jni_GetMethodID(JNIEnv *env, jclass clazz, const char *name, const char *sig);

jobject JNICALL dyc_jni_CallObjectMethod(JNIEnv *env, jobject obj, jmethodID methodID, ...);
jobject JNICALL dyc_jni_CallObjectMethodV(JNIEnv *env, jobject obj, jmethodID methodID, va_list args);
jobject JNICALL dyc_jni_CallObjectMethodA(JNIEnv *env, jobject obj, jmethodID methodID, jvalue * args);

jboolean JNICALL dyc_jni_CallBooleanMethod(JNIEnv *env, jobject obj, jmethodID methodID, ...);
jboolean JNICALL dyc_jni_CallBooleanMethodV(JNIEnv *env, jobject obj, jmethodID methodID, va_list args);
jboolean JNICALL dyc_jni_CallBooleanMethodA(JNIEnv *env, jobject obj, jmethodID methodID, jvalue * args);

jbyte JNICALL dyc_jni_CallByteMethod(JNIEnv *env, jobject obj, jmethodID methodID, ...);
jbyte JNICALL dyc_jni_CallByteMethodV(JNIEnv *env, jobject obj, jmethodID methodID, va_list args);
jbyte JNICALL dyc_jni_CallByteMethodA(JNIEnv *env, jobject obj, jmethodID methodID, jvalue *args);

jchar JNICALL dyc_jni_CallCharMethod(JNIEnv *env, jobject obj, jmethodID methodID, ...);
jchar JNICALL dyc_jni_CallCharMethodV(JNIEnv *env, jobject obj, jmethodID methodID, va_list args);
jchar JNICALL dyc_jni_CallCharMethodA(JNIEnv *env, jobject obj, jmethodID methodID, jvalue *args);

jshort JNICALL dyc_jni_CallShortMethod(JNIEnv *env, jobject obj, jmethodID methodID, ...);
jshort JNICALL dyc_jni_CallShortMethodV(JNIEnv *env, jobject obj, jmethodID methodID, va_list args);
jshort JNICALL dyc_jni_CallShortMethodA(JNIEnv *env, jobject obj, jmethodID methodID, jvalue *args);

jint JNICALL dyc_jni_CallIntMethod(JNIEnv *env, jobject obj, jmethodID methodID, ...);
jint JNICALL dyc_jni_CallIntMethodV(JNIEnv *env, jobject obj, jmethodID methodID, va_list args);
jint JNICALL dyc_jni_CallIntMethodA(JNIEnv *env, jobject obj, jmethodID methodID, jvalue *args);

jlong JNICALL dyc_jni_CallLongMethod(JNIEnv *env, jobject obj, jmethodID methodID, ...);
jlong JNICALL dyc_jni_CallLongMethodV(JNIEnv *env, jobject obj, jmethodID methodID, va_list args);
jlong JNICALL dyc_jni_CallLongMethodA(JNIEnv *env, jobject obj, jmethodID methodID, jvalue *args);

jfloat JNICALL dyc_jni_CallFloatMethod(JNIEnv *env, jobject obj, jmethodID methodID, ...);
jfloat JNICALL dyc_jni_CallFloatMethodV(JNIEnv *env, jobject obj, jmethodID methodID, va_list args);
jfloat JNICALL dyc_jni_CallFloatMethodA(JNIEnv *env, jobject obj, jmethodID methodID, jvalue *args);

jdouble JNICALL dyc_jni_CallDoubleMethod(JNIEnv *env, jobject obj, jmethodID methodID, ...);
jdouble JNICALL dyc_jni_CallDoubleMethodV(JNIEnv *env, jobject obj, jmethodID methodID, va_list args);
jdouble JNICALL dyc_jni_CallDoubleMethodA(JNIEnv *env, jobject obj, jmethodID methodID, jvalue *args);

void JNICALL dyc_jni_CallVoidMethod(JNIEnv *env, jobject obj, jmethodID methodID, ...);
void JNICALL dyc_jni_CallVoidMethodV(JNIEnv *env, jobject obj, jmethodID methodID, va_list args);
void JNICALL dyc_jni_CallVoidMethodA(JNIEnv *env, jobject obj, jmethodID methodID, jvalue * args);

jobject JNICALL dyc_jni_CallNonvirtualObjectMethod(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, ...);
jobject JNICALL dyc_jni_CallNonvirtualObjectMethodV(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, va_list args);
jobject JNICALL dyc_jni_CallNonvirtualObjectMethodA(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, jvalue * args);

jboolean JNICALL dyc_jni_CallNonvirtualBooleanMethod(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, ...);
jboolean JNICALL dyc_jni_CallNonvirtualBooleanMethodV(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, va_list args);
jboolean JNICALL dyc_jni_CallNonvirtualBooleanMethodA(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, jvalue * args);

jbyte JNICALL dyc_jni_CallNonvirtualByteMethod(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, ...);
jbyte JNICALL dyc_jni_CallNonvirtualByteMethodV(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, va_list args);
jbyte JNICALL dyc_jni_CallNonvirtualByteMethodA(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, jvalue *args);

jchar JNICALL dyc_jni_CallNonvirtualCharMethod(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, ...);
jchar JNICALL dyc_jni_CallNonvirtualCharMethodV(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, va_list args);
jchar JNICALL dyc_jni_CallNonvirtualCharMethodA(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, jvalue *args);

jshort JNICALL dyc_jni_CallNonvirtualShortMethod(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, ...);
jshort JNICALL dyc_jni_CallNonvirtualShortMethodV(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, va_list args);
jshort JNICALL dyc_jni_CallNonvirtualShortMethodA(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, jvalue *args);

jint JNICALL dyc_jni_CallNonvirtualIntMethod(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, ...);
jint JNICALL dyc_jni_CallNonvirtualIntMethodV(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, va_list args);
jint JNICALL dyc_jni_CallNonvirtualIntMethodA(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, jvalue *args);

jlong JNICALL dyc_jni_CallNonvirtualLongMethod(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, ...);
jlong JNICALL dyc_jni_CallNonvirtualLongMethodV(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, va_list args);
jlong JNICALL dyc_jni_CallNonvirtualLongMethodA(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, jvalue *args);

jfloat JNICALL dyc_jni_CallNonvirtualFloatMethod(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, ...);
jfloat JNICALL dyc_jni_CallNonvirtualFloatMethodV(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, va_list args);
jfloat JNICALL dyc_jni_CallNonvirtualFloatMethodA(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, jvalue *args);

jdouble JNICALL dyc_jni_CallNonvirtualDoubleMethod(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, ...);
jdouble JNICALL dyc_jni_CallNonvirtualDoubleMethodV(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, va_list args);
jdouble JNICALL dyc_jni_CallNonvirtualDoubleMethodA(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, jvalue *args);

void JNICALL dyc_jni_CallNonvirtualVoidMethod(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, ...);
void JNICALL dyc_jni_CallNonvirtualVoidMethodV(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, va_list args);
void JNICALL dyc_jni_CallNonvirtualVoidMethodA(JNIEnv *env, jobject obj, jclass clazz, jmethodID methodID, jvalue * args);

jfieldID JNICALL dyc_jni_GetFieldID(JNIEnv *env, jclass clazz, const char *name, const char *sig);

jobject JNICALL dyc_jni_GetObjectField(JNIEnv *env, jobject obj, jfieldID fieldID);
jboolean JNICALL dyc_jni_GetBooleanField(JNIEnv *env, jobject obj, jfieldID fieldID);
jbyte JNICALL dyc_jni_GetByteField(JNIEnv *env, jobject obj, jfieldID fieldID);
jchar JNICALL dyc_jni_GetCharField(JNIEnv *env, jobject obj, jfieldID fieldID);
jshort JNICALL dyc_jni_GetShortField(JNIEnv *env, jobject obj, jfieldID fieldID);
jint JNICALL dyc_jni_GetIntField(JNIEnv *env, jobject obj, jfieldID fieldID);
jlong JNICALL dyc_jni_GetLongField(JNIEnv *env, jobject obj, jfieldID fieldID);
jfloat JNICALL dyc_jni_GetFloatField(JNIEnv *env, jobject obj, jfieldID fieldID);
jdouble JNICALL dyc_jni_GetDoubleField(JNIEnv *env, jobject obj, jfieldID fieldID);

void JNICALL dyc_jni_SetObjectField(JNIEnv *env, jobject obj, jfieldID fieldID, jobject val);
void JNICALL dyc_jni_SetBooleanField(JNIEnv *env, jobject obj, jfieldID fieldID, jboolean val);
void JNICALL dyc_jni_SetByteField(JNIEnv *env, jobject obj, jfieldID fieldID, jbyte val);
void JNICALL dyc_jni_SetCharField(JNIEnv *env, jobject obj, jfieldID fieldID, jchar val);
void JNICALL dyc_jni_SetShortField(JNIEnv *env, jobject obj, jfieldID fieldID, jshort val);
void JNICALL dyc_jni_SetIntField(JNIEnv *env, jobject obj, jfieldID fieldID, jint val);
void JNICALL dyc_jni_SetLongField(JNIEnv *env, jobject obj, jfieldID fieldID, jlong val);
void JNICALL dyc_jni_SetFloatField(JNIEnv *env, jobject obj, jfieldID fieldID, jfloat val);
void JNICALL dyc_jni_SetDoubleField(JNIEnv *env, jobject obj, jfieldID fieldID, jdouble val);

jmethodID JNICALL dyc_jni_GetStaticMethodID(JNIEnv *env, jclass clazz, const char *name, const char *sig);

jobject JNICALL dyc_jni_CallStaticObjectMethod(JNIEnv *env, jclass clazz, jmethodID methodID, ...);
jobject JNICALL dyc_jni_CallStaticObjectMethodV(JNIEnv *env, jclass clazz, jmethodID methodID, va_list args);
jobject JNICALL dyc_jni_CallStaticObjectMethodA(JNIEnv *env, jclass clazz, jmethodID methodID, jvalue *args);

jboolean JNICALL dyc_jni_CallStaticBooleanMethod(JNIEnv *env, jclass clazz, jmethodID methodID, ...);
jboolean JNICALL dyc_jni_CallStaticBooleanMethodV(JNIEnv *env, jclass clazz, jmethodID methodID, va_list args);
jboolean JNICALL dyc_jni_CallStaticBooleanMethodA(JNIEnv *env, jclass clazz, jmethodID methodID, jvalue *args);

jbyte JNICALL dyc_jni_CallStaticByteMethod(JNIEnv *env, jclass clazz, jmethodID methodID, ...);
jbyte JNICALL dyc_jni_CallStaticByteMethodV(JNIEnv *env, jclass clazz, jmethodID methodID, va_list args);
jbyte JNICALL dyc_jni_CallStaticByteMethodA(JNIEnv *env, jclass clazz, jmethodID methodID, jvalue *args);

jchar JNICALL dyc_jni_CallStaticCharMethod(JNIEnv *env, jclass clazz, jmethodID methodID, ...);
jchar JNICALL dyc_jni_CallStaticCharMethodV(JNIEnv *env, jclass clazz, jmethodID methodID, va_list args);
jchar JNICALL dyc_jni_CallStaticCharMethodA(JNIEnv *env, jclass clazz, jmethodID methodID, jvalue *args);

jshort JNICALL dyc_jni_CallStaticShortMethod(JNIEnv *env, jclass clazz, jmethodID methodID, ...);
jshort JNICALL dyc_jni_CallStaticShortMethodV(JNIEnv *env, jclass clazz, jmethodID methodID, va_list args);
jshort JNICALL dyc_jni_CallStaticShortMethodA(JNIEnv *env, jclass clazz, jmethodID methodID, jvalue *args);

jint JNICALL dyc_jni_CallStaticIntMethod(JNIEnv *env, jclass clazz, jmethodID methodID, ...);
jint JNICALL dyc_jni_CallStaticIntMethodV(JNIEnv *env, jclass clazz, jmethodID methodID, va_list args);
jint JNICALL dyc_jni_CallStaticIntMethodA(JNIEnv *env, jclass clazz, jmethodID methodID, jvalue *args);

jlong JNICALL dyc_jni_CallStaticLongMethod(JNIEnv *env, jclass clazz, jmethodID methodID, ...);
jlong JNICALL dyc_jni_CallStaticLongMethodV(JNIEnv *env, jclass clazz, jmethodID methodID, va_list args);
jlong JNICALL dyc_jni_CallStaticLongMethodA(JNIEnv *env, jclass clazz, jmethodID methodID, jvalue *args);

jfloat JNICALL dyc_jni_CallStaticFloatMethod(JNIEnv *env, jclass clazz, jmethodID methodID, ...);
jfloat JNICALL dyc_jni_CallStaticFloatMethodV(JNIEnv *env, jclass clazz, jmethodID methodID, va_list args);
jfloat JNICALL dyc_jni_CallStaticFloatMethodA(JNIEnv *env, jclass clazz, jmethodID methodID, jvalue *args);

jdouble JNICALL dyc_jni_CallStaticDoubleMethod(JNIEnv *env, jclass clazz, jmethodID methodID, ...);
jdouble JNICALL dyc_jni_CallStaticDoubleMethodV(JNIEnv *env, jclass clazz, jmethodID methodID, va_list args);
jdouble JNICALL dyc_jni_CallStaticDoubleMethodA(JNIEnv *env, jclass clazz, jmethodID methodID, jvalue *args);

void JNICALL dyc_jni_CallStaticVoidMethod(JNIEnv *env, jclass cls, jmethodID methodID, ...);
void JNICALL dyc_jni_CallStaticVoidMethodV(JNIEnv *env, jclass cls, jmethodID methodID, va_list args);
void JNICALL dyc_jni_CallStaticVoidMethodA(JNIEnv *env, jclass cls, jmethodID methodID, jvalue * args);

jfieldID JNICALL dyc_jni_GetStaticFieldID(JNIEnv *env, jclass clazz, const char *name, const char *sig);
jobject JNICALL dyc_jni_GetStaticObjectField(JNIEnv *env, jclass clazz, jfieldID fieldID);
jboolean JNICALL dyc_jni_GetStaticBooleanField(JNIEnv *env, jclass clazz, jfieldID fieldID);
jbyte JNICALL dyc_jni_GetStaticByteField(JNIEnv *env, jclass clazz, jfieldID fieldID);
jchar JNICALL dyc_jni_GetStaticCharField(JNIEnv *env, jclass clazz, jfieldID fieldID);
jshort JNICALL dyc_jni_GetStaticShortField(JNIEnv *env, jclass clazz, jfieldID fieldID);
jint JNICALL dyc_jni_GetStaticIntField(JNIEnv *env, jclass clazz, jfieldID fieldID);
jlong JNICALL dyc_jni_GetStaticLongField(JNIEnv *env, jclass clazz, jfieldID fieldID);
jfloat JNICALL dyc_jni_GetStaticFloatField(JNIEnv *env, jclass clazz, jfieldID fieldID);
jdouble JNICALL dyc_jni_GetStaticDoubleField(JNIEnv *env, jclass clazz, jfieldID fieldID);

void JNICALL dyc_jni_SetStaticObjectField(JNIEnv *env, jclass clazz, jfieldID fieldID, jobject value);
void JNICALL dyc_jni_SetStaticBooleanField(JNIEnv *env, jclass clazz, jfieldID fieldID, jboolean value);
void JNICALL dyc_jni_SetStaticByteField(JNIEnv *env, jclass clazz, jfieldID fieldID, jbyte value);
void JNICALL dyc_jni_SetStaticCharField(JNIEnv *env, jclass clazz, jfieldID fieldID, jchar value);
void JNICALL dyc_jni_SetStaticShortField(JNIEnv *env, jclass clazz, jfieldID fieldID, jshort value);
void JNICALL dyc_jni_SetStaticIntField(JNIEnv *env, jclass clazz, jfieldID fieldID, jint value);
void JNICALL dyc_jni_SetStaticLongField(JNIEnv *env, jclass clazz, jfieldID fieldID, jlong value);
void JNICALL dyc_jni_SetStaticFloatField(JNIEnv *env, jclass clazz, jfieldID fieldID, jfloat value);
void JNICALL dyc_jni_SetStaticDoubleField(JNIEnv *env, jclass clazz, jfieldID fieldID, jdouble value);

jstring JNICALL dyc_jni_NewString(JNIEnv *env, const jchar *unicode, jsize len);
jsize JNICALL dyc_jni_GetStringLength(JNIEnv *env, jstring str);
const jchar * JNICALL dyc_jni_GetStringChars(JNIEnv *env, jstring str, jboolean *isCopy);
void JNICALL dyc_jni_ReleaseStringChars(JNIEnv *env, jstring str, const jchar *chars);

jstring JNICALL dyc_jni_NewStringUTF(JNIEnv *env, const char *utf);
jsize JNICALL dyc_jni_GetStringUTFLength(JNIEnv *env, jstring str);
const char* JNICALL dyc_jni_GetStringUTFChars(JNIEnv *env, jstring str, jboolean *isCopy);
void JNICALL dyc_jni_ReleaseStringUTFChars(JNIEnv *env, jstring str, const char* chars);

jsize JNICALL dyc_jni_GetArrayLength(JNIEnv *env, jarray array);

jobjectArray JNICALL dyc_jni_NewObjectArray(JNIEnv *env, jsize len, jclass clazz, jobject init);
jobject JNICALL dyc_jni_GetObjectArrayElement(JNIEnv *env, jobjectArray array, jsize index);
void JNICALL dyc_jni_SetObjectArrayElement(JNIEnv *env, jobjectArray array, jsize index, jobject val);

jbooleanArray JNICALL dyc_jni_NewBooleanArray(JNIEnv *env, jsize len);
jbyteArray JNICALL dyc_jni_NewByteArray(JNIEnv *env, jsize len);
jcharArray JNICALL dyc_jni_NewCharArray(JNIEnv *env, jsize len);
jshortArray JNICALL dyc_jni_NewShortArray(JNIEnv *env, jsize len);
jintArray JNICALL dyc_jni_NewIntArray(JNIEnv *env, jsize len);
jlongArray JNICALL dyc_jni_NewLongArray(JNIEnv *env, jsize len);
jfloatArray JNICALL dyc_jni_NewFloatArray(JNIEnv *env, jsize len);
jdoubleArray JNICALL dyc_jni_NewDoubleArray(JNIEnv *env, jsize len);

jboolean * JNICALL dyc_jni_GetBooleanArrayElements(JNIEnv *env, jbooleanArray array, jboolean *isCopy);
jbyte * JNICALL dyc_jni_GetByteArrayElements(JNIEnv *env, jbyteArray array, jboolean *isCopy);
jchar * JNICALL dyc_jni_GetCharArrayElements(JNIEnv *env, jcharArray array, jboolean *isCopy);
jshort * JNICALL dyc_jni_GetShortArrayElements(JNIEnv *env, jshortArray array, jboolean *isCopy);
jint * JNICALL dyc_jni_GetIntArrayElements(JNIEnv *env, jintArray array, jboolean *isCopy);
jlong * JNICALL dyc_jni_GetLongArrayElements(JNIEnv *env, jlongArray array, jboolean *isCopy);
jfloat * JNICALL dyc_jni_GetFloatArrayElements(JNIEnv *env, jfloatArray array, jboolean *isCopy);
jdouble * JNICALL dyc_jni_GetDoubleArrayElements(JNIEnv *env, jdoubleArray array, jboolean *isCopy);

void JNICALL dyc_jni_ReleaseBooleanArrayElements(JNIEnv *env, jbooleanArray array, jboolean *elems, jint mode);
void JNICALL dyc_jni_ReleaseByteArrayElements(JNIEnv *env, jbyteArray array, jbyte *elems, jint mode);
void JNICALL dyc_jni_ReleaseCharArrayElements(JNIEnv *env, jcharArray array, jchar *elems, jint mode);
void JNICALL dyc_jni_ReleaseShortArrayElements(JNIEnv *env, jshortArray array, jshort *elems, jint mode);
void JNICALL dyc_jni_ReleaseIntArrayElements(JNIEnv *env, jintArray array, jint *elems, jint mode);
void JNICALL dyc_jni_ReleaseLongArrayElements(JNIEnv *env, jlongArray array, jlong *elems, jint mode);
void JNICALL dyc_jni_ReleaseFloatArrayElements(JNIEnv *env, jfloatArray array, jfloat *elems, jint mode);
void JNICALL dyc_jni_ReleaseDoubleArrayElements(JNIEnv *env, jdoubleArray array, jdouble *elems, jint mode);

void JNICALL dyc_jni_GetBooleanArrayRegion(JNIEnv *env, jbooleanArray array, jsize start, jsize l, jboolean *buf);
void JNICALL dyc_jni_GetByteArrayRegion(JNIEnv *env, jbyteArray array, jsize start, jsize len, jbyte *buf);
void JNICALL dyc_jni_GetCharArrayRegion(JNIEnv *env, jcharArray array, jsize start, jsize len, jchar *buf);
void JNICALL dyc_jni_GetShortArrayRegion(JNIEnv *env, jshortArray array, jsize start, jsize len, jshort *buf);
void JNICALL dyc_jni_GetIntArrayRegion(JNIEnv *env, jintArray array, jsize start, jsize len, jint *buf);
void JNICALL dyc_jni_GetLongArrayRegion(JNIEnv *env, jlongArray array, jsize start, jsize len, jlong *buf);
void JNICALL dyc_jni_GetFloatArrayRegion(JNIEnv *env, jfloatArray array, jsize start, jsize len, jfloat *buf);
void JNICALL dyc_jni_GetDoubleArrayRegion(JNIEnv *env, jdoubleArray array, jsize start, jsize len, jdouble *buf);

void JNICALL dyc_jni_SetBooleanArrayRegion(JNIEnv *env, jbooleanArray array, jsize start, jsize l, jboolean *buf);
void JNICALL dyc_jni_SetByteArrayRegion(JNIEnv *env, jbyteArray array, jsize start, jsize len, jbyte *buf);
void JNICALL dyc_jni_SetCharArrayRegion(JNIEnv *env, jcharArray array, jsize start, jsize len, jchar *buf);
void JNICALL dyc_jni_SetShortArrayRegion(JNIEnv *env, jshortArray array, jsize start, jsize len, jshort *buf);
void JNICALL dyc_jni_SetIntArrayRegion(JNIEnv *env, jintArray array, jsize start, jsize len, jint *buf);
void JNICALL dyc_jni_SetLongArrayRegion(JNIEnv *env, jlongArray array, jsize start, jsize len, jlong *buf);
void JNICALL dyc_jni_SetFloatArrayRegion(JNIEnv *env, jfloatArray array, jsize start, jsize len, jfloat *buf);
void JNICALL dyc_jni_SetDoubleArrayRegion(JNIEnv *env, jdoubleArray array, jsize start, jsize len, jdouble *buf);

jint JNICALL dyc_jni_RegisterNatives(JNIEnv *env, jclass clazz, const JNINativeMethod *methods, jint nMethods);
jint JNICALL dyc_jni_UnregisterNatives(JNIEnv *env, jclass clazz);

jint JNICALL dyc_jni_MonitorEnter(JNIEnv *env, jobject obj);
jint JNICALL dyc_jni_MonitorExit(JNIEnv *env, jobject obj);
 
jint JNICALL dyc_jni_GetJavaVM(JNIEnv *env, JavaVM **vm);

void JNICALL dyc_jni_GetStringRegion(JNIEnv *env, jstring str, jsize i, jsize j, jchar *buf);
void JNICALL dyc_jni_GetStringUTFRegion(JNIEnv *env, jstring str, jsize i, jsize j, char *buf);

void* JNICALL dyc_jni_GetPrimitiveArrayCritical(JNIEnv *env, jarray array, jboolean *isCopy);
void JNICALL dyc_jni_ReleasePrimitiveArrayCritical(JNIEnv *env, jarray array, void* carray, jint mode);

const jchar* JNICALL dyc_jni_GetStringCritical(JNIEnv *env, jstring s, jboolean* isCopy);
void JNICALL dyc_jni_ReleaseStringCritical(JNIEnv *env, jstring s, const jchar* cstr);

jweak JNICALL dyc_jni_NewWeakGlobalRef(JNIEnv *env, jobject obj);
void JNICALL dyc_jni_DeleteWeakGlobalRef(JNIEnv *env, jweak obj);

jboolean JNICALL dyc_jni_ExceptionCheck(JNIEnv *env);

jobject JNICALL dyc_jni_NewDirectByteBuffer(JNIEnv* env, void* address, jlong capacity);
void* JNICALL dyc_jni_GetDirectBufferAddress(JNIEnv* env, jobject buf);
jlong JNICALL dyc_jni_GetDirectBufferCapacity(JNIEnv* env, jobject buf);

struct JNINativeInterface_ dyc_jni_iface=
{
NULL,						//0
NULL,						//1
NULL,						//2
NULL,						//3
dyc_jni_GetVersion,				//4
dyc_jni_DefineClass,				//5
dyc_jni_FindClass,				//6
dyc_jni_FromReflectedMethod,			//7
dyc_jni_FromReflectedField,			//8
dyc_jni_ToReflectedMethod,			//9
dyc_jni_GetSuperclass,				//10
dyc_jni_IsAssignableFrom,			//11
dyc_jni_ToReflectedField,			//12
dyc_jni_Throw,					//13
dyc_jni_ThrowNew,				//14
dyc_jni_ExceptionOccurred,			//15
dyc_jni_ExceptionDescribe,			//16
dyc_jni_ExceptionClear,			//17
dyc_jni_FatalError,				//18
dyc_jni_PushLocalFrame,			//19
dyc_jni_PopLocalFrame,				//20
dyc_jni_NewGlobalRef,				//21
dyc_jni_DeleteGlobalRef,			//22
dyc_jni_DeleteLocalRef,			//23
dyc_jni_IsSameObject,				//24
dyc_jni_NewLocalRef,				//25
dyc_jni_EnsureLocalCapacity,			//26
dyc_jni_AllocObject,				//27
dyc_jni_NewObject,				//28
dyc_jni_NewObjectV,				//29
dyc_jni_NewObjectA,				//30
dyc_jni_GetObjectClass,			//31
dyc_jni_IsInstanceOf,				//32
dyc_jni_GetMethodID,				//33
dyc_jni_CallObjectMethod,			//34
dyc_jni_CallObjectMethodV,			//35
dyc_jni_CallObjectMethodA,			//36
dyc_jni_CallBooleanMethod,			//37
dyc_jni_CallBooleanMethodV,			//38
dyc_jni_CallBooleanMethodA,			//39
dyc_jni_CallByteMethod,			//40
dyc_jni_CallByteMethodV,			//41
dyc_jni_CallByteMethodA,			//42
dyc_jni_CallCharMethod,			//43
dyc_jni_CallCharMethodV,			//44
dyc_jni_CallCharMethodA,			//45
dyc_jni_CallShortMethod,			//46
dyc_jni_CallShortMethodV,			//47
dyc_jni_CallShortMethodA,			//48
dyc_jni_CallIntMethod,				//49
dyc_jni_CallIntMethodV,			//50
dyc_jni_CallIntMethodA,			//51
dyc_jni_CallLongMethod,			//52
dyc_jni_CallLongMethodV,			//53
dyc_jni_CallLongMethodA,			//54
dyc_jni_CallFloatMethod,			//55
dyc_jni_CallFloatMethodV,			//56
dyc_jni_CallFloatMethodA,			//57
dyc_jni_CallDoubleMethod,			//58
dyc_jni_CallDoubleMethodV,			//59
dyc_jni_CallDoubleMethodA,			//60
dyc_jni_CallVoidMethod,			//61
dyc_jni_CallVoidMethodV,			//62
dyc_jni_CallVoidMethodA,			//63
dyc_jni_CallNonvirtualObjectMethod,		//64
dyc_jni_CallNonvirtualObjectMethodV,		//65
dyc_jni_CallNonvirtualObjectMethodA,		//66
dyc_jni_CallNonvirtualBooleanMethod,		//67
dyc_jni_CallNonvirtualBooleanMethodV,		//68
dyc_jni_CallNonvirtualBooleanMethodA,		//69
dyc_jni_CallNonvirtualByteMethod,		//70
dyc_jni_CallNonvirtualByteMethodV,		//71
dyc_jni_CallNonvirtualByteMethodA,		//72
dyc_jni_CallNonvirtualCharMethod,		//73
dyc_jni_CallNonvirtualCharMethodV,		//74
dyc_jni_CallNonvirtualCharMethodA,		//75
dyc_jni_CallNonvirtualShortMethod,		//76
dyc_jni_CallNonvirtualShortMethodV,		//77
dyc_jni_CallNonvirtualShortMethodA,		//78
dyc_jni_CallNonvirtualIntMethod,		//79
dyc_jni_CallNonvirtualIntMethodV,		//80
dyc_jni_CallNonvirtualIntMethodA,		//81
dyc_jni_CallNonvirtualLongMethod,		//82
dyc_jni_CallNonvirtualLongMethodV,		//83
dyc_jni_CallNonvirtualLongMethodA,		//84
dyc_jni_CallNonvirtualFloatMethod,		//85
dyc_jni_CallNonvirtualFloatMethodV,		//86
dyc_jni_CallNonvirtualFloatMethodA,		//87
dyc_jni_CallNonvirtualDoubleMethod,		//88
dyc_jni_CallNonvirtualDoubleMethodV,		//89
dyc_jni_CallNonvirtualDoubleMethodA,		//90
dyc_jni_CallNonvirtualVoidMethod,		//91
dyc_jni_CallNonvirtualVoidMethodV,		//92
dyc_jni_CallNonvirtualVoidMethodA,		//93
dyc_jni_GetFieldID,				//94
dyc_jni_GetObjectField,			//95
dyc_jni_GetBooleanField,			//96
dyc_jni_GetByteField,				//97
dyc_jni_GetCharField,				//98
dyc_jni_GetShortField,				//99
dyc_jni_GetIntField,				//100
dyc_jni_GetLongField,				//101
dyc_jni_GetFloatField,				//102
dyc_jni_GetDoubleField,			//103
dyc_jni_SetObjectField,			//104
dyc_jni_SetBooleanField,			//105
dyc_jni_SetByteField,				//106
dyc_jni_SetCharField,				//107
dyc_jni_SetShortField,				//108
dyc_jni_SetIntField,				//109
dyc_jni_SetLongField,				//110
dyc_jni_SetFloatField,				//111
dyc_jni_SetDoubleField,				//112
dyc_jni_GetStaticMethodID,			//113
dyc_jni_CallStaticObjectMethod,		//114
dyc_jni_CallStaticObjectMethodV,		//115
dyc_jni_CallStaticObjectMethodA,		//116
dyc_jni_CallStaticBooleanMethod,		//117
dyc_jni_CallStaticBooleanMethodV,		//118
dyc_jni_CallStaticBooleanMethodA,		//119
dyc_jni_CallStaticByteMethod,			//120
dyc_jni_CallStaticByteMethodV,			//121
dyc_jni_CallStaticByteMethodA,			//122
dyc_jni_CallStaticCharMethod,			//123
dyc_jni_CallStaticCharMethodV,			//124
dyc_jni_CallStaticCharMethodA,			//125
dyc_jni_CallStaticShortMethod,			//126
dyc_jni_CallStaticShortMethodV,		//127
dyc_jni_CallStaticShortMethodA,		//128
dyc_jni_CallStaticIntMethod,			//129
dyc_jni_CallStaticIntMethodV,			//130
dyc_jni_CallStaticIntMethodA,			//131
dyc_jni_CallStaticLongMethod,			//132
dyc_jni_CallStaticLongMethodV,			//133
dyc_jni_CallStaticLongMethodA,			//134
dyc_jni_CallStaticFloatMethod,			//135
dyc_jni_CallStaticFloatMethodV,		//136
dyc_jni_CallStaticFloatMethodA,		//137
dyc_jni_CallStaticDoubleMethod,		//138
dyc_jni_CallStaticDoubleMethodV,		//139
dyc_jni_CallStaticDoubleMethodA,       	//140
dyc_jni_CallStaticVoidMethod,			//141
dyc_jni_CallStaticVoidMethodV,			//142
dyc_jni_CallStaticVoidMethodA,			//143
dyc_jni_GetStaticFieldID,			//144
dyc_jni_GetStaticObjectField,			//145
dyc_jni_GetStaticBooleanField,			//146
dyc_jni_GetStaticByteField,			//147
dyc_jni_GetStaticCharField,			//148
dyc_jni_GetStaticShortField,			//149
dyc_jni_GetStaticIntField,			//150
dyc_jni_GetStaticLongField,			//151
dyc_jni_GetStaticFloatField,			//152
dyc_jni_GetStaticDoubleField,			//153
dyc_jni_SetStaticObjectField,			//154
dyc_jni_SetStaticBooleanField,			//155
dyc_jni_SetStaticByteField,			//156
dyc_jni_SetStaticCharField,			//157
dyc_jni_SetStaticShortField,			//158
dyc_jni_SetStaticIntField,			//159
dyc_jni_SetStaticLongField,			//160
dyc_jni_SetStaticFloatField,			//161
dyc_jni_SetStaticDoubleField,			//162
dyc_jni_NewString,				//163
dyc_jni_GetStringLength,			//164
dyc_jni_GetStringChars,			//165
dyc_jni_ReleaseStringChars,			//166
dyc_jni_NewStringUTF,				//167
dyc_jni_GetStringUTFLength,			//168
dyc_jni_GetStringUTFChars,			//169
dyc_jni_ReleaseStringUTFChars,			//170
dyc_jni_GetArrayLength,			//171
dyc_jni_NewObjectArray,			//172
dyc_jni_GetObjectArrayElement,			//173
dyc_jni_SetObjectArrayElement,			//174
dyc_jni_NewBooleanArray,			//175
dyc_jni_NewByteArray,				//176
dyc_jni_NewCharArray,				//177
dyc_jni_NewShortArray,				//178
dyc_jni_NewIntArray,				//179
dyc_jni_NewLongArray,				//180
dyc_jni_NewFloatArray,				//181
dyc_jni_NewDoubleArray,			//182
dyc_jni_GetBooleanArrayElements,		//183
dyc_jni_GetByteArrayElements,			//184
dyc_jni_GetCharArrayElements,			//185
dyc_jni_GetShortArrayElements,			//186
dyc_jni_GetIntArrayElements,			//187
dyc_jni_GetLongArrayElements,			//188
dyc_jni_GetFloatArrayElements,			//189
dyc_jni_GetDoubleArrayElements,		//190
dyc_jni_ReleaseBooleanArrayElements,		//191
dyc_jni_ReleaseByteArrayElements,		//192
dyc_jni_ReleaseCharArrayElements,		//193
dyc_jni_ReleaseShortArrayElements,		//194
dyc_jni_ReleaseIntArrayElements,		//195
dyc_jni_ReleaseLongArrayElements,		//196
dyc_jni_ReleaseFloatArrayElements,		//197
dyc_jni_ReleaseDoubleArrayElements,		//198
dyc_jni_GetBooleanArrayRegion,			//199
dyc_jni_GetByteArrayRegion,			//200
dyc_jni_GetCharArrayRegion,			//201
dyc_jni_GetShortArrayRegion,			//202
dyc_jni_GetIntArrayRegion,			//203
dyc_jni_GetLongArrayRegion,			//204
dyc_jni_GetFloatArrayRegion,			//205
dyc_jni_GetDoubleArrayRegion,			//206
dyc_jni_SetBooleanArrayRegion,			//207
dyc_jni_SetByteArrayRegion,			//208
dyc_jni_SetCharArrayRegion,			//209
dyc_jni_SetShortArrayRegion,			//210
dyc_jni_SetIntArrayRegion,			//211
dyc_jni_SetLongArrayRegion,			//212
dyc_jni_SetFloatArrayRegion,			//213
dyc_jni_SetDoubleArrayRegion,			//214
dyc_jni_RegisterNatives,			//215
dyc_jni_UnregisterNatives,			//216
dyc_jni_MonitorEnter,				//217
dyc_jni_MonitorExit,				//218
dyc_jni_GetJavaVM,				//219
dyc_jni_GetStringRegion,			//220
dyc_jni_GetStringUTFRegion,			//221
dyc_jni_GetPrimitiveArrayCritical,		//222
dyc_jni_ReleasePrimitiveArrayCritical,		//223
dyc_jni_GetStringCritical,			//224
dyc_jni_ReleaseStringCritical,			//225
dyc_jni_NewWeakGlobalRef,			//226
dyc_jni_DeleteWeakGlobalRef,			//227
dyc_jni_ExceptionCheck,			//228
dyc_jni_NewDirectByteBuffer,			//229
dyc_jni_GetDirectBufferAddress,		//230
dyc_jni_GetDirectBufferCapacity		//231
};

jint JNICALL dyc_jni_DestroyJavaVM(JavaVM *jvm);
jint JNICALL dyc_jni_AttachCurrentThread(JavaVM *jvm,
	void **penv, void *args);
jint JNICALL dyc_jni_DetachCurrentThread(JavaVM *jvm);
jint JNICALL dyc_jni_GetEnv(JavaVM *jvm, void **penv, jint ver);
jint JNICALL dyc_jni_AttachCurrentThreadAsDaemon(JavaVM *jvm,
	void **penv, void *args);

struct JNIInvokeInterface_ dyc_jni_javavm_iface={
NULL,						//0
NULL,						//1
NULL,						//2
dyc_jni_DestroyJavaVM,				//3
dyc_jni_AttachCurrentThread,			//4
dyc_jni_DetachCurrentThread,			//5
dyc_jni_GetEnv,					//6
dyc_jni_AttachCurrentThreadAsDaemon		//7
};


BGBDY_API void *dycJniGetEnv()
{
	static JNIEnv env;
	env=&dyc_jni_iface;
	return((void *)(&env));
}

BGBDY_API void *dycJniGetJavaVM()
{
	static JavaVM vm;
	vm=&dyc_jni_javavm_iface;
	return((void *)(&vm));
}
