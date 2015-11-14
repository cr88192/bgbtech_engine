#include <bgbdy.h>
#include <jni.h>
#include <jvmti.h>
	
typedef struct ti_interface JVMTIEnv_t;

//AH:skip

jvmtiError JNICALL dyc_jvmti_SetEventNotificationMode(jvmtiEnv *env,
	jvmtiEventMode mode, jvmtiEvent event_type,
	jthread event_thread, ...);
jvmtiError JNICALL dyc_jvmti_GetAllThreads(jvmtiEnv *env,
	jint *threads_count_ptr, jthread **threads_ptr);
jvmtiError JNICALL dyc_jvmti_SuspendThread(jvmtiEnv *env, jthread thread);
jvmtiError JNICALL dyc_jvmti_ResumeThread(jvmtiEnv *env, jthread thread);
jvmtiError JNICALL dyc_jvmti_StopThread(jvmtiEnv *env,
	jthread thread, jobject exception);
jvmtiError JNICALL dyc_jvmti_InterruptThread(jvmtiEnv *env, jthread thread);
jvmtiError JNICALL dyc_jvmti_GetThreadInfo(jvmtiEnv *env,
	jthread thread, jvmtiThreadInfo *info_ptr);
jvmtiError JNICALL dyc_jvmti_GetOwnedMonitorInfo(jvmtiEnv *env,
	jthread thread, jint *owned_monitor_count_ptr,
	jobject **owned_monitors_ptr);
jvmtiError JNICALL dyc_jvmti_GetCurrentContendedMonitor(jvmtiEnv *env,
	jthread thread, jobject *monitor_ptr);
jvmtiError JNICALL dyc_jvmti_RunAgentThread(jvmtiEnv *env,
	jthread thread, jvmtiStartFunction proc,
	const void *arg, jint priority);
jvmtiError JNICALL dyc_jvmti_GetTopThreadGroups(jvmtiEnv *env,
	jint *group_count_ptr, jthreadGroup **groups_ptr);
jvmtiError JNICALL dyc_jvmti_GetThreadGroupInfo(jvmtiEnv *env,
	jthreadGroup group, jvmtiThreadGroupInfo *info_ptr);
jvmtiError JNICALL dyc_jvmti_GetThreadGroupChildren(jvmtiEnv *env,
	jthreadGroup group, jint *thread_count_ptr,
	jthread **threads_ptr, jint *group_count_ptr,
	jthreadGroup **groups_ptr);
jvmtiError JNICALL dyc_jvmti_GetFrameCount(jvmtiEnv *env,
	jthread thread, jint *count_ptr);
jvmtiError JNICALL dyc_jvmti_GetThreadState(jvmtiEnv *env,
	jthread thread, jint *thread_state_ptr);
jvmtiError JNICALL dyc_jvmti_GetFrameLocation(jvmtiEnv *env,
	jthread thread, jint depth, jmethodID *method_ptr,
	jlocation *location_ptr);
jvmtiError JNICALL dyc_jvmti_NotifyFramePop(jvmtiEnv *env,
	jthread thread, jint depth);
jvmtiError JNICALL dyc_jvmti_GetLocalObject(jvmtiEnv *env,
	jthread thread, jint depth, jint slot, jobject *value_ptr);
jvmtiError JNICALL dyc_jvmti_GetLocalInt(jvmtiEnv *env,
	jthread thread, jint depth, jint slot, jint *value_ptr);
jvmtiError JNICALL dyc_jvmti_GetLocalLong(jvmtiEnv *env,
	jthread thread, jint depth, jint slot, jlong *value_ptr);
jvmtiError JNICALL dyc_jvmti_GetLocalFloat(jvmtiEnv *env,
	jthread thread, jint depth, jint slot, jfloat *value_ptr);
jvmtiError JNICALL dyc_jvmti_GetLocalDouble(jvmtiEnv *env,
	jthread thread, jint depth, jint slot, jdouble *value_ptr);
jvmtiError JNICALL dyc_jvmti_SetLocalObject(jvmtiEnv *env,
	jthread thread, jint depth, jint slot, jobject value);
jvmtiError JNICALL dyc_jvmti_SetLocalInt(jvmtiEnv *env,
	jthread thread, jint depth, jint slot, jint value);
jvmtiError JNICALL dyc_jvmti_SetLocalLong(jvmtiEnv *env,
	jthread thread, jint depth, jint slot, jlong value);
jvmtiError JNICALL dyc_jvmti_SetLocalFloat(jvmtiEnv *env,
	jthread thread, jint depth, jint slot, jfloat value);
jvmtiError JNICALL dyc_jvmti_SetLocalDouble(jvmtiEnv *env,
	jthread thread, jint depth, jint slot, jdouble value);
jvmtiError JNICALL dyc_jvmti_CreateRawMonitor(jvmtiEnv *env,
	const char *name, jrawMonitorID *monitor_ptr);
jvmtiError JNICALL dyc_jvmti_DestroyRawMonitor(jvmtiEnv *env,
	jrawMonitorID monitor);
jvmtiError JNICALL dyc_jvmti_RawMonitorEnter(jvmtiEnv *env,
	jrawMonitorID monitor);
jvmtiError JNICALL dyc_jvmti_RawMonitorExit(jvmtiEnv *env,
	jrawMonitorID monitor);
jvmtiError JNICALL dyc_jvmti_RawMonitorWait(jvmtiEnv *env,
	jrawMonitorID monitor, jlong millis);
jvmtiError JNICALL dyc_jvmti_RawMonitorNotify(jvmtiEnv *env,
	jrawMonitorID monitor);
jvmtiError JNICALL dyc_jvmti_RawMonitorNotifyAll(jvmtiEnv *env,
	jrawMonitorID monitor);
jvmtiError JNICALL dyc_jvmti_SetBreakpoint(jvmtiEnv *env,
	jmethodID method, jlocation location);
jvmtiError JNICALL dyc_jvmti_ClearBreakpoint(jvmtiEnv *env,
	jmethodID method, jlocation location);
jvmtiError JNICALL dyc_jvmti_SetFieldAccessWatch(jvmtiEnv *env,
	jclass clazz, jfieldID field);
jvmtiError JNICALL dyc_jvmti_ClearFieldAccessWatch(jvmtiEnv *env,
	jclass clazz, jfieldID field);
jvmtiError JNICALL dyc_jvmti_SetFieldModificationWatch(jvmtiEnv *env,
	jclass clazz, jfieldID field);
jvmtiError JNICALL dyc_jvmti_ClearFieldModificationWatch(jvmtiEnv *env,
	jclass clazz, jfieldID field);
jvmtiError JNICALL dyc_jvmti_Allocate(jvmtiEnv *env,
	jlong size, unsigned char **mem_ptr);
jvmtiError JNICALL dyc_jvmti_Deallocate(jvmtiEnv *env, unsigned char *mem);
jvmtiError JNICALL dyc_jvmti_GetClassSignature(jvmtiEnv *env,
	jclass clazz, char **signature_ptr, char **generic_ptr);
jvmtiError JNICALL dyc_jvmti_GetClassStatus(jvmtiEnv *env,
	jclass clazz, jint *status_ptr);
jvmtiError JNICALL dyc_jvmti_GetSourceFileName(jvmtiEnv *env,
	jclass clazz, char **source_name_ptr);
jvmtiError JNICALL dyc_jvmti_GetClassModifiers(jvmtiEnv *env,
	jclass clazz, jint *modifiers_ptr);
jvmtiError JNICALL dyc_jvmti_GetClassMethods(jvmtiEnv *env,
	jclass clazz, jint *method_count_ptr, jmethodID **methods_ptr);
jvmtiError JNICALL dyc_jvmti_GetClassFields(jvmtiEnv *env,
	jclass clazz, jint *field_count_ptr, jfieldID **fields_ptr);
jvmtiError JNICALL dyc_jvmti_GetImplementedInterfaces(jvmtiEnv *env,
	jclass clazz, jint *interface_count_ptr, jclass **interfaces_ptr);
jvmtiError JNICALL dyc_jvmti_IsInterface(jvmtiEnv *env,
	jclass clazz, jboolean *is_interface_ptr);
jvmtiError JNICALL dyc_jvmti_IsArrayClass(jvmtiEnv *env,
	jclass clazz, jboolean *is_array_class_ptr);
jvmtiError JNICALL dyc_jvmti_GetClassLoader(jvmtiEnv *env,
	jclass clazz, jobject *classloader_ptr);
jvmtiError JNICALL dyc_jvmti_GetObjectHashCode(jvmtiEnv *env,
	jobject object, jint *hash_code_ptr);
jvmtiError JNICALL dyc_jvmti_GetObjectMonitorUsage(jvmtiEnv *env,
	jobject object, jvmtiMonitorUsage *info_ptr);
jvmtiError JNICALL dyc_jvmti_GetFieldName(jvmtiEnv *env,
	jclass clazz, jfieldID field, char **name_ptr,
	char **signature_ptr, char **generic_ptr);
jvmtiError JNICALL dyc_jvmti_GetFieldDeclaringClass(jvmtiEnv *env,
	jclass clazz, jfieldID field, jclass *declaring_class_ptr);
jvmtiError JNICALL dyc_jvmti_GetFieldModifiers(jvmtiEnv *env,
	jclass clazz, jfieldID field, jint *modifiers_ptr);
jvmtiError JNICALL dyc_jvmti_IsFieldSynthetic(jvmtiEnv *env,
	jclass clazz, jfieldID field, jboolean *is_synthetic_ptr);
jvmtiError JNICALL dyc_jvmti_GetMethodName(jvmtiEnv *env,
	jmethodID method, char **name_ptr,
	char **signature_ptr, char **generic_ptr);
jvmtiError JNICALL dyc_jvmti_GetMethodDeclaringClass(jvmtiEnv *env,
	jmethodID method, jclass *declaring_class_ptr);
jvmtiError JNICALL dyc_jvmti_GetMethodModifiers(jvmtiEnv *env,
	jmethodID method, jint *modifiers_ptr);
jvmtiError JNICALL dyc_jvmti_GetMaxLocals(jvmtiEnv *env,
	jmethodID method, jint *max_ptr);
jvmtiError JNICALL dyc_jvmti_GetArgumentsSize(jvmtiEnv *env,
	jmethodID method, jint *size_ptr);
jvmtiError JNICALL dyc_jvmti_GetLineNumberTable(jvmtiEnv *env,
	jmethodID method, jint *entry_count_ptr,
	jvmtiLineNumberEntry **table_ptr);
jvmtiError JNICALL dyc_jvmti_GetMethodLocation(jvmtiEnv *env,
	jmethodID method, jlocation *start_location_ptr,
	jlocation *end_location_ptr);
jvmtiError JNICALL dyc_jvmti_GetLocalVariableTable(jvmtiEnv *env,
	jmethodID method, jint *entry_count_ptr,
	jvmtiLocalVariableEntry **table_ptr);
jvmtiError JNICALL dyc_jvmti_GetBytecodes(jvmtiEnv *env,
	jmethodID method, jint *bytecode_count_ptr,
	unsigned char **bytecodes_ptr);
jvmtiError JNICALL dyc_jvmti_IsMethodNative(jvmtiEnv *env,
	jmethodID method, jboolean *is_native_ptr);
jvmtiError JNICALL dyc_jvmti_IsMethodSynthetic(jvmtiEnv *env,
	jmethodID method, jboolean *is_synthetic_ptr);
jvmtiError JNICALL dyc_jvmti_GetLoadedClasses(jvmtiEnv *env,
	jint *class_count_ptr, jclass **classes_ptr);
jvmtiError JNICALL dyc_jvmti_GetClassLoaderClasses(jvmtiEnv *env,
	jobject initiating_loader, jint *class_count_ptr,
	jclass **classes_ptr);
jvmtiError JNICALL dyc_jvmti_PopFrame(jvmtiEnv *env, jthread thread);
jvmtiError JNICALL dyc_jvmti_RedefineClasses(jvmtiEnv *env,
	jint class_count, const jvmtiClassDefinition *class_definitions);
jvmtiError JNICALL dyc_jvmti_GetVersionNumber(jvmtiEnv *env, jint *version_ptr);
jvmtiError JNICALL dyc_jvmti_GetCapabilities(jvmtiEnv *env,
	jvmtiCapabilities *capabilities_ptr);
jvmtiError JNICALL dyc_jvmti_GetSourceDebugExtension(jvmtiEnv *env,
	jclass clazz, char **source_debug_extension_ptr);
jvmtiError JNICALL dyc_jvmti_IsMethodObsolete(jvmtiEnv *env,
	jmethodID method, jboolean *is_obsolete_ptr);
jvmtiError JNICALL dyc_jvmti_SuspendThreadList(jvmtiEnv *env,
	jint request_count, const jthread *request_list,
	jvmtiError *results);
jvmtiError JNICALL dyc_jvmti_ResumeThreadList(jvmtiEnv *env,
	jint request_count, const jthread *request_list,
	jvmtiError *results);
jvmtiError JNICALL dyc_jvmti_GetAllStackTraces(jvmtiEnv *env,
	jint max_frame_count, jvmtiStackInfo **stack_info_ptr,
	jint *thread_count_ptr);
jvmtiError JNICALL dyc_jvmti_GetThreadListStackTraces(jvmtiEnv *env,
	jint thread_count, const jthread *thread_list,
	jint max_frame_count, jvmtiStackInfo **stack_info_ptr);
jvmtiError JNICALL dyc_jvmti_GetThreadLocalStorage(jvmtiEnv *env,
	jthread thread, void **data_ptr);
jvmtiError JNICALL dyc_jvmti_SetThreadLocalStorage(jvmtiEnv *env,
	jthread thread, const void *data);
jvmtiError JNICALL dyc_jvmti_GetStackTrace(jvmtiEnv *env,
	jthread thread, jint start_depth, jint max_frame_count,
	jvmtiFrameInfo *frame_buffer, jint *count_ptr);
jvmtiError JNICALL dyc_jvmti_GetTag(jvmtiEnv *env,
	jobject object, jlong *tag_ptr);
jvmtiError JNICALL dyc_jvmti_SetTag(jvmtiEnv *env, jobject object, jlong tag);
jvmtiError JNICALL dyc_jvmti_ForceGarbageCollection(jvmtiEnv *env);
jvmtiError JNICALL dyc_jvmti_IterateOverObjectsReachableFromObject(
	jvmtiEnv *env, jobject object,
	jvmtiObjectReferenceCallback object_reference_callback,
	void *user_data);
jvmtiError JNICALL dyc_jvmti_IterateOverReachableObjects(jvmtiEnv *env,
	jvmtiHeapRootCallback heap_root_callback,
	jvmtiStackReferenceCallback stack_ref_callback,
	jvmtiObjectReferenceCallback object_ref_callback,
	void *user_data);
jvmtiError JNICALL dyc_jvmti_IterateOverHeap(jvmtiEnv *env,
	jvmtiHeapObjectFilter object_filter,
	jvmtiHeapObjectCallback heap_object_callback, void *user_data);
jvmtiError JNICALL dyc_jvmti_IterateOverInstancesOfClass(jvmtiEnv *env,
	jclass clazz, jvmtiHeapObjectFilter object_filter,
	jvmtiHeapObjectCallback heap_object_callback, void *user_data);
jvmtiError JNICALL dyc_jvmti_GetObjectsWithTags(jvmtiEnv *env,
	jint tag_count, const jlong *tags, jint *count_ptr,
	jobject **object_result_ptr, jlong **tag_result_ptr);
jvmtiError JNICALL dyc_jvmti_SetJNIFunctionTable(jvmtiEnv *env,
	const jniNativeInterface *function_table);
jvmtiError JNICALL dyc_jvmti_GetJNIFunctionTable(jvmtiEnv *env,
	jniNativeInterface **function_table);
jvmtiError JNICALL dyc_jvmti_SetEventCallbacks(jvmtiEnv *env,
	const jvmtiEventCallbacks *callbacks, jint size_of_callbacks);
jvmtiError JNICALL dyc_jvmti_GenerateEvents(jvmtiEnv *env,
	jvmtiEvent event_type);
jvmtiError JNICALL dyc_jvmti_GetExtensionFunctions(jvmtiEnv *env,
	jint *extension_count_ptr, jvmtiExtensionFunctionInfo **extensions);
jvmtiError JNICALL dyc_jvmti_GetExtensionEvents(jvmtiEnv *env,
	jint *extension_count_ptr, jvmtiExtensionEventInfo **extensions);
jvmtiError JNICALL dyc_jvmti_SetExtensionEventCallback(jvmtiEnv *env,
	jint extension_event_index, jvmtiExtensionEvent callback);
jvmtiError JNICALL dyc_jvmti_DisposeEnvironment(jvmtiEnv *env);
jvmtiError JNICALL dyc_jvmti_GetErrorName(jvmtiEnv *env,
	jvmtiError error, char **name_ptr);
jvmtiError JNICALL dyc_jvmti_GetJLocationFormat(jvmtiEnv *env,
	jvmtiJlocationFormat *format_ptr);
jvmtiError JNICALL dyc_jvmti_GetSystemProperties(jvmtiEnv *env,
	jint *count_ptr, char ***property_ptr);
jvmtiError JNICALL dyc_jvmti_GetSystemProperty(jvmtiEnv *env,
	const char *property, char **value_ptr);
jvmtiError JNICALL dyc_jvmti_SetSystemProperty(jvmtiEnv *env,
	const char *property, const char *value);
jvmtiError JNICALL dyc_jvmti_GetPhase(jvmtiEnv *env, jvmtiPhase *phase_ptr);
jvmtiError JNICALL dyc_jvmti_GetCurrentThreadCpuTimerInfo(jvmtiEnv *env,
	jvmtiTimerInfo *info_ptr);
jvmtiError JNICALL dyc_jvmti_GetCurrentThreadCpuTime(jvmtiEnv *env,
	jlong *nanos_ptr);
jvmtiError JNICALL dyc_jvmti_GetThreadCpuTimerInfo(jvmtiEnv *env,
	jvmtiTimerInfo *info_ptr);
jvmtiError JNICALL dyc_jvmti_GetThreadCpuTime(jvmtiEnv *env,
	jthread thread, jlong *nanos_ptr);
jvmtiError JNICALL dyc_jvmti_GetTimerInfo(jvmtiEnv *env,
	jvmtiTimerInfo *info_ptr);
jvmtiError JNICALL dyc_jvmti_GetTime(jvmtiEnv *env, jlong *nanos_ptr);
jvmtiError JNICALL dyc_jvmti_GetPotentialCapabilities(jvmtiEnv *env,
	jvmtiCapabilities *capabilities_ptr);
jvmtiError JNICALL dyc_jvmti_AddCapabilities(jvmtiEnv *env,
	const jvmtiCapabilities *capabilities_ptr);
jvmtiError JNICALL dyc_jvmti_RelinquishCapabilities(jvmtiEnv *env,
	const jvmtiCapabilities *capabilities_ptr);
jvmtiError JNICALL dyc_jvmti_GetAvailableProcessors(jvmtiEnv *env,
	jint *processor_count_ptr);
jvmtiError JNICALL dyc_jvmti_GetEnvironmentLocalStorage(jvmtiEnv *env,
	void **data_ptr);
jvmtiError JNICALL dyc_jvmti_SetEnvironmentLocalStorage(jvmtiEnv *env,
	const void *data);
jvmtiError JNICALL dyc_jvmti_AddToBootstrapClassLoaderSearch(jvmtiEnv *env,
	const char *segment);
jvmtiError JNICALL dyc_jvmti_SetVerboseFlag(jvmtiEnv *env,
	jvmtiVerboseFlag flag, jboolean value);
jvmtiError JNICALL dyc_jvmti_GetObjectSize(jvmtiEnv *env,
	jobject object, jlong *size_ptr);

			
struct ti_interface dyc_jni_jvmti_iface =
{
NULL,					//1;
dyc_jvmti_SetEventNotificationMode,	//2, SetEventNotificationMode,
NULL,					//3;
dyc_jvmti_GetAllThreads,		//4, GetAllThreads,
dyc_jvmti_SuspendThread,		//5, SuspendThread,
dyc_jvmti_ResumeThread,			//6, ResumeThread,
dyc_jvmti_StopThread,			//7, StopThread,
dyc_jvmti_InterruptThread,		//8, InterruptThread,
dyc_jvmti_GetThreadInfo,		//9, GetThreadInfo,
dyc_jvmti_GetOwnedMonitorInfo,		//10, GetOwnedMonitorInfo,
dyc_jvmti_GetCurrentContendedMonitor,	//11, GetCurrentContendedMonitor,
dyc_jvmti_RunAgentThread,		//12, RunAgentThread,
dyc_jvmti_GetTopThreadGroups,		//13, GetTopThreadGroups,
dyc_jvmti_GetThreadGroupInfo,		//14, GetThreadGroupInfo,
dyc_jvmti_GetThreadGroupChildren,	//15, GetThreadGroupChildren,
dyc_jvmti_GetFrameCount,		//16, GetFrameCount,
dyc_jvmti_GetThreadState,		//17, GetThreadState,
NULL,					//18;
dyc_jvmti_GetFrameLocation,		//19, GetFrameLocation,
dyc_jvmti_NotifyFramePop,		//20, NotifyFramePop,
dyc_jvmti_GetLocalObject,		//21, GetLocalObject,
dyc_jvmti_GetLocalInt,			//22, GetLocalInt,
dyc_jvmti_GetLocalLong,			//23, GetLocalLong,
dyc_jvmti_GetLocalFloat,		//24, GetLocalFloat,
dyc_jvmti_GetLocalDouble,		//25, GetLocalDouble,
dyc_jvmti_SetLocalObject,		//26, SetLocalObject,
dyc_jvmti_SetLocalInt,			//27, SetLocalInt,
dyc_jvmti_SetLocalLong,			//28, SetLocalLong,
dyc_jvmti_SetLocalFloat,		//29, SetLocalFloat,
dyc_jvmti_SetLocalDouble,		//30, SetLocalDouble,
dyc_jvmti_CreateRawMonitor,		//31, CreateRawMonitor,
dyc_jvmti_DestroyRawMonitor,		//32, DestroyRawMonitor,
dyc_jvmti_RawMonitorEnter,		//33, RawMonitorEnter,
dyc_jvmti_RawMonitorExit,		//34, RawMonitorExit,
dyc_jvmti_RawMonitorWait,		//35, RawMonitorWait,
dyc_jvmti_RawMonitorNotify,		//36, RawMonitorNotify,
dyc_jvmti_RawMonitorNotifyAll,		//37, RawMonitorNotifyAll,
dyc_jvmti_SetBreakpoint,		//38, SetBreakpoint,
dyc_jvmti_ClearBreakpoint,		//39, ClearBreakpoint,
NULL,					//40;
dyc_jvmti_SetFieldAccessWatch,		//41, SetFieldAccessWatch,
dyc_jvmti_ClearFieldAccessWatch,	//42, ClearFieldAccessWatch,
dyc_jvmti_SetFieldModificationWatch,	//43, SetFieldModificationWatch,
dyc_jvmti_ClearFieldModificationWatch,	//44, ClearFieldModificationWatch,
NULL,					//45;
dyc_jvmti_Allocate,			//46, Allocate,
dyc_jvmti_Deallocate,			//47, Deallocate,
dyc_jvmti_GetClassSignature,		//48, GetClassSignature,
dyc_jvmti_GetClassStatus,		//49, GetClassStatus,
dyc_jvmti_GetSourceFileName,		//50, GetSourceFileName,
dyc_jvmti_GetClassModifiers,		//51, GetClassModifiers,
dyc_jvmti_GetClassMethods,		//52, GetClassMethods,
dyc_jvmti_GetClassFields,		//53, GetClassFields,
dyc_jvmti_GetImplementedInterfaces,	//54, GetImplementedInterfaces,
dyc_jvmti_IsInterface,			//55, IsInterface,
dyc_jvmti_IsArrayClass,			//56, IsArrayClass,
dyc_jvmti_GetClassLoader,		//57, GetClassLoader,
dyc_jvmti_GetObjectHashCode,		//58, GetObjectHashCode,
dyc_jvmti_GetObjectMonitorUsage,	//59, GetObjectMonitorUsage,
dyc_jvmti_GetFieldName,			//60, GetFieldName,
dyc_jvmti_GetFieldDeclaringClass,	//61, GetFieldDeclaringClass,
dyc_jvmti_GetFieldModifiers,		//62, GetFieldModifiers,
dyc_jvmti_IsFieldSynthetic,		//63, IsFieldSynthetic,
dyc_jvmti_GetMethodName,		//64, GetMethodName,
dyc_jvmti_GetMethodDeclaringClass,	//65, GetMethodDeclaringClass,
dyc_jvmti_GetMethodModifiers,		//66, GetMethodModifiers,
NULL,					//67;
dyc_jvmti_GetMaxLocals,			//68, GetMaxLocals,
dyc_jvmti_GetArgumentsSize,		//69, GetArgumentsSize,
dyc_jvmti_GetLineNumberTable,		//70, GetLineNumberTable,
dyc_jvmti_GetMethodLocation,		//71, GetMethodLocation,
dyc_jvmti_GetLocalVariableTable,	//72, GetLocalVariableTable,
NULL,					//73;
NULL,					//74;
dyc_jvmti_GetBytecodes,			//75, GetBytecodes,
dyc_jvmti_IsMethodNative,		//76, IsMethodNative,
dyc_jvmti_IsMethodSynthetic,		//77, IsMethodSynthetic,
dyc_jvmti_GetLoadedClasses,		//78, GetLoadedClasses,
dyc_jvmti_GetClassLoaderClasses,	//79, GetClassLoaderClasses,
dyc_jvmti_PopFrame,			//80, PopFrame,
NULL,					//81;
NULL,					//82;
NULL,					//83;
NULL,					//84;
NULL,					//85;
NULL,					//86;
dyc_jvmti_RedefineClasses,		//87, RedefineClasses,
dyc_jvmti_GetVersionNumber,		//88, GetVersionNumber,
dyc_jvmti_GetCapabilities,		//89, GetCapabilities,
dyc_jvmti_GetSourceDebugExtension,	//90, GetSourceDebugExtension,
dyc_jvmti_IsMethodObsolete,		//91, IsMethodObsolete,
dyc_jvmti_SuspendThreadList,		//92, SuspendThreadList,
dyc_jvmti_ResumeThreadList,		//93, ResumeThreadList,
NULL,					//94;
NULL,					//95;
NULL,					//96;
NULL,					//97;
NULL,					//98;
NULL,					//99;
dyc_jvmti_GetAllStackTraces,		//100, GetAllStackTraces,
dyc_jvmti_GetThreadListStackTraces,	//101, GetThreadListStackTraces,
dyc_jvmti_GetThreadLocalStorage,	//102, GetThreadLocalStorage,
dyc_jvmti_SetThreadLocalStorage,	//103, SetThreadLocalStorage,
dyc_jvmti_GetStackTrace,		//104, GetStackTrace,
NULL,					//105;
dyc_jvmti_GetTag,			//106, GetTag,
dyc_jvmti_SetTag,			//107, SetTag,
dyc_jvmti_ForceGarbageCollection,	//108, ForceGarbageCollection,
dyc_jvmti_IterateOverObjectsReachableFromObject,	//109, IterateOverObjectsReachableFromObject,
dyc_jvmti_IterateOverReachableObjects,	//110, IterateOverReachableObjects,
dyc_jvmti_IterateOverHeap,		//111, IterateOverHeap,
dyc_jvmti_IterateOverInstancesOfClass,	//112, IterateOverInstancesOfClass,
NULL,					//113;
dyc_jvmti_GetObjectsWithTags,		//114, GetObjectsWithTags,
NULL,					//115;
NULL,					//116;
NULL,					//117;
NULL,					//118;
NULL,					//119;
dyc_jvmti_SetJNIFunctionTable,		//120, SetJNIFunctionTable,
dyc_jvmti_GetJNIFunctionTable,		//121, GetJNIFunctionTable,
dyc_jvmti_SetEventCallbacks,		//122, SetEventCallbacks,
dyc_jvmti_GenerateEvents,		//123, GenerateEvents,
dyc_jvmti_GetExtensionFunctions,	//124, GetExtensionFunctions,
dyc_jvmti_GetExtensionEvents,		//125, GetExtensionEvents,
dyc_jvmti_SetExtensionEventCallback,	//126, SetExtensionEventCallback,
dyc_jvmti_DisposeEnvironment,		//127, DisposeEnvironment,
dyc_jvmti_GetErrorName,			//128, GetErrorName,
dyc_jvmti_GetJLocationFormat,		//129, GetJLocationFormat,
dyc_jvmti_GetSystemProperties,		//130, GetSystemProperties,
dyc_jvmti_GetSystemProperty,		//131, GetSystemProperty,
dyc_jvmti_SetSystemProperty,		//132, SetSystemProperty,
dyc_jvmti_GetPhase,			//133, GetPhase,
dyc_jvmti_GetCurrentThreadCpuTimerInfo,	//134, GetCurrentThreadCpuTimerInfo,
dyc_jvmti_GetCurrentThreadCpuTime,	//135, GetCurrentThreadCpuTime,
dyc_jvmti_GetThreadCpuTimerInfo,	//136, GetThreadCpuTimerInfo,
dyc_jvmti_GetThreadCpuTime,		//137, GetThreadCpuTime,
dyc_jvmti_GetTimerInfo,			//138, GetTimerInfo,
dyc_jvmti_GetTime,			//139, GetTime,
dyc_jvmti_GetPotentialCapabilities,	//140, GetPotentialCapabilities,
NULL,					//141;
dyc_jvmti_AddCapabilities,		//142, AddCapabilities,
dyc_jvmti_RelinquishCapabilities,	//143, RelinquishCapabilities,
dyc_jvmti_GetAvailableProcessors,	//144, GetAvailableProcessors,
NULL,					//145;
NULL,					//146;
dyc_jvmti_GetEnvironmentLocalStorage,	//147, GetEnvironmentLocalStorage,
dyc_jvmti_SetEnvironmentLocalStorage,	//148, SetEnvironmentLocalStorage,
dyc_jvmti_AddToBootstrapClassLoaderSearch,	//149, AddToBootstrapClassLoaderSearch,
dyc_jvmti_SetVerboseFlag,		//150, SetVerboseFlag,
NULL,					//151;
NULL,					//152;
NULL,					//153;
dyc_jvmti_GetObjectSize,		//154, GetObjectSize,
};

BGBDY_API void *dycJniGetJvmtiEnv()
{
	static jvmtiEnv env;
	env=&dyc_jni_jvmti_iface;
	return((void *)(&env));
}

