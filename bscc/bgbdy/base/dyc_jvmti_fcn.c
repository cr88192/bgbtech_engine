#include <bgbdy.h>
#include <jni.h>
#include <jvmti.h>
	

//AH:skip

jvmtiError JNICALL dyc_jvmti_SetEventNotificationMode(jvmtiEnv *env,
	jvmtiEventMode mode, jvmtiEvent event_type,
	jthread event_thread, ...)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_GetAllThreads(jvmtiEnv *env,
	jint *threads_count_ptr, jthread **threads_ptr)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_SuspendThread(jvmtiEnv *env, jthread thread)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_ResumeThread(jvmtiEnv *env, jthread thread)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_StopThread(jvmtiEnv *env,
	jthread thread, jobject exception)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_InterruptThread(jvmtiEnv *env, jthread thread)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_GetThreadInfo(jvmtiEnv *env,
	jthread thread, jvmtiThreadInfo *info_ptr)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_GetOwnedMonitorInfo(jvmtiEnv *env,
	jthread thread, jint *owned_monitor_count_ptr,
	jobject **owned_monitors_ptr)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_GetCurrentContendedMonitor(jvmtiEnv *env,
	jthread thread, jobject *monitor_ptr)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_RunAgentThread(jvmtiEnv *env,
	jthread thread, jvmtiStartFunction proc,
	const void *arg, jint priority)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_GetTopThreadGroups(jvmtiEnv *env,
	jint *group_count_ptr, jthreadGroup **groups_ptr)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_GetThreadGroupInfo(jvmtiEnv *env,
	jthreadGroup group, jvmtiThreadGroupInfo *info_ptr)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_GetThreadGroupChildren(jvmtiEnv *env,
	jthreadGroup group, jint *thread_count_ptr,
	jthread **threads_ptr, jint *group_count_ptr,
	jthreadGroup **groups_ptr)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_GetFrameCount(jvmtiEnv *env,
	jthread thread, jint *count_ptr)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_GetThreadState(jvmtiEnv *env,
	jthread thread, jint *thread_state_ptr)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_GetFrameLocation(jvmtiEnv *env,
	jthread thread, jint depth, jmethodID *method_ptr,
	jlocation *location_ptr)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_NotifyFramePop(jvmtiEnv *env,
	jthread thread, jint depth)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_GetLocalObject(jvmtiEnv *env,
	jthread thread, jint depth, jint slot, jobject *value_ptr)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_GetLocalInt(jvmtiEnv *env,
	jthread thread, jint depth, jint slot, jint *value_ptr)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_GetLocalLong(jvmtiEnv *env,
	jthread thread, jint depth, jint slot, jlong *value_ptr)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_GetLocalFloat(jvmtiEnv *env,
	jthread thread, jint depth, jint slot, jfloat *value_ptr)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_GetLocalDouble(jvmtiEnv *env,
	jthread thread, jint depth, jint slot, jdouble *value_ptr)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_SetLocalObject(jvmtiEnv *env,
	jthread thread, jint depth, jint slot, jobject value)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_SetLocalInt(jvmtiEnv *env,
	jthread thread, jint depth, jint slot, jint value)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_SetLocalLong(jvmtiEnv *env,
	jthread thread, jint depth, jint slot, jlong value)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_SetLocalFloat(jvmtiEnv *env,
	jthread thread, jint depth, jint slot, jfloat value)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_SetLocalDouble(jvmtiEnv *env,
	jthread thread, jint depth, jint slot, jdouble value)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_CreateRawMonitor(jvmtiEnv *env,
	const char *name, jrawMonitorID *monitor_ptr)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_DestroyRawMonitor(jvmtiEnv *env,
	jrawMonitorID monitor)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_RawMonitorEnter(jvmtiEnv *env,
	jrawMonitorID monitor)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_RawMonitorExit(jvmtiEnv *env,
	jrawMonitorID monitor)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_RawMonitorWait(jvmtiEnv *env,
	jrawMonitorID monitor, jlong millis)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_RawMonitorNotify(jvmtiEnv *env,
	jrawMonitorID monitor)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_RawMonitorNotifyAll(jvmtiEnv *env,
	jrawMonitorID monitor)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_SetBreakpoint(jvmtiEnv *env,
	jmethodID method, jlocation location)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_ClearBreakpoint(jvmtiEnv *env,
	jmethodID method, jlocation location)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_SetFieldAccessWatch(jvmtiEnv *env,
	jclass clazz, jfieldID field)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_ClearFieldAccessWatch(jvmtiEnv *env,
	jclass clazz, jfieldID field)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_SetFieldModificationWatch(jvmtiEnv *env,
	jclass clazz, jfieldID field)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_ClearFieldModificationWatch(jvmtiEnv *env,
	jclass clazz, jfieldID field)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_Allocate(jvmtiEnv *env,
	jlong size, unsigned char **mem_ptr)
{
	if(!mem_ptr)return(JVMTI_ERROR_NULL_POINTER);
	if(!(size<=0))return(JVMTI_ERROR_ILLEGAL_ARGUMENT);
	*mem_ptr=gcmalloc(size);
	return(JVMTI_ERROR_NONE);
}

jvmtiError JNICALL dyc_jvmti_Deallocate(jvmtiEnv *env, unsigned char *mem)
{
	if(!mem)return(JVMTI_ERROR_NONE);
	gcfree(mem);
	return(JVMTI_ERROR_NONE);
}

jvmtiError JNICALL dyc_jvmti_GetClassSignature(jvmtiEnv *env,
	jclass clazz, char **signature_ptr, char **generic_ptr)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_GetClassStatus(jvmtiEnv *env,
	jclass clazz, jint *status_ptr)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_GetSourceFileName(jvmtiEnv *env,
	jclass clazz, char **source_name_ptr)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_GetClassModifiers(jvmtiEnv *env,
	jclass clazz, jint *modifiers_ptr)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_GetClassMethods(jvmtiEnv *env,
	jclass clazz, jint *method_count_ptr, jmethodID **methods_ptr)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_GetClassFields(jvmtiEnv *env,
	jclass clazz, jint *field_count_ptr, jfieldID **fields_ptr)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_GetImplementedInterfaces(jvmtiEnv *env,
	jclass clazz, jint *interface_count_ptr, jclass **interfaces_ptr)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_IsInterface(jvmtiEnv *env,
	jclass clazz, jboolean *is_interface_ptr)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_IsArrayClass(jvmtiEnv *env,
	jclass clazz, jboolean *is_array_class_ptr)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_GetClassLoader(jvmtiEnv *env,
	jclass clazz, jobject *classloader_ptr)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_GetObjectHashCode(jvmtiEnv *env,
	jobject object, jint *hash_code_ptr)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_GetObjectMonitorUsage(jvmtiEnv *env,
	jobject object, jvmtiMonitorUsage *info_ptr)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_GetFieldName(jvmtiEnv *env,
	jclass clazz, jfieldID field, char **name_ptr,
	char **signature_ptr, char **generic_ptr)
{
	char *s0, *s1, *t;

	s0=dycGetSlotName((dycSlot)field);
	s1=dycGetSlotSig((dycSlot)field);

	if(name_ptr)
		{ t=gcalloc(strlen(s0)+1); strcpy(t, s0); *name_ptr=t; }
	if(signature_ptr)
		{ t=gcalloc(strlen(s1)+1); strcpy(t, s1); *signature_ptr=t; }
	if(generic_ptr)*generic_ptr=NULL;
	return(JVMTI_ERROR_NONE);
}

jvmtiError JNICALL dyc_jvmti_GetFieldDeclaringClass(jvmtiEnv *env,
	jclass clazz, jfieldID field, jclass *declaring_class_ptr)
{
	if(declaring_class_ptr)
		*declaring_class_ptr=(jclass)dycGetSlotClass((dycSlot)field);
	return(JVMTI_ERROR_NONE);
}

jvmtiError JNICALL dyc_jvmti_GetFieldModifiers(jvmtiEnv *env,
	jclass clazz, jfieldID field, jint *modifiers_ptr)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_IsFieldSynthetic(jvmtiEnv *env,
	jclass clazz, jfieldID field, jboolean *is_synthetic_ptr)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_GetMethodName(jvmtiEnv *env,
	jmethodID method, char **name_ptr,
	char **signature_ptr, char **generic_ptr)
{
	char *s0, *s1, *t;

	s0=dycGetMethodName((dycMethod)method);
	s1=dycGetMethodSig((dycMethod)method);

	if(name_ptr)
		{ t=gcalloc(strlen(s0)+1); strcpy(t, s0); *name_ptr=t; }
	if(signature_ptr)
		{ t=gcalloc(strlen(s1)+1); strcpy(t, s1); *signature_ptr=t; }
	if(generic_ptr)*generic_ptr=NULL;
	return(JVMTI_ERROR_NONE);
}

jvmtiError JNICALL dyc_jvmti_GetMethodDeclaringClass(jvmtiEnv *env,
	jmethodID method, jclass *declaring_class_ptr)
{
	if(declaring_class_ptr)
		*declaring_class_ptr=(jclass)dycGetMethodClass((dycMethod)method);
	return(JVMTI_ERROR_NONE);
}

jvmtiError JNICALL dyc_jvmti_GetMethodModifiers(jvmtiEnv *env,
	jmethodID method, jint *modifiers_ptr)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_GetMaxLocals(jvmtiEnv *env,
	jmethodID method, jint *max_ptr)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_GetArgumentsSize(jvmtiEnv *env,
	jmethodID method, jint *size_ptr)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_GetLineNumberTable(jvmtiEnv *env,
	jmethodID method, jint *entry_count_ptr,
	jvmtiLineNumberEntry **table_ptr)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_GetMethodLocation(jvmtiEnv *env,
	jmethodID method, jlocation *start_location_ptr,
	jlocation *end_location_ptr)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_GetLocalVariableTable(jvmtiEnv *env,
	jmethodID method, jint *entry_count_ptr,
	jvmtiLocalVariableEntry **table_ptr)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_GetBytecodes(jvmtiEnv *env,
	jmethodID method, jint *bytecode_count_ptr,
	unsigned char **bytecodes_ptr)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_IsMethodNative(jvmtiEnv *env,
	jmethodID method, jboolean *is_native_ptr)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_IsMethodSynthetic(jvmtiEnv *env,
	jmethodID method, jboolean *is_synthetic_ptr)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_GetLoadedClasses(jvmtiEnv *env,
	jint *class_count_ptr, jclass **classes_ptr)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_GetClassLoaderClasses(jvmtiEnv *env,
	jobject initiating_loader, jint *class_count_ptr,
	jclass **classes_ptr)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_PopFrame(jvmtiEnv *env, jthread thread)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_RedefineClasses(jvmtiEnv *env,
	jint class_count, const jvmtiClassDefinition *class_definitions)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_GetVersionNumber(jvmtiEnv *env, jint *version_ptr)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_GetCapabilities(jvmtiEnv *env,
	jvmtiCapabilities *capabilities_ptr)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_GetSourceDebugExtension(jvmtiEnv *env,
	jclass clazz, char **source_debug_extension_ptr)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_IsMethodObsolete(jvmtiEnv *env,
	jmethodID method, jboolean *is_obsolete_ptr)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_SuspendThreadList(jvmtiEnv *env,
	jint request_count, const jthread *request_list,
	jvmtiError *results)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_ResumeThreadList(jvmtiEnv *env,
	jint request_count, const jthread *request_list,
	jvmtiError *results)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_GetAllStackTraces(jvmtiEnv *env,
	jint max_frame_count, jvmtiStackInfo **stack_info_ptr,
	jint *thread_count_ptr)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_GetThreadListStackTraces(jvmtiEnv *env,
	jint thread_count, const jthread *thread_list,
	jint max_frame_count, jvmtiStackInfo **stack_info_ptr)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_GetThreadLocalStorage(jvmtiEnv *env,
	jthread thread, void **data_ptr)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_SetThreadLocalStorage(jvmtiEnv *env,
	jthread thread, const void *data)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_GetStackTrace(jvmtiEnv *env,
	jthread thread, jint start_depth, jint max_frame_count,
	jvmtiFrameInfo *frame_buffer, jint *count_ptr)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_GetTag(jvmtiEnv *env,
	jobject object, jlong *tag_ptr)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_SetTag(jvmtiEnv *env, jobject object, jlong tag)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_ForceGarbageCollection(jvmtiEnv *env)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_IterateOverObjectsReachableFromObject(
	jvmtiEnv *env, jobject object,
	jvmtiObjectReferenceCallback object_reference_callback,
	void *user_data)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_IterateOverReachableObjects(jvmtiEnv *env,
	jvmtiHeapRootCallback heap_root_callback,
	jvmtiStackReferenceCallback stack_ref_callback,
	jvmtiObjectReferenceCallback object_ref_callback,
	void *user_data)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_IterateOverHeap(jvmtiEnv *env,
	jvmtiHeapObjectFilter object_filter,
	jvmtiHeapObjectCallback heap_object_callback, void *user_data)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_IterateOverInstancesOfClass(jvmtiEnv *env,
	jclass clazz, jvmtiHeapObjectFilter object_filter,
	jvmtiHeapObjectCallback heap_object_callback, void *user_data)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_GetObjectsWithTags(jvmtiEnv *env,
	jint tag_count, const jlong *tags, jint *count_ptr,
	jobject **object_result_ptr, jlong **tag_result_ptr)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_SetJNIFunctionTable(jvmtiEnv *env,
	const jniNativeInterface *function_table)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_GetJNIFunctionTable(jvmtiEnv *env,
	jniNativeInterface **function_table)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_SetEventCallbacks(jvmtiEnv *env,
	const jvmtiEventCallbacks *callbacks, jint size_of_callbacks)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_GenerateEvents(jvmtiEnv *env,
	jvmtiEvent event_type)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_GetExtensionFunctions(jvmtiEnv *env,
	jint *extension_count_ptr, jvmtiExtensionFunctionInfo **extensions)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_GetExtensionEvents(jvmtiEnv *env,
	jint *extension_count_ptr, jvmtiExtensionEventInfo **extensions)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_SetExtensionEventCallback(jvmtiEnv *env,
	jint extension_event_index, jvmtiExtensionEvent callback)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_DisposeEnvironment(jvmtiEnv *env)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_GetErrorName(jvmtiEnv *env,
	jvmtiError error, char **name_ptr)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_GetJLocationFormat(jvmtiEnv *env,
	jvmtiJlocationFormat *format_ptr)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_GetSystemProperties(jvmtiEnv *env,
	jint *count_ptr, char ***property_ptr)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_GetSystemProperty(jvmtiEnv *env,
	const char *property, char **value_ptr)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_SetSystemProperty(jvmtiEnv *env,
	const char *property, const char *value)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_GetPhase(jvmtiEnv *env, jvmtiPhase *phase_ptr)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_GetCurrentThreadCpuTimerInfo(jvmtiEnv *env,
	jvmtiTimerInfo *info_ptr)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_GetCurrentThreadCpuTime(jvmtiEnv *env,
	jlong *nanos_ptr)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_GetThreadCpuTimerInfo(jvmtiEnv *env,
	jvmtiTimerInfo *info_ptr)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_GetThreadCpuTime(jvmtiEnv *env,
	jthread thread, jlong *nanos_ptr)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_GetTimerInfo(jvmtiEnv *env,
	jvmtiTimerInfo *info_ptr)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_GetTime(jvmtiEnv *env, jlong *nanos_ptr)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_GetPotentialCapabilities(jvmtiEnv *env,
	jvmtiCapabilities *capabilities_ptr)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_AddCapabilities(jvmtiEnv *env,
	const jvmtiCapabilities *capabilities_ptr)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_RelinquishCapabilities(jvmtiEnv *env,
	const jvmtiCapabilities *capabilities_ptr)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_GetAvailableProcessors(jvmtiEnv *env,
	jint *processor_count_ptr)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_GetEnvironmentLocalStorage(jvmtiEnv *env,
	void **data_ptr)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_SetEnvironmentLocalStorage(jvmtiEnv *env,
	const void *data)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_AddToBootstrapClassLoaderSearch(jvmtiEnv *env,
	const char *segment)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_SetVerboseFlag(jvmtiEnv *env,
	jvmtiVerboseFlag flag, jboolean value)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

jvmtiError JNICALL dyc_jvmti_GetObjectSize(jvmtiEnv *env,
	jobject object, jlong *size_ptr)
{ return(JVMTI_ERROR_ACCESS_DENIED); }

