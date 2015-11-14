#include <bgbdy.h>
#include <jni.h>

//AH:skip

typedef struct JNINativeInterface_ JNIEnv_t;

extern struct JNINativeInterface_ dyc_jni_iface;
extern struct JNIInvokeInterface_ dyc_jni_javavm_iface;

jobject JNICALL dyc_jni_AllocObject(JNIEnv *env, jclass clazz)
{
	dycClass inf;
	dycObject p;

	inf=(dycClass )clazz;
	p=dycAlloc(inf);
	return((jobject)p);
}


jint JNICALL dyc_jni_GetVersion(JNIEnv *env)
{
	return(JNI_VERSION_1_4);
}


jclass JNICALL dyc_jni_DefineClass(JNIEnv *env, const char *name,
	jobject loader, const jbyte *buf, jsize len)
{
	//use a real JNI...
	return(NULL);
}

jclass JNICALL dyc_jni_FindClass(JNIEnv *env, const char *name)
{
	return((jclass)dycGetClass((char *)name));
}

jclass JNICALL dyc_jni_GetSuperclass(JNIEnv *env, jclass sub)
{
	return((jclass)dycGetClassSuper((dycClass)sub));
}

jboolean JNICALL dyc_jni_IsAssignableFrom(JNIEnv *env,
	jclass sub, jclass sup)
{
	dycClass inf0, inf1;
	inf0=(dycClass)sub; inf1=(dycClass)sup;
	return(dycSubclassP(inf0, inf1));
}

jboolean JNICALL dyc_jni_IsSameObject(JNIEnv *env, jobject obj1, jobject obj2)
{
	return(obj1==obj2);
}

jclass JNICALL dyc_jni_GetObjectClass(JNIEnv *env, jobject obj)
{
	return((jclass)dycGetObjClass((dycObject)obj));
}

jboolean JNICALL dyc_jni_IsInstanceOf(JNIEnv *env, jobject obj, jclass clazz)
{
	return(dycInstanceP((dycObject)obj, (dycClass)clazz));
}


jfieldID JNICALL dyc_jni_GetFieldID(JNIEnv *env, jclass clazz,
	const char *name, const char *sig)
{
	dycClass inf;
	dycSlot id;

	inf=(dycClass )clazz;
 	id=dycIdxSlot(inf, (char *)name);
	return((jfieldID)id);
}

jobject JNICALL dyc_jni_GetObjectField(JNIEnv *env,
	jobject obj, jfieldID fieldID)
{
	void *p; p=dycGetSlotPtr((dycObject)obj, (dycSlot)fieldID);
	return((jobject)(*(void **)p));
}

jboolean JNICALL dyc_jni_GetBooleanField(JNIEnv *env,
	jobject obj, jfieldID fieldID)
{
	void *p; p=dycGetSlotPtr((dycObject)obj, (dycSlot)fieldID);
	return((jboolean)(*(char *)p));
}

jbyte JNICALL dyc_jni_GetByteField(JNIEnv *env,
	jobject obj, jfieldID fieldID)
{
	void *p; p=dycGetSlotPtr((dycObject)obj, (dycSlot)fieldID);
	return((jbyte)(*(char *)p));
}

jchar JNICALL dyc_jni_GetCharField(JNIEnv *env,
	jobject obj, jfieldID fieldID)
{
	void *p; p=dycGetSlotPtr((dycObject)obj, (dycSlot)fieldID);
	return((jchar)(*(wchar_t *)p));
}

jshort JNICALL dyc_jni_GetShortField(JNIEnv *env,
	jobject obj, jfieldID fieldID)
{
	void *p; p=dycGetSlotPtr((dycObject)obj, (dycSlot)fieldID);
	return((jshort)(*(short *)p));
}

jint JNICALL dyc_jni_GetIntField(JNIEnv *env,
	jobject obj, jfieldID fieldID)
{
	void *p; p=dycGetSlotPtr((dycObject)obj, (dycSlot)fieldID);
	return((jint)(*(int *)p));
}

jlong JNICALL dyc_jni_GetLongField(JNIEnv *env,
	jobject obj, jfieldID fieldID)
{
	void *p; p=dycGetSlotPtr((dycObject)obj, (dycSlot)fieldID);
	return((jlong)(*(s64 *)p));
}

jfloat JNICALL dyc_jni_GetFloatField(JNIEnv *env,
	jobject obj, jfieldID fieldID)
{
	void *p; p=dycGetSlotPtr((dycObject)obj, (dycSlot)fieldID);
	return((jfloat)(*(float *)p));
}

jdouble JNICALL dyc_jni_GetDoubleField(JNIEnv *env,
	jobject obj, jfieldID fieldID)
{
	void *p; p=dycGetSlotPtr((dycObject)obj, (dycSlot)fieldID);
	return((jdouble)(*(double *)p));
}

void JNICALL dyc_jni_SetObjectField(JNIEnv *env,
	jobject obj, jfieldID fieldID, jobject val)
{
	void *p; p=dycGetSlotPtr((dycObject)obj, (dycSlot)fieldID);
	*(void **)p=val;
}

void JNICALL dyc_jni_SetBooleanField(JNIEnv *env,
	jobject obj, jfieldID fieldID, jboolean val)
{
	void *p; p=dycGetSlotPtr((dycObject)obj, (dycSlot)fieldID);
	*(char *)p=val;
}

void JNICALL dyc_jni_SetByteField(JNIEnv *env,
	jobject obj, jfieldID fieldID, jbyte val)
{
	void *p; p=dycGetSlotPtr((dycObject)obj, (dycSlot)fieldID);
	*(char *)p=val;
}

void JNICALL dyc_jni_SetCharField(JNIEnv *env,
	jobject obj, jfieldID fieldID, jchar val)
{
	void *p; p=dycGetSlotPtr((dycObject)obj, (dycSlot)fieldID);
	*(wchar_t *)p=val;
}

void JNICALL dyc_jni_SetShortField(JNIEnv *env,
	jobject obj, jfieldID fieldID, jshort val)
{
	void *p; p=dycGetSlotPtr((dycObject)obj, (dycSlot)fieldID);
	*(short *)p=val;
}

void JNICALL dyc_jni_SetIntField(JNIEnv *env,
	jobject obj, jfieldID fieldID, jint val)
{
	void *p; p=dycGetSlotPtr((dycObject)obj, (dycSlot)fieldID);
	*(int *)p=val;
}

void JNICALL dyc_jni_SetLongField(JNIEnv *env,
	jobject obj, jfieldID fieldID, jlong val)
{
	void *p; p=dycGetSlotPtr((dycObject)obj, (dycSlot)fieldID);
	*(s64 *)p=val;
}

void JNICALL dyc_jni_SetFloatField(JNIEnv *env,
	jobject obj, jfieldID fieldID, jfloat val)
{
	void *p; p=dycGetSlotPtr((dycObject)obj, (dycSlot)fieldID);
	*(float *)p=val;
}

void JNICALL dyc_jni_SetDoubleField(JNIEnv *env,
	jobject obj, jfieldID fieldID, jdouble val)
{
	void *p; p=dycGetSlotPtr((dycObject)obj, (dycSlot)fieldID);
	*(double *)p=val;
}


jfieldID JNICALL dyc_jni_GetStaticFieldID(JNIEnv *env,
	jclass clazz, const char *name, const char *sig)
{
	dycClass inf;
	dycSlot id;

	inf=(dycClass )clazz;
 	id=dycIdxSlot(inf, (char *)name);
	return((jfieldID)id);
}

jobject JNICALL dyc_jni_GetStaticObjectField(JNIEnv *env,
	jclass clazz, jfieldID fieldID)
{
	void *p; p=dycGetStaticPtr((dycClass)clazz, (dycSlot)fieldID);
	return((jobject)(*(void **)p));
}

jboolean JNICALL dyc_jni_GetStaticBooleanField(JNIEnv *env,
	jclass clazz, jfieldID fieldID)
{
	void *p; p=dycGetStaticPtr((dycClass)clazz, (dycSlot)fieldID);
	return(*(char *)p);
}

jbyte JNICALL dyc_jni_GetStaticByteField(JNIEnv *env,
	jclass clazz, jfieldID fieldID)
{
	void *p; p=dycGetStaticPtr((dycClass)clazz, (dycSlot)fieldID);
	return(*(char *)p);
}

jchar JNICALL dyc_jni_GetStaticCharField(JNIEnv *env,
	jclass clazz, jfieldID fieldID)
{
	void *p; p=dycGetStaticPtr((dycClass)clazz, (dycSlot)fieldID);
	return(*(wchar_t *)p);
}

jshort JNICALL dyc_jni_GetStaticShortField(JNIEnv *env,
	jclass clazz, jfieldID fieldID)
{
	void *p; p=dycGetStaticPtr((dycClass)clazz, (dycSlot)fieldID);
	return(*(short *)p);
}

jint JNICALL dyc_jni_GetStaticIntField(JNIEnv *env,
	jclass clazz, jfieldID fieldID)
{
	void *p; p=dycGetStaticPtr((dycClass)clazz, (dycSlot)fieldID);
	return(*(int *)p);
}

jlong JNICALL dyc_jni_GetStaticLongField(JNIEnv *env,
	jclass clazz, jfieldID fieldID)
{
	void *p; p=dycGetStaticPtr((dycClass)clazz, (dycSlot)fieldID);
	return(*(long long *)p);
}

jfloat JNICALL dyc_jni_GetStaticFloatField(JNIEnv *env,
	jclass clazz, jfieldID fieldID)
{
	void *p; p=dycGetStaticPtr((dycClass)clazz, (dycSlot)fieldID);
	return(*(float *)p);
}

jdouble JNICALL dyc_jni_GetStaticDoubleField(JNIEnv *env,
	jclass clazz, jfieldID fieldID)
{
	void *p; p=dycGetStaticPtr((dycClass)clazz, (dycSlot)fieldID);
	return(*(double *)p);
}


void JNICALL dyc_jni_SetStaticObjectField(JNIEnv *env,
	jclass clazz, jfieldID fieldID, jobject value)
{
	void *p; p=dycGetStaticPtr((dycClass)clazz, (dycSlot)fieldID);
	*(void **)p=value;
}

void JNICALL dyc_jni_SetStaticBooleanField(JNIEnv *env,
	jclass clazz, jfieldID fieldID, jboolean value)
{
	void *p; p=dycGetStaticPtr((dycClass)clazz, (dycSlot)fieldID);
	*(char *)p=value;
}

void JNICALL dyc_jni_SetStaticByteField(JNIEnv *env,
	jclass clazz, jfieldID fieldID, jbyte value)
{
	void *p; p=dycGetStaticPtr((dycClass)clazz, (dycSlot)fieldID);
	*(char *)p=value;
}

void JNICALL dyc_jni_SetStaticCharField(JNIEnv *env,
	jclass clazz, jfieldID fieldID, jchar value)
{
	void *p; p=dycGetStaticPtr((dycClass)clazz, (dycSlot)fieldID);
	*(wchar_t *)p=value;
}

void JNICALL dyc_jni_SetStaticShortField(JNIEnv *env,
	jclass clazz, jfieldID fieldID, jshort value)
{
	void *p; p=dycGetStaticPtr((dycClass)clazz, (dycSlot)fieldID);
	*(short *)p=value;
}

void JNICALL dyc_jni_SetStaticIntField(JNIEnv *env,
	jclass clazz, jfieldID fieldID, jint value)
{
	void *p; p=dycGetStaticPtr((dycClass)clazz, (dycSlot)fieldID);
	*(int *)p=value;
}

void JNICALL dyc_jni_SetStaticLongField(JNIEnv *env,
	jclass clazz, jfieldID fieldID, jlong value)
{
	void *p; p=dycGetStaticPtr((dycClass)clazz, (dycSlot)fieldID);
	*(long long *)p=value;
}

void JNICALL dyc_jni_SetStaticFloatField(JNIEnv *env,
	jclass clazz, jfieldID fieldID, jfloat value)
{
	void *p; p=dycGetStaticPtr((dycClass)clazz, (dycSlot)fieldID);
	*(float *)p=value;
}

void JNICALL dyc_jni_SetStaticDoubleField(JNIEnv *env,
	jclass clazz, jfieldID fieldID, jdouble value)
{
	void *p; p=dycGetStaticPtr((dycClass)clazz, (dycSlot)fieldID);
	*(double *)p=value;
}

jstring JNICALL dyc_jni_NewString(JNIEnv *env,
	const jchar *unicode, jsize len)
{
	u16 *buf;
	int i;

	buf=gcralloc((len+1)*sizeof(u16));
	for(i=0; i<len; i++)buf[i]=unicode[i];
	buf[i]=0;
	return((jstring)dywstring(buf));

//	return(BJVM_NewStringWSz((short *)unicode, len));
}

jsize JNICALL dyc_jni_GetStringLength(JNIEnv *env, jstring str)
{
	return(strlen(dystringv((dyt)str)));
}

const jchar *JNICALL dyc_jni_GetStringChars(JNIEnv *env,
	jstring str, jboolean *isCopy)
{
	if(isCopy)*isCopy=0;
	return(dywstringv((dyt)str));
}

void JNICALL dyc_jni_ReleaseStringChars(JNIEnv *env, jstring str, const jchar *chars)
{
//	gcfree((void *)chars);
}

jstring JNICALL dyc_jni_NewStringUTF(JNIEnv *env, const char *utf)
{
	return((jstring)dystring((char *)utf));
}

jsize JNICALL dyc_jni_GetStringUTFLength(JNIEnv *env, jstring str)
{
	u16 *s;
	int i;

	s=dywstringv((dyt)str);
	i=0;
	while(*s)
	{
		if(*s>=4096)i+=3;
		else if(*s>=128)i+=2;
		else i++;
		s++;
	}

	return(i);
}

const char* JNICALL dyc_jni_GetStringUTFChars(JNIEnv *env,
	jstring str, jboolean *isCopy)
{
	if(isCopy)*isCopy=0;
	return(dystringv((dyt)str));
}

void JNICALL dyc_jni_ReleaseStringUTFChars(JNIEnv *env, jstring str, const char* chars)
{
//	gcfree((char *)chars);
}

void JNICALL dyc_jni_GetStringRegion(JNIEnv *env,
	jstring str, jsize start, jsize len, jchar *buf)
{
	u16 *s;
	int i;

	s=dywstringv((dyt)str);
	for(i=0; i<len; i++)buf[i]=s[i+start];
}

void JNICALL dyc_jni_GetStringUTFRegion(JNIEnv *env,
	jstring str, jsize start, jsize len, char *buf)
{
	u16 *s;
	char *t;
	int i;

	s=dywstringv((dyt)str);
	t=buf;

	for(i=0; i<len; i++)BGBGC_EmitChar(&t, s[i+start]);
	*t++=0;
}


jobjectArray JNICALL dyc_jni_NewObjectArray(JNIEnv *env,
	jsize len, jclass clazz, jobject init)
{
	dycClass inf;

	inf=(dycClass)clazz;	
	return((jobjectArray)dycNewArray(dycGetClassName(inf), len));
}

jobject JNICALL dyc_jni_GetObjectArrayElement(JNIEnv *env,
	jobjectArray array, jsize index)
	{ return((jobject)dycGetArrayr((dycArray)array, index)); }
void JNICALL dyc_jni_SetObjectArrayElement(JNIEnv *env,
	jobjectArray array, jsize index, jobject val)
	{ dycSetArrayr((dycArray)array, index, (dyt)val); }

jbooleanArray JNICALL dyc_jni_NewBooleanArray(JNIEnv *env, jsize len)
	{ return((jbooleanArray)dycNewArray("b", len)); }
jbyteArray JNICALL dyc_jni_NewByteArray(JNIEnv *env, jsize len)
	{ return((jbyteArray)dycNewArray("h", len)); }
jcharArray JNICALL dyc_jni_NewCharArray(JNIEnv *env, jsize len)
	{ return((jcharArray)dycNewArray("w", len)); }
jshortArray JNICALL dyc_jni_NewShortArray(JNIEnv *env, jsize len)
	{ return((jshortArray)dycNewArray("s", len)); }
jintArray JNICALL dyc_jni_NewIntArray(JNIEnv *env, jsize len)
	{ return((jintArray)dycNewArray("i", len)); }
jlongArray JNICALL dyc_jni_NewLongArray(JNIEnv *env, jsize len)
	{ return((jlongArray)dycNewArray("x", len)); }
jfloatArray JNICALL dyc_jni_NewFloatArray(JNIEnv *env, jsize len)
	{ return((jfloatArray)dycNewArray("f", len)); }
jdoubleArray JNICALL dyc_jni_NewDoubleArray(JNIEnv *env, jsize len)
	{ return((jdoubleArray)dycNewArray("d", len)); }

jboolean* JNICALL dyc_jni_GetBooleanArrayElements(JNIEnv *env, jbooleanArray array, jboolean *isCopy)
	{ *isCopy=0; return(dycArrayV((dycArray)array)); }

jbyte* JNICALL dyc_jni_GetByteArrayElements(JNIEnv *env,
		jbyteArray array, jboolean *isCopy)
	{ *isCopy=0; return(dycArrayV((dycArray)array)); }
jchar* JNICALL dyc_jni_GetCharArrayElements(JNIEnv *env,
		jcharArray array, jboolean *isCopy)
	{ *isCopy=0; return(dycArrayV((dycArray)array)); }
jshort* JNICALL dyc_jni_GetShortArrayElements(JNIEnv *env,
		jshortArray array, jboolean *isCopy)
	{ *isCopy=0; return(dycArrayV((dycArray)array)); }
jint* JNICALL dyc_jni_GetIntArrayElements(JNIEnv *env,
		jintArray array, jboolean *isCopy)
	{ *isCopy=0; return(dycArrayV((dycArray)array)); }
jlong* JNICALL dyc_jni_GetLongArrayElements(JNIEnv *env,
		jlongArray array, jboolean *isCopy)
	{ *isCopy=0; return(dycArrayV((dycArray)array)); }
jfloat* JNICALL dyc_jni_GetFloatArrayElements(JNIEnv *env,
		jfloatArray array, jboolean *isCopy)
	{ *isCopy=0; return(dycArrayV((dycArray)array)); }
jdouble* JNICALL dyc_jni_GetDoubleArrayElements(JNIEnv *env,
		jdoubleArray array, jboolean *isCopy)
	{ *isCopy=0; return(dycArrayV((dycArray)array)); }

void JNICALL dyc_jni_ReleaseBooleanArrayElements(JNIEnv *env, jbooleanArray array, jboolean *elems, jint mode) {}
void JNICALL dyc_jni_ReleaseByteArrayElements(JNIEnv *env, jbyteArray array, jbyte *elems, jint mode) {}
void JNICALL dyc_jni_ReleaseCharArrayElements(JNIEnv *env, jcharArray array, jchar *elems, jint mode) {}
void JNICALL dyc_jni_ReleaseShortArrayElements(JNIEnv *env, jshortArray array, jshort *elems, jint mode) {}
void JNICALL dyc_jni_ReleaseIntArrayElements(JNIEnv *env, jintArray array, jint *elems, jint mode) {}
void JNICALL dyc_jni_ReleaseLongArrayElements(JNIEnv *env, jlongArray array, jlong *elems, jint mode) {}
void JNICALL dyc_jni_ReleaseFloatArrayElements(JNIEnv *env, jfloatArray array, jfloat *elems, jint mode) {}
void JNICALL dyc_jni_ReleaseDoubleArrayElements(JNIEnv *env, jdoubleArray array, jdouble *elems, jint mode) {}

void JNICALL dyc_jni_GetBooleanArrayRegion(JNIEnv *env,
		jbooleanArray array, jsize start, jsize len, jboolean *buf)
	{ memcpy(buf, ((byte *)(dycArrayV((dycArray)array)))+start, len); }
void JNICALL dyc_jni_GetByteArrayRegion(JNIEnv *env,
		jbyteArray array, jsize start, jsize len, jbyte *buf)
	{ memcpy(buf, ((char *)(dycArrayV((dycArray)array)))+start, len); }
void JNICALL dyc_jni_GetCharArrayRegion(JNIEnv *env,
		jcharArray array, jsize start, jsize len, jchar *buf)
	{ memcpy(buf, ((wchar_t *)(dycArrayV((dycArray)array)))+start, len*sizeof(wchar_t)); }
void JNICALL dyc_jni_GetShortArrayRegion(JNIEnv *env,
		jshortArray array, jsize start, jsize len, jshort *buf)
	{ memcpy(buf, ((short *)(dycArrayV((dycArray)array)))+start, len*sizeof(short)); }
void JNICALL dyc_jni_GetIntArrayRegion(JNIEnv *env,
		jintArray array, jsize start, jsize len, jint *buf)
	{ memcpy(buf, ((int *)(dycArrayV((dycArray)array)))+start, len*sizeof(int)); }
void JNICALL dyc_jni_GetLongArrayRegion(JNIEnv *env,
		jlongArray array, jsize start, jsize len, jlong *buf)
	{ memcpy(buf, ((s64 *)(dycArrayV((dycArray)array)))+start, len*sizeof(s64)); }
void JNICALL dyc_jni_GetFloatArrayRegion(JNIEnv *env,
		jfloatArray array, jsize start, jsize len, jfloat *buf)
	{ memcpy(buf, ((float *)(dycArrayV((dycArray)array)))+start, len*sizeof(float)); }
void JNICALL dyc_jni_GetDoubleArrayRegion(JNIEnv *env,
		jdoubleArray array, jsize start, jsize len, jdouble *buf)
	{ memcpy(buf, ((double *)(dycArrayV((dycArray)array)))+start, len*sizeof(double)); }

void JNICALL dyc_jni_SetBooleanArrayRegion(JNIEnv *env,
		jbooleanArray array, jsize start, jsize len, jboolean *buf)
	{ memcpy(((byte *)(dycArrayV((dycArray)array)))+start, buf, len); }
void JNICALL dyc_jni_SetByteArrayRegion(JNIEnv *env,
		jbyteArray array, jsize start, jsize len, jbyte *buf)
	{ memcpy(((char *)(dycArrayV((dycArray)array)))+start, buf, len); }
void JNICALL dyc_jni_SetCharArrayRegion(JNIEnv *env,
		jcharArray array, jsize start, jsize len, jchar *buf)
	{ memcpy(((wchar_t *)(dycArrayV((dycArray)array)))+start, buf, len*sizeof(wchar_t)); }
void JNICALL dyc_jni_SetShortArrayRegion(JNIEnv *env,
		jshortArray array, jsize start, jsize len, jshort *buf)
	{ memcpy(((short *)(dycArrayV((dycArray)array)))+start, buf, len*sizeof(short)); }
void JNICALL dyc_jni_SetIntArrayRegion(JNIEnv *env,
		jintArray array, jsize start, jsize len, jint *buf)
	{ memcpy(((int *)(dycArrayV((dycArray)array)))+start, buf, len*sizeof(int)); }
void JNICALL dyc_jni_SetLongArrayRegion(JNIEnv *env,
		jlongArray array, jsize start, jsize len, jlong *buf)
	{ memcpy(((s64 *)(dycArrayV((dycArray)array)))+start, buf, len*sizeof(s64)); }
void JNICALL dyc_jni_SetFloatArrayRegion(JNIEnv *env,
		jfloatArray array, jsize start, jsize len, jfloat *buf)
	{ memcpy(((float *)(dycArrayV((dycArray)array)))+start, buf, len*sizeof(float)); }
void JNICALL dyc_jni_SetDoubleArrayRegion(JNIEnv *env,
		jdoubleArray array, jsize start, jsize len, jdouble *buf)
	{ memcpy(((double *)(dycArrayV((dycArray)array)))+start, buf, len*sizeof(double)); }

jsize JNICALL dyc_jni_GetArrayLength(JNIEnv *env, jarray array)
	{ return(dycArraySz((dycArray)array)); }

void* JNICALL dyc_jni_GetPrimitiveArrayCritical(JNIEnv *env,
		jarray array, jboolean *isCopy)
	{*isCopy=0; return(dycArrayV((dycArray)array)); }
void JNICALL dyc_jni_ReleasePrimitiveArrayCritical(JNIEnv *env,
		jarray array, void* carray, jint mode)
	{ /* no-op*/ }

const jchar* JNICALL dyc_jni_GetStringCritical(JNIEnv *env,
		jstring s, jboolean* isCopy)
	{ *isCopy=0; return(dywstringv((dyt)s)); }

void JNICALL dyc_jni_ReleaseStringCritical(JNIEnv *env,
		jstring s, const jchar* cstr)
	{ /* no-op*/ }

jweak JNICALL dyc_jni_NewWeakGlobalRef(JNIEnv *env, jobject obj)
{
	dyt *p;

	p=gcweak(sizeof(dyt));
	*p=(dyt)obj;
	return((jweak)p);
}

void JNICALL dyc_jni_DeleteWeakGlobalRef(JNIEnv *env, jweak obj)
	{ gcfree((void *)obj); }


jmethodID JNICALL dyc_jni_GetMethodID(JNIEnv *env,
	jclass clazz, const char *name, const char *sig)
{
	dycMethod midx;

	midx=dycIdxMethod((dycClass)clazz, (char *)name, (char *)sig);
	return((jmethodID)midx);
}

jobject JNICALL dyc_jni_NewObjectA(JNIEnv *env,
	jclass clazz, jmethodID methodID, jvalue *args)
{
	dycClass inf;
	jobject obj;

	inf=(dycClass)clazz;
	obj=(jobject)dycAlloc(inf);
	dycCallU((dycObject)obj, (dycMethod)methodID, (void **)args);
	return(obj);
}

jobject JNICALL dyc_jni_NewObjectV(JNIEnv *env,
	jclass clazz, jmethodID methodID, va_list args)
{
	dycClass inf;
	jobject obj;

	inf=(dycClass)clazz;
	obj=(jobject)dycAlloc(inf);
	dycCallV((dycObject)obj, (dycMethod)methodID, args);
	return(obj);
}

jobject JNICALL dyc_jni_NewObject(JNIEnv *env,
	jclass clazz, jmethodID methodID, ...)
{
	jobject obj;
	va_list lst;

	va_start(lst, methodID);
	obj=dyc_jni_NewObjectV(env, clazz, methodID, lst);
	va_end(lst);
	return(obj);
}

jobject JNICALL dyc_jni_CallObjectMethodA(JNIEnv *env,
	jobject obj, jmethodID methodID, jvalue *args)
{
	void *p;
	p=dycCallU((dycObject)obj,
		(dycMethod)methodID, (void **)args);
	return(*(jobject *)p);
}

jboolean JNICALL dyc_jni_CallBooleanMethodA(JNIEnv *env,
	jobject obj, jmethodID methodID, jvalue * args)
{
	void *p;
	p=dycCallU((dycObject)obj,
		(dycMethod)methodID, (void **)args);
	return(*(char *)p);
}

jbyte JNICALL dyc_jni_CallByteMethodA(JNIEnv *env,
	jobject obj, jmethodID methodID, jvalue *args)
{
	void *p;
	p=dycCallU((dycObject)obj,
		(dycMethod)methodID, (void **)args);
	return(*(char *)p);
}

jchar JNICALL dyc_jni_CallCharMethodA(JNIEnv *env,
	jobject obj, jmethodID methodID, jvalue *args)
{
	void *p;
	p=dycCallU((dycObject)obj,
		(dycMethod)methodID, (void **)args);
	return(*(wchar_t *)p);
}

jshort JNICALL dyc_jni_CallShortMethodA(JNIEnv *env,
	jobject obj, jmethodID methodID, jvalue *args)
{
	void *p;
	p=dycCallU((dycObject)obj,
		(dycMethod)methodID, (void **)args);
	return(*(short *)p);
}

jint JNICALL dyc_jni_CallIntMethodA(JNIEnv *env,
	jobject obj, jmethodID methodID, jvalue *args)
{
	void *p;
	p=dycCallU((dycObject)obj,
		(dycMethod)methodID, (void **)args);
	return(*(int *)p);
}

jlong JNICALL dyc_jni_CallLongMethodA(JNIEnv *env,
	jobject obj, jmethodID methodID, jvalue *args)
{
	void *p;
	p=dycCallU((dycObject)obj,
		(dycMethod)methodID, (void **)args);
	return(*(s64 *)p);
}

jfloat JNICALL dyc_jni_CallFloatMethodA(JNIEnv *env,
	jobject obj, jmethodID methodID, jvalue *args)
{
	void *p;
	p=dycCallU((dycObject)obj,
		(dycMethod)methodID, (void **)args);
	return(*(float *)p);
}

jdouble JNICALL dyc_jni_CallDoubleMethodA(JNIEnv *env,
	jobject obj, jmethodID methodID, jvalue *args)
{
	void *p;
	p=dycCallU((dycObject)obj,
		(dycMethod)methodID, (void **)args);
	return(*(double *)p);
}

void JNICALL dyc_jni_CallVoidMethodA(JNIEnv *env,
	jobject obj, jmethodID methodID, jvalue * args)
{
	dycCallU((dycObject)obj,
		(dycMethod)methodID, (void **)args);
}


jobject JNICALL dyc_jni_CallObjectMethodV(JNIEnv *env,
	jobject obj, jmethodID methodID, va_list args)
{
	void *p;
	p=dycCallV((dycObject)obj, (dycMethod)methodID, args);
	return(*(jobject *)p);
}

jboolean JNICALL dyc_jni_CallBooleanMethodV(JNIEnv *env,
	jobject obj, jmethodID methodID, va_list args)
{
	void *p;
	p=dycCallV((dycObject)obj, (dycMethod)methodID, args);
	return(*(char *)p);
}

jbyte JNICALL dyc_jni_CallByteMethodV(JNIEnv *env,
	jobject obj, jmethodID methodID, va_list args)
{
	void *p;
	p=dycCallV((dycObject)obj, (dycMethod)methodID, args);
	return(*(char *)p);
}

jchar JNICALL dyc_jni_CallCharMethodV(JNIEnv *env,
	jobject obj, jmethodID methodID, va_list args)
{
	void *p;
	p=dycCallV((dycObject)obj, (dycMethod)methodID, args);
	return(*(wchar_t *)p);
}

jshort JNICALL dyc_jni_CallShortMethodV(JNIEnv *env,
	jobject obj, jmethodID methodID, va_list args)
{
	void *p;
	p=dycCallV((dycObject)obj, (dycMethod)methodID, args);
	return(*(short *)p);
}

jint JNICALL dyc_jni_CallIntMethodV(JNIEnv *env,
	jobject obj, jmethodID methodID, va_list args)
{
	void *p;
	p=dycCallV((dycObject)obj, (dycMethod)methodID, args);
	return(*(int *)p);
}

jlong JNICALL dyc_jni_CallLongMethodV(JNIEnv *env,
	jobject obj, jmethodID methodID, va_list args)
{
	void *p;
	p=dycCallV((dycObject)obj, (dycMethod)methodID, args);
	return(*(s64 *)p);
}

jfloat JNICALL dyc_jni_CallFloatMethodV(JNIEnv *env,
	jobject obj, jmethodID methodID, va_list args)
{
	void *p;
	p=dycCallV((dycObject)obj, (dycMethod)methodID, args);
	return(*(float *)p);
}

jdouble JNICALL dyc_jni_CallDoubleMethodV(JNIEnv *env,
	jobject obj, jmethodID methodID, va_list args)
{
	void *p;
	p=dycCallV((dycObject)obj, (dycMethod)methodID, args);
	return(*(double *)p);
}

void JNICALL dyc_jni_CallVoidMethodV(JNIEnv *env,
	jobject obj, jmethodID methodID, va_list args)
{
	dycCallV((dycObject)obj, (dycMethod)methodID, args);
}


jobject JNICALL dyc_jni_CallObjectMethod(JNIEnv *env,
	jobject obj, jmethodID methodID, ...)
{
	va_list lst;
	void *p;
	va_start(lst, methodID);
	p=dycCallV((dycObject)obj, (dycMethod)methodID, lst);
	va_end(lst);
	return(*(jobject *)p);
}

jboolean JNICALL dyc_jni_CallBooleanMethod(JNIEnv *env,
	jobject obj, jmethodID methodID, ...)
{
	va_list lst;
	void *p;
	va_start(lst, methodID);
	p=dycCallV((dycObject)obj, (dycMethod)methodID, lst);
	va_end(lst);
	return(*(char *)p);
}

jbyte JNICALL dyc_jni_CallByteMethod(JNIEnv *env,
	jobject obj, jmethodID methodID, ...)
{
	va_list lst;
	void *p;
	va_start(lst, methodID);
	p=dycCallV((dycObject)obj, (dycMethod)methodID, lst);
	va_end(lst);
	return(*(char *)p);
}

jchar JNICALL dyc_jni_CallCharMethod(JNIEnv *env,
	jobject obj, jmethodID methodID, ...)
{
	va_list lst;
	void *p;
	va_start(lst, methodID);
	p=dycCallV((dycObject)obj, (dycMethod)methodID, lst);
	va_end(lst);
	return(*(wchar_t *)p);
}

jshort JNICALL dyc_jni_CallShortMethod(JNIEnv *env,
	jobject obj, jmethodID methodID, ...)
{
	va_list lst;
	void *p;
	va_start(lst, methodID);
	p=dycCallV((dycObject)obj, (dycMethod)methodID, lst);
	va_end(lst);
	return(*(short *)p);
}

jint JNICALL dyc_jni_CallIntMethod(JNIEnv *env,
	jobject obj, jmethodID methodID, ...)
{
	va_list lst;
	void *p;
	va_start(lst, methodID);
	p=dycCallV((dycObject)obj, (dycMethod)methodID, lst);
	va_end(lst);
	return(*(int *)p);
}

jlong JNICALL dyc_jni_CallLongMethod(JNIEnv *env,
	jobject obj, jmethodID methodID, ...)
{
	va_list lst;
	void *p;
	va_start(lst, methodID);
	p=dycCallV((dycObject)obj, (dycMethod)methodID, lst);
	va_end(lst);
	return(*(s64 *)p);
}

jfloat JNICALL dyc_jni_CallFloatMethod(JNIEnv *env,
	jobject obj, jmethodID methodID, ...)
{
	va_list lst;
	void *p;
	va_start(lst, methodID);
	p=dycCallV((dycObject)obj, (dycMethod)methodID, lst);
	va_end(lst);
	return(*(float *)p);
}

jdouble JNICALL dyc_jni_CallDoubleMethod(JNIEnv *env,
	jobject obj, jmethodID methodID, ...)
{
	va_list lst;
	void *p;
	va_start(lst, methodID);
	p=dycCallV((dycObject)obj, (dycMethod)methodID, lst);
	va_end(lst);
	return(*(double *)p);
}

void JNICALL dyc_jni_CallVoidMethod(JNIEnv *env,
	jobject obj, jmethodID methodID, ...)
{
	va_list lst;
	va_start(lst, methodID);
	dycCallV((dycObject)obj, (dycMethod)methodID, lst);
	va_end(lst);
}

jmethodID JNICALL dyc_jni_GetStaticMethodID(JNIEnv *env,
	jclass clazz, const char *name, const char *sig)
{
	dycMethod midx;
	midx=dycIdxMethod((dycClass)clazz, (char *)name, (char *)sig);
	return((jmethodID)midx);
}

jobject JNICALL dyc_jni_CallStaticObjectMethodA(JNIEnv *env,
	jclass clazz, jmethodID methodID, jvalue *args)
{
	void *p;
	p=dycCallStaticU((dycClass)clazz, (dycMethod)methodID, (void **)args);
	return(*(jobject *)p);
}

jboolean JNICALL dyc_jni_CallStaticBooleanMethodA(JNIEnv *env,
	jclass clazz, jmethodID methodID, jvalue *args)
{
	void *p;
	p=dycCallStaticU((dycClass)clazz, (dycMethod)methodID, (void **)args);
	return(*(char *)p);
}

jbyte JNICALL dyc_jni_CallStaticByteMethodA(JNIEnv *env,
	jclass clazz, jmethodID methodID, jvalue *args)
{
	void *p;
	p=dycCallStaticU((dycClass)clazz, (dycMethod)methodID, (void **)args);
	return(*(char *)p);
}

jchar JNICALL dyc_jni_CallStaticCharMethodA(JNIEnv *env,
	jclass clazz, jmethodID methodID, jvalue *args)
{
	void *p;
	p=dycCallStaticU((dycClass)clazz, (dycMethod)methodID, (void **)args);
	return(*(wchar_t *)p);
}

jshort JNICALL dyc_jni_CallStaticShortMethodA(JNIEnv *env,
	jclass clazz, jmethodID methodID, jvalue *args)
{
	void *p;
	p=dycCallStaticU((dycClass)clazz, (dycMethod)methodID, (void **)args);
	return(*(short *)p);
}

jint JNICALL dyc_jni_CallStaticIntMethodA(JNIEnv *env,
	jclass clazz, jmethodID methodID, jvalue *args)
{
	void *p;
	p=dycCallStaticU((dycClass)clazz, (dycMethod)methodID, (void **)args);
	return(*(int *)p);
}

jlong JNICALL dyc_jni_CallStaticLongMethodA(JNIEnv *env,
	jclass clazz, jmethodID methodID, jvalue *args)
{
	void *p;
	p=dycCallStaticU((dycClass)clazz, (dycMethod)methodID, (void **)args);
	return(*(s64 *)p);
}

jfloat JNICALL dyc_jni_CallStaticFloatMethodA(JNIEnv *env,
	jclass clazz, jmethodID methodID, jvalue *args)
{
	void *p;
	p=dycCallStaticU((dycClass)clazz, (dycMethod)methodID, (void **)args);
	return(*(float *)p);
}

jdouble JNICALL dyc_jni_CallStaticDoubleMethodA(JNIEnv *env,
	jclass clazz, jmethodID methodID, jvalue *args)
{
	void *p;
	p=dycCallStaticU((dycClass)clazz, (dycMethod)methodID, (void **)args);
	return(*(double *)p);
}

void JNICALL dyc_jni_CallStaticVoidMethodA(JNIEnv *env,
	jclass cls, jmethodID methodID, jvalue * args)
{
	dycCallStaticU((dycClass)cls, (dycMethod)methodID, (void **)args);
}


jobject JNICALL dyc_jni_CallStaticObjectMethodV(JNIEnv *env,
	jclass clazz, jmethodID methodID, va_list args)
{
	void *p;
	p=dycCallStaticV((dycClass)clazz, (dycMethod)methodID, args);
	return(*(jobject *)p);
}

jboolean JNICALL dyc_jni_CallStaticBooleanMethodV(JNIEnv *env,
	jclass clazz, jmethodID methodID, va_list args)
{
	void *p;
	p=dycCallStaticV((dycClass)clazz, (dycMethod)methodID, args);
	return(*(char *)p);
}

jbyte JNICALL dyc_jni_CallStaticByteMethodV(JNIEnv *env,
	jclass clazz, jmethodID methodID, va_list args)
{
	void *p;
	p=dycCallStaticV((dycClass)clazz, (dycMethod)methodID, args);
	return(*(char *)p);
}

jchar JNICALL dyc_jni_CallStaticCharMethodV(JNIEnv *env,
	jclass clazz, jmethodID methodID, va_list args)
{
	void *p;
	p=dycCallStaticV((dycClass)clazz, (dycMethod)methodID, args);
	return(*(wchar_t *)p);
}

jshort JNICALL dyc_jni_CallStaticShortMethodV(JNIEnv *env,
	jclass clazz, jmethodID methodID, va_list args)
{
	void *p;
	p=dycCallStaticV((dycClass)clazz, (dycMethod)methodID, args);
	return(*(short *)p);
}

jint JNICALL dyc_jni_CallStaticIntMethodV(JNIEnv *env,
	jclass clazz, jmethodID methodID, va_list args)
{
	void *p;
	p=dycCallStaticV((dycClass)clazz, (dycMethod)methodID, args);
	return(*(int *)p);
}

jlong JNICALL dyc_jni_CallStaticLongMethodV(JNIEnv *env,
	jclass clazz, jmethodID methodID, va_list args)
{
	void *p;
	p=dycCallStaticV((dycClass)clazz, (dycMethod)methodID, args);
	return(*(s64 *)p);
}

jfloat JNICALL dyc_jni_CallStaticFloatMethodV(JNIEnv *env,
	jclass clazz, jmethodID methodID, va_list args)
{
	void *p;
	p=dycCallStaticV((dycClass)clazz, (dycMethod)methodID, args);
	return(*(float *)p);
}

jdouble JNICALL dyc_jni_CallStaticDoubleMethodV(JNIEnv *env,
	jclass clazz, jmethodID methodID, va_list args)
{
	void *p;
	p=dycCallStaticV((dycClass)clazz, (dycMethod)methodID, args);
	return(*(double *)p);
}

void JNICALL dyc_jni_CallStaticVoidMethodV(JNIEnv *env,
	jclass cls, jmethodID methodID, va_list args)
{
	dycCallStaticV((dycClass)cls, (dycMethod)methodID, args);
}


jobject JNICALL dyc_jni_CallStaticObjectMethod(JNIEnv *env,
	jclass clazz, jmethodID methodID, ...)
{
	va_list lst;
	void *p;
	va_start(lst, methodID);
	p=dycCallStaticV((dycClass)clazz, (dycMethod)methodID, lst);
	va_end(lst);
	return(*(jobject *)p);
}

jboolean JNICALL dyc_jni_CallStaticBooleanMethod(JNIEnv *env,
	jclass clazz, jmethodID methodID, ...)
{
	va_list lst;
	void *p;
	va_start(lst, methodID);
	p=dycCallStaticV((dycClass)clazz, (dycMethod)methodID, lst);
	va_end(lst);
	return(*(char *)p);
}

jbyte JNICALL dyc_jni_CallStaticByteMethod(JNIEnv *env,
	jclass clazz, jmethodID methodID, ...)
{
	va_list lst;
	void *p;
	va_start(lst, methodID);
	p=dycCallStaticV((dycClass)clazz, (dycMethod)methodID, lst);
	va_end(lst);
	return(*(char *)p);
}

jchar JNICALL dyc_jni_CallStaticCharMethod(JNIEnv *env,
	jclass clazz, jmethodID methodID, ...)
{
	va_list lst;
	void *p;
	va_start(lst, methodID);
	p=dycCallStaticV((dycClass)clazz, (dycMethod)methodID, lst);
	va_end(lst);
	return(*(wchar_t *)p);
}

jshort JNICALL dyc_jni_CallStaticShortMethod(JNIEnv *env,
	jclass clazz, jmethodID methodID, ...)
{
	va_list lst;
	void *p;
	va_start(lst, methodID);
	p=dycCallStaticV((dycClass)clazz, (dycMethod)methodID, lst);
	va_end(lst);
	return(*(short *)p);
}

jint JNICALL dyc_jni_CallStaticIntMethod(JNIEnv *env,
	jclass clazz, jmethodID methodID, ...)
{
	va_list lst;
	void *p;
	va_start(lst, methodID);
	p=dycCallStaticV((dycClass)clazz, (dycMethod)methodID, lst);
	va_end(lst);
	return(*(int *)p);
}

jlong JNICALL dyc_jni_CallStaticLongMethod(JNIEnv *env,
	jclass clazz, jmethodID methodID, ...)
{
	va_list lst;
	void *p;
	va_start(lst, methodID);
	p=dycCallStaticV((dycClass)clazz, (dycMethod)methodID, lst);
	va_end(lst);
	return(*(s64 *)p);
}

jfloat JNICALL dyc_jni_CallStaticFloatMethod(JNIEnv *env,
	jclass clazz, jmethodID methodID, ...)
{
	va_list lst;
	void *p;
	va_start(lst, methodID);
	p=dycCallStaticV((dycClass)clazz, (dycMethod)methodID, lst);
	va_end(lst);
	return(*(float *)p);
}

jdouble JNICALL dyc_jni_CallStaticDoubleMethod(JNIEnv *env,
	jclass clazz, jmethodID methodID, ...)
{
	va_list lst;
	void *p;
	va_start(lst, methodID);
	p=dycCallStaticV((dycClass)clazz, (dycMethod)methodID, lst);
	va_end(lst);
	return(*(double *)p);
}

void JNICALL dyc_jni_CallStaticVoidMethod(JNIEnv *env,
	jclass cls, jmethodID methodID, ...)
{
	va_list lst;
	va_start(lst, methodID);
	dycCallStaticV((dycClass)cls, (dycMethod)methodID, lst);
	va_end(lst);
}


jobject JNICALL dyc_jni_CallNonvirtualObjectMethodA(JNIEnv *env,
	jobject obj, jclass clazz, jmethodID methodID, jvalue *args)
{
	void *p;
	p=dycCallU((dycObject)obj,
		(dycMethod)methodID, (void **)args);
	return(*(jobject *)p);
}

jboolean JNICALL dyc_jni_CallNonvirtualBooleanMethodA(JNIEnv *env,
	jobject obj, jclass clazz, jmethodID methodID, jvalue *args)
{
	void *p;
	p=dycCallU((dycObject)obj,
		(dycMethod)methodID, (void **)args);
	return(*(char *)p);
}

jbyte JNICALL dyc_jni_CallNonvirtualByteMethodA(JNIEnv *env,
	jobject obj, jclass clazz, jmethodID methodID, jvalue *args)
{
	void *p;
	p=dycCallU((dycObject)obj,
		(dycMethod)methodID, (void **)args);
	return(*(char *)p);
}

jchar JNICALL dyc_jni_CallNonvirtualCharMethodA(JNIEnv *env,
	jobject obj, jclass clazz, jmethodID methodID, jvalue *args)
{
	void *p;
	p=dycCallU((dycObject)obj,
		(dycMethod)methodID, (void **)args);
	return(*(wchar_t *)p);
}

jshort JNICALL dyc_jni_CallNonvirtualShortMethodA(JNIEnv *env,
	jobject obj, jclass clazz, jmethodID methodID, jvalue *args)
{
	void *p;
	p=dycCallU((dycObject)obj,
		(dycMethod)methodID, (void **)args);
	return(*(short *)p);
}

jint JNICALL dyc_jni_CallNonvirtualIntMethodA(JNIEnv *env,
	jobject obj, jclass clazz, jmethodID methodID, jvalue *args)
{
	void *p;
	p=dycCallU((dycObject)obj,
		(dycMethod)methodID, (void **)args);
	return(*(int *)p);
}

jlong JNICALL dyc_jni_CallNonvirtualLongMethodA(JNIEnv *env,
	jobject obj, jclass clazz, jmethodID methodID, jvalue *args)
{
	void *p;
	p=dycCallU((dycObject)obj,
		(dycMethod)methodID, (void **)args);
	return(*(long long *)p);
}

jfloat JNICALL dyc_jni_CallNonvirtualFloatMethodA(JNIEnv *env,
	jobject obj, jclass clazz, jmethodID methodID, jvalue *args)
{
	void *p;
	p=dycCallU((dycObject)obj,
		(dycMethod)methodID, (void **)args);
	return(*(float *)p);
}

jdouble JNICALL dyc_jni_CallNonvirtualDoubleMethodA(JNIEnv *env,
	jobject obj, jclass clazz, jmethodID methodID, jvalue *args)
{
	void *p;
	p=dycCallU((dycObject)obj,
		(dycMethod)methodID, (void **)args);
	return(*(double *)p);
}

void JNICALL dyc_jni_CallNonvirtualVoidMethodA(JNIEnv *env,
	jobject obj, jclass clazz, jmethodID methodID, jvalue * args)
{
	dycCallU((dycObject)obj,
		(dycMethod)methodID, (void **)args);
}


jobject JNICALL dyc_jni_CallNonvirtualObjectMethodV(JNIEnv *env,
	jobject obj, jclass clazz, jmethodID methodID, va_list args)
{
	void *p;
	p=dycCallV((dycObject)obj, (dycMethod)methodID, args);
	return(*(jobject *)p);
}

jboolean JNICALL dyc_jni_CallNonvirtualBooleanMethodV(JNIEnv *env,
	jobject obj, jclass clazz, jmethodID methodID, va_list args)
{
	void *p;
	p=dycCallV((dycObject)obj, (dycMethod)methodID, args);
	return(*(char *)p);
}

jbyte JNICALL dyc_jni_CallNonvirtualByteMethodV(JNIEnv *env,
	jobject obj, jclass clazz, jmethodID methodID, va_list args)
{
	void *p;
	p=dycCallV((dycObject)obj, (dycMethod)methodID, args);
	return(*(char *)p);
}

jchar JNICALL dyc_jni_CallNonvirtualCharMethodV(JNIEnv *env,
	jobject obj, jclass clazz, jmethodID methodID, va_list args)
{
	void *p;
	p=dycCallV((dycObject)obj, (dycMethod)methodID, args);
	return(*(wchar_t *)p);
}

jshort JNICALL dyc_jni_CallNonvirtualShortMethodV(JNIEnv *env,
	jobject obj, jclass clazz, jmethodID methodID, va_list args)
{
	void *p;
	p=dycCallV((dycObject)obj, (dycMethod)methodID, args);
	return(*(short *)p);
}

jint JNICALL dyc_jni_CallNonvirtualIntMethodV(JNIEnv *env,
	jobject obj, jclass clazz, jmethodID methodID, va_list args)
{
	void *p;
	p=dycCallV((dycObject)obj, (dycMethod)methodID, args);
	return(*(int *)p);
}

jlong JNICALL dyc_jni_CallNonvirtualLongMethodV(JNIEnv *env,
	jobject obj, jclass clazz, jmethodID methodID, va_list args)
{
	void *p;
	p=dycCallV((dycObject)obj, (dycMethod)methodID, args);
	return(*(long long *)p);
}

jfloat JNICALL dyc_jni_CallNonvirtualFloatMethodV(JNIEnv *env,
	jobject obj, jclass clazz, jmethodID methodID, va_list args)
{
	void *p;
	p=dycCallV((dycObject)obj, (dycMethod)methodID, args);
	return(*(float *)p);
}

jdouble JNICALL dyc_jni_CallNonvirtualDoubleMethodV(JNIEnv *env,
	jobject obj, jclass clazz, jmethodID methodID, va_list args)
{
	void *p;
	p=dycCallV((dycObject)obj, (dycMethod)methodID, args);
	return(*(double *)p);
}

void JNICALL dyc_jni_CallNonvirtualVoidMethodV(JNIEnv *env,
	jobject obj, jclass clazz, jmethodID methodID, va_list args)
{
	dycCallV((dycObject)obj, (dycMethod)methodID, args);
}



jobject JNICALL dyc_jni_CallNonvirtualObjectMethod(JNIEnv *env,
	jobject obj, jclass clazz, jmethodID methodID, ...)
{
	va_list lst;
	void *p;
	va_start(lst, methodID);
	p=dycCallV((dycObject)obj, (dycMethod)methodID, lst);
	va_end(lst);
	return(*(jobject *)p);
}

jboolean JNICALL dyc_jni_CallNonvirtualBooleanMethod(JNIEnv *env,
	jobject obj, jclass clazz, jmethodID methodID, ...)
{
	va_list lst;
	void *p;
	va_start(lst, methodID);
	p=dycCallV((dycObject)obj, (dycMethod)methodID, lst);
	va_end(lst);
	return(*(char *)p);
}

jbyte JNICALL dyc_jni_CallNonvirtualByteMethod(JNIEnv *env,
	jobject obj, jclass clazz, jmethodID methodID, ...)
{
	va_list lst;
	void *p;
	va_start(lst, methodID);
	p=dycCallV((dycObject)obj, (dycMethod)methodID, lst);
	va_end(lst);
	return(*(char *)p);
}

jchar JNICALL dyc_jni_CallNonvirtualCharMethod(JNIEnv *env,
	jobject obj, jclass clazz, jmethodID methodID, ...)
{
	va_list lst;
	void *p;
	va_start(lst, methodID);
	p=dycCallV((dycObject)obj, (dycMethod)methodID, lst);
	va_end(lst);
	return(*(wchar_t *)p);
}

jshort JNICALL dyc_jni_CallNonvirtualShortMethod(JNIEnv *env,
	jobject obj, jclass clazz, jmethodID methodID, ...)
{
	va_list lst;
	void *p;
	va_start(lst, methodID);
	p=dycCallV((dycObject)obj, (dycMethod)methodID, lst);
	va_end(lst);
	return(*(short *)p);
}

jint JNICALL dyc_jni_CallNonvirtualIntMethod(JNIEnv *env,
	jobject obj, jclass clazz, jmethodID methodID, ...)
{
	va_list lst;
	void *p;
	va_start(lst, methodID);
	p=dycCallV((dycObject)obj, (dycMethod)methodID, lst);
	va_end(lst);
	return(*(int *)p);
}

jlong JNICALL dyc_jni_CallNonvirtualLongMethod(JNIEnv *env,
	jobject obj, jclass clazz, jmethodID methodID, ...)
{
	va_list lst;
	void *p;
	va_start(lst, methodID);
	p=dycCallV((dycObject)obj, (dycMethod)methodID, lst);
	va_end(lst);
	return(*(long long *)p);
}

jfloat JNICALL dyc_jni_CallNonvirtualFloatMethod(JNIEnv *env,
	jobject obj, jclass clazz, jmethodID methodID, ...)
{
	va_list lst;
	void *p;
	va_start(lst, methodID);
	p=dycCallV((dycObject)obj, (dycMethod)methodID, lst);
	va_end(lst);
	return(*(float *)p);
}

jdouble JNICALL dyc_jni_CallNonvirtualDoubleMethod(JNIEnv *env,
	jobject obj, jclass clazz, jmethodID methodID, ...)
{
	va_list lst;
	void *p;
	va_start(lst, methodID);
	p=dycCallV((dycObject)obj, (dycMethod)methodID, lst);
	va_end(lst);
	return(*(double *)p);
}

void JNICALL dyc_jni_CallNonvirtualVoidMethod(JNIEnv *env,
	jobject obj, jclass clazz, jmethodID methodID, ...)
{
	va_list lst;
	va_start(lst, methodID);
	dycCallV((dycObject)obj, (dycMethod)methodID, lst);
	va_end(lst);
}




jmethodID JNICALL dyc_jni_FromReflectedMethod(JNIEnv *env, jobject method) { return(NULL); }
jfieldID JNICALL dyc_jni_FromReflectedField(JNIEnv *env, jobject field) { return(NULL); }

jobject JNICALL dyc_jni_ToReflectedMethod(JNIEnv *env, jclass cls,
	jmethodID methodID, jboolean isStatic) { return(NULL); }
jobject JNICALL dyc_jni_ToReflectedField(JNIEnv *env, jclass cls,
	jfieldID fieldID, jboolean isStatic) { return(NULL); }

jint JNICALL dyc_jni_Throw(JNIEnv *env, jthrowable obj)
	{ dyllThrowObj((dyt)obj); return(-1); }

jint JNICALL dyc_jni_ThrowNew(JNIEnv *env, jclass clazz, const char *msg)
	{ dyllThrowObj((dyt)dycNew((dycClass)clazz)); return(-1); }

jthrowable JNICALL dyc_jni_ExceptionOccurred(JNIEnv *env)
	{ return(NULL); }
void JNICALL dyc_jni_ExceptionDescribe(JNIEnv *env) {}
void JNICALL dyc_jni_ExceptionClear(JNIEnv *env) {}
void JNICALL dyc_jni_FatalError(JNIEnv *env, const char *msg) {}

jint JNICALL dyc_jni_PushLocalFrame(JNIEnv *env, jint cap)
	{ return(0); }
jobject JNICALL dyc_jni_PopLocalFrame(JNIEnv *env, jobject res)
	{ return(NULL); }

jobject JNICALL dyc_jni_NewGlobalRef(JNIEnv *env, jobject lobj)
	{ return(lobj); }
void JNICALL dyc_jni_DeleteGlobalRef(JNIEnv *env, jobject gref) {}
void JNICALL dyc_jni_DeleteLocalRef(JNIEnv *env, jobject obj) {}

jobject JNICALL dyc_jni_NewLocalRef(JNIEnv *env, jobject ref)
	{ return(ref); }
jint JNICALL dyc_jni_EnsureLocalCapacity(JNIEnv *env, jint i)
	{ return(0); }


jint JNICALL dyc_jni_RegisterNatives(JNIEnv *env, jclass clazz, const JNINativeMethod *methods, jint nMethods)
	{ return(-1); }
jint JNICALL dyc_jni_UnregisterNatives(JNIEnv *env, jclass clazz)
	{ return(-1); }

jint JNICALL dyc_jni_MonitorEnter(JNIEnv *env, jobject obj)
	{ return(-1); }
jint JNICALL dyc_jni_MonitorExit(JNIEnv *env, jobject obj)
	{ return(-1); }
 
jint JNICALL dyc_jni_GetJavaVM(JNIEnv *env, JavaVM **vm)
{
	(*vm)=(JavaVM *)(&dyc_jni_javavm_iface);
	return(0);
}



jboolean JNICALL dyc_jni_ExceptionCheck(JNIEnv *env) { return(0); }

jobject JNICALL dyc_jni_NewDirectByteBuffer(JNIEnv* env, void* address, jlong capacity)
	{ return(NULL); }
void* JNICALL dyc_jni_GetDirectBufferAddress(JNIEnv* env, jobject buf)
	{ return(NULL); }
jlong JNICALL dyc_jni_GetDirectBufferCapacity(JNIEnv* env, jobject buf)
	{ return(0); }



jint JNICALL dyc_jni_DestroyJavaVM(JavaVM *jvm)
	{ return(-1); }

jint JNICALL dyc_jni_AttachCurrentThread(JavaVM *jvm,
	void **penv, void *args)
{
	(*penv)=&dyc_jni_iface;
	return(0);
}

jint JNICALL dyc_jni_DetachCurrentThread(JavaVM *jvm)
	{ return(0); }

jint JNICALL dyc_jni_GetEnv(JavaVM *jvm, void **penv, jint ver)
{
	(*penv)=&dyc_jni_iface;
	return(0);
}

jint JNICALL dyc_jni_AttachCurrentThreadAsDaemon(JavaVM *jvm,
	void **penv, void *args)
{
	(*penv)=&dyc_jni_iface;
	return(0);
}

