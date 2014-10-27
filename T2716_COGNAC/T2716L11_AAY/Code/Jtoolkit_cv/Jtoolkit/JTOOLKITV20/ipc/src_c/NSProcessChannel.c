/*
 * @File: NSProcessChannel.c
 * @Description: NSProcessChannel Class JNI 'C' code functions that
 *       provide client end-point for the communication to a NS Process.
 *
 * @Version: 1.0
 * @CreationDate: April 29, 2009
 * @Author: Sajad Bashir Qadri
 * @Author: Suveer Nagendra
 */

/* As a result of code reivew on 25th April, there were quite a few suggestions for improving the code quality.
 * There are lot of TODO items in the file.
 * Need to make changes are appropriate.
 */
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/time.h>
#include<sys/types.h>

#include<tal.h>
#include<cextdecs.h>
#include<pthread.h>
#include<errno.h>


#include"com_hp_nonstop_io_process_impl_NSProcessChannel.h"
#include"functionpointers.h"

/* VARIABLE DECLARATIONS */

/* Error codes shared with java wrapper*/

int JNI_ERROR = 0;
int AWAITIO_ERROR = 0;
int INVALID_FILENUM_ERROR = 0;
int NULL_PROCESS_NAME_ERROR = 0;
int NULL_REQDATA_ERROR = 0;
int NULL_REPLY_DATA_ERROR = 0;
int TRANSFER_OVERFLOW_ERROR = 0;
int INVALID_STATE = 0;
int CLOSE_INTERRUPED = 0;
int CLASS_NOT_FOUND = 0;
int TIMEDOUT = 0;

/* This is the error code to indicate more data is present in the server */
const short FE_CONTINUE = 70;

/* Name of the java class to receive status of an operation.*/
#define STATUS_CLASSNAME "com/hp/nonstop/io/process/impl/NativeStatus"
#define OPENOPTIONS_CLASSNAME "com/hp/nonstop/io/process/impl/NSProcessOpenOptions"

#define IO_BUFFER_MAX_SIZE 2097152
#define DEFAULT_IO_BUFFER_SIZE 57344

#define FILE_NOT_OPEN_ERROR_CODE 16

/* Log Levels */
#define TRACE  0
#define DEBUG  1
#define INFO   2
#define WARN   3
#define ERROR  4
#define FATAL  5
#define NOLOGS 6
/* Some helper macros */
#define HANDLEERROR(s,f,ret) if(ret!=0){ \
						char msg[1024]; \
						sprintf(msg,"%s [%s]",s,strerror(ret)); \
						logMsg(FATAL,"%s [%d][%s]",s,ret,strerror(ret)); \
						return createStatusObject(env,(short)f,(int)ret,msg); \
					}

#define HANDLEINITERROR(s) if(initialized==0){ \
						char msg[255]; \
						sprintf(msg,"Call to %s. JNI not initialized. Call jniInit first",s); \
						logMsg(FATAL,"%s",msg); \
						initializeStatusClass(env); \
						return createStatusObject(env,(short)-1,(int)-1,msg); \
					}

/*
 * The following are the macros that can be used to trace memory leaks
 * Its possible to trace memory leaks using einspect. But that requires good knowledge of einspect
 * For someone without good knowledge of einspect, replacing all the "malloc" calls with MEMALLOC and
 * replacing all "free" calls with MEMFREE will help detect memory leaks
 */
#define MEMALLOC(size) MALLOC(size,__FUNCTION__,__LINE__)
#define MEMFREE(ptr) FREE(ptr)

/* Current Logging level. Default to nologs */
static short currentLogLevel= FATAL;

/* FILE_OPEN parameters */
const short SETMODE_FUNCTION = 72;
const short SETMODE_FUNCTION_PARAM_1 = 2;

/* Other Global variables */
static pthread_mutex_t gs_InitList_mutex;
static pthread_mutex_t gs_compList_mutex;

/*This condition is used to signal that there is a response available*/
static pthread_cond_t gs_response_available_cond;

/* Global declaration of JNI-specific variables */
static jclass channelData_class = NULL;
static jmethodID channelStatus_constructor = NULL;
static jmethodID channelData_constructor = NULL;
static jclass short_class = NULL;
static jmethodID short_constructor = NULL;
static int initialized = 0;
/* Structure holding data related to an IO transaction.*/
typedef struct IOEntry_s {
	short fileNum; /* This is the file number of the process opened */
	int tag; /* The tag is generated and passed to FS with every FS */
	/* call. This will uniquely identify an IO operation with */
	/* a particular process. */
	char* buffer; /* Buffer to hold data bytes to be written to the process.*/
	/* The buffer is also used for receiving the reply data. */
	long xferCount; /* Number of bytes received in a "read" operation. */
	long status; /* Holds the status of a "read" operation. */
} IOEntry_t;

/* Structure encapsulating an IO Entry and a pointer to the next similar node.
 * This structure is just a list-aware version of IOEntry_s structure,
 * thus used in any lists created for IOEntries.
 */
typedef struct IOEntryNode_s {
	IOEntry_t ioEntry;
	struct IOEntryNode_s* next;
} IOEntryNode_t;

/* Structure representing a list of IOEntries. The list contains a number of
 * IOEntryNode_s nodes each one linked to the next similar node. This
 * structure, IOEntryList_s, points to the first and the last nodes in the
 * list; this acts as single object keeping track of the whole list.
 */
/*TODO
 * use C++ list to hold all the entries. That will make the linked lists redundant and code much simpler to read and use
 */

struct IOEntryList_s {
	IOEntryNode_t* head;
	IOEntryNode_t* tail;
} ioInitiatedList, ioCompletedList;

/* This struct is expected to keep the file open options */
typedef struct FileOpenOptions {
	char pname[128]; /* This assumes the process name length is not longer than 127 character */
	short access;
	short exclusion;
	short nowaitDepth;
} OPEN_OPTIONS_T;

/* FUNCTION DECLARATIONS */
static void initializeStatusClass(JNIEnv *env);
static jobject createShortObject(JNIEnv *env, jshort num);
static jobject createStatusObjectFrom(JNIEnv *env, short filenum, int code);
static jobject createStatusObject(JNIEnv *env, short filenum, int code,
		char* descr);
static jobject createDataObject(JNIEnv *env, short filenum, int code, int tag,
		char* data, int dataLength);
static void logMsg(int level, char *format, ...);
static void setLogLevel(short level);
static short getLogLevel();
static void getErrorDesciption(int error, char description[]);
static int getErrorValue(JNIEnv *, char *);
static void printList(IOEntryNode_t* listHead);
static void *my_malloc(size_t size);

/* FUNCTION DEFINITIONS */
/* Memory leak detection functions */
typedef struct leakloc {
	char location[100];
	int address;
} LEAKLOC;

#define MAX_LOCATIONS 50
LEAKLOC locs[MAX_LOCATIONS];
static int endanalysis = 0;
static int leakLocs_initialized = 0;

static void initLeakLocation() {
	int i = 0;
	if (leakLocs_initialized == 1)
		return;
	for (i = 0; i < MAX_LOCATIONS; i++) {
		locs[i].address = 0;
	}
	leakLocs_initialized = 1;
}

static void printLeakLocation() {
	int i = 0;
	for (i = 0; i < MAX_LOCATIONS; i++) {
		printf("%s.%d\n", locs[i].location, locs[i].address);
	}
}

static void addLeakLocation(char *func, int line, int address) {
	int i = 0;
	int found = 0;
	if (endanalysis == 1)
		return;
	initLeakLocation();
	for (i = 0; i < MAX_LOCATIONS; i++) {
		if (locs[i].address == 0) {
			locs[i].address = address;
			sprintf(locs[i].location, "%s:%d", func, line);
			found = 1;
			break;
		}
	}
	if (found == 0) {
		endanalysis = 1;
		printLeakLocation();
	}
}

static void removeLeakLocation(int address) {
	int i = 0;
	if (endanalysis == 1)
		return;
	for (i = 0; i < MAX_LOCATIONS; i++) {
		if (locs[i].address == address) {
			locs[i].address = 0;
			break;
		}
	}
}

void *MALLOC(size_t size, char *func, int line) {
	void *ptr = (void *) NULL;
	ptr = my_malloc(size);
	addLeakLocation(func, line, (int) ptr);
	return ptr;
}

void FREE(void *ptr) {
	removeLeakLocation((int) ptr);
	free(ptr);
}

/*End memory leak detection functions*/

/* Helper functions to locate mutex locks and unlocks */
/* Currently everything is fine. Just in case something goes wrong in teh future these functions can be used */
static int my_mutex_lock(pthread_mutex_t *mutex, char *func, int line) {
	int ret;
	ret = pthread_mutex_lock(mutex);
	if (ret!=0) {
		logMsg(FATAL, "%s.%d:pthread_mutex_lock [%d][%s]",func,line, ret, strerror(ret));
	} else {
		logMsg(DEBUG,"%s.%d:successfully obtained mutex lock",func,line);
	}
	return ret;
}

static int my_mutex_unlock(pthread_mutex_t *mutex, char *func, int line) {
	int ret;
	ret = pthread_mutex_unlock(mutex);
	if (ret!=0) {
		logMsg(FATAL, "%s.%d:pthread_mutex_unlock [%d][%s]",func,line, ret, strerror(ret));
	} else {
		logMsg(DEBUG,"%s.%d:successfully unlocked mutex",func,line);
	}
	return ret;

}

/*END Mutex functions*/

/* BEGIN: Generic functions for IOEntry list manipulation. */
/*TODO:
 * This method should not be used at all. JNI should be communicating its state using NativeStatus object ONLY
 * and  not through exceptions
 */
static void throwUnsatisfiedLinkError(JNIEnv *env, const char *msg) {
	jclass clazz = (*env)->FindClass(env, "java/lang/UnsatisfiedLinkError");
	if (clazz == 0)
		return;
	(*env)->ThrowNew(env, clazz, msg);
	return;
}

/* Adds the passed entry to the passed list */
static void addIOEntry(struct IOEntryList_s* ioEntryList, IOEntry_t ioEntry) {
	IOEntryNode_t *newNode = (IOEntryNode_t *) my_malloc(sizeof(IOEntryNode_t));

	newNode->ioEntry = ioEntry;
	newNode->next = NULL;

	if (ioEntryList->head == NULL) {
		ioEntryList->head = newNode;
	} else {
		ioEntryList->tail->next = newNode;
	}
	ioEntryList->tail = newNode;
}

/* Find the IOEntry in the passed list, whose fileNum and tag matches
 * with passed fileNum and tag. The entry is also removed from the list.
 * Returns NULL, if no entry is matching with the passed fileNum and tag.
 *
 * IMPORTANT: The function invoking this method should free the pointer returned by this function
 */
static IOEntry_t* findAndRemoveIOEntry(struct IOEntryList_s* ioEntryList,
		short fileNum, int tag) {
	IOEntryNode_t *temp;
	IOEntry_t* result;
	IOEntryNode_t* current = ioEntryList->head;

	if (current == NULL) {
		logMsg(TRACE, "findAndRemoveIOEntry: The list is empty");
		return NULL;
	}

	if ((current->ioEntry.fileNum == fileNum) && (current->ioEntry.tag == tag)) {
		ioEntryList->head = current->next;
		result = (IOEntry_t*) my_malloc(sizeof(IOEntry_t));
		result->buffer = current->ioEntry.buffer;
		result->fileNum = current->ioEntry.fileNum;
		result->tag = current->ioEntry.tag;
		result->xferCount = current->ioEntry.xferCount;
		result->status = current->ioEntry.status;
		free(current);

		if (ioEntryList->head == NULL) {
			/* If we removed the last node from the list, we should also
			 * make the "tail" to point to null. */
			ioEntryList->tail = NULL;
		}

		return result;
	}

	if (current->next == NULL) {
		return NULL;
	}

	/* Current node is not matching the criteria. So,
	 * start checking the nodes after the current node.
	 */
	do {
		if ((current->next->ioEntry.fileNum == fileNum)
				&& (current->next->ioEntry.tag == tag)) {
			temp = current->next;
			current->next = current->next->next;
			result = (IOEntry_t*) my_malloc(sizeof(IOEntry_t));
			result->buffer = temp->ioEntry.buffer;
			result->fileNum = temp->ioEntry.fileNum;
			result->tag = temp->ioEntry.tag;
			result->xferCount = temp->ioEntry.xferCount;
			result->status = temp->ioEntry.status;
			free(temp);

			if (current->next == NULL) {
				/* If we removed the last node from the list, we should also
				 * make the "tail" to point to current node. */
				ioEntryList->tail = current;
			}

			return result;
		}

		current = current->next;
	} while (current->next != NULL);

	return NULL;
}

/*
 * IMPORTANT; The method calling this function should free the pointer returned by this function
 */
static IOEntry_t* findAndRemoveOldestEntry(struct IOEntryList_s* ioEntryList) {
	IOEntry_t* ret = NULL;
	/*
	 * First get the head. If it is not null then return the head and replace the
	 * head's next as the new head.
	 * if the new head is null, then it means that we have come to the end of the list
	 * so "nullify" the tail too.
	 */
	IOEntryNode_t* result = ioEntryList->head;

	if (result == NULL) {
		logMsg(TRACE, "findAndRemoveOldestEntry: List is empty");
		return NULL;
	}
	ioEntryList->head = result->next;

	ret = (IOEntry_t *) my_malloc(sizeof(IOEntry_t));
	ret->fileNum = result->ioEntry.fileNum;
	ret->status = result->ioEntry.status;
	ret->tag = result->ioEntry.tag;
	ret->xferCount = result->ioEntry.xferCount;
	ret->buffer = result->ioEntry.buffer;
	free(result);

	if (ioEntryList->head == NULL) {
		ioEntryList->tail = NULL;
	}

	return ret;
}

static IOEntry_t* findIOEntry(struct IOEntryList_s* ioEntryList, short fileNum) {
	IOEntryNode_t *current;
	IOEntryNode_t* head = ioEntryList->head;

	if (head == NULL) {
		logMsg(TRACE, "findIOEntry: The list is empty");
		return NULL;
	} else {
		current = head;

		/* Current node is not matching the criteria. So,
		 * start checking the nodes after the current node.
		 */
		do {
			if ((current->ioEntry.fileNum == fileNum)) {
				return &(current->ioEntry);
			}

			current = current->next;
		} while (current != NULL);
	}
	return NULL;
}

/* Removes all the IOEntries from the passed list, whose fileNum matches with
 * the passed fileNum.
 */
/*NOTE:
 * This does not iteratively search through the list for all entries to fileNum
 * because there is a contract between the java code and teh code that only 1 outstanding IO is permitted
 * per file open (fileNum) !!
 */
static void removeIOEntry(struct IOEntryList_s* ioEntryList, short fileNum) {
	IOEntryNode_t *temp;
	IOEntryNode_t* current = ioEntryList->head;

	if (current == NULL) {
		/* The list is empty, so nothing to remove */
		logMsg(TRACE, "removeIOEntry: The list is empty");
		return;
	} else {
		/* Keep on removing the head entries till they match "fileNum" */
		while ((current != NULL) && (current->ioEntry.fileNum == fileNum)) {
			temp = current;
			current = current->next;
			free(temp->ioEntry.buffer);
			free(temp);
		}

		/* Reset the head to point to the first un-removed node. */
		ioEntryList->head = current;

		/* Set the tail to null if we removed all the nodes till the end */
		if (ioEntryList->head == NULL) {
			ioEntryList->tail = NULL;
		}

		if (current == NULL || current->next == NULL) {
			/* We already traversed through all nodes till "current" node and
			 * the next node is null, so there is nothing more to remove. */
			return;
		}

		/* Current node is not matching the criteria. So,
		 * start checking the nodes after the current node.
		 */
		do {
			if ((current->next->ioEntry.fileNum == fileNum)) {
				temp = current->next;
				current->next = current->next->next;
				free(temp->ioEntry.buffer);
				free(temp);
			} else {
				current = current->next;
			}
		} while (current->next != NULL);

		ioEntryList->tail = current;
	}
}
/* END: Generic functions for list manipulation */

/* Call back function to receive IO Completion notifications */
static void fileIOHandler(const short filenum, const long tag,
		const long xferCount, const long error, void *userdata) {

	IOEntry_t* ioEntry;

	logMsg(TRACE, "fileIOHandler: ioInitiatedList before findAndRemoveIOEntry=");
	printList(ioInitiatedList.head);

	/* Move the IO entry from Initiated list to Completed list */
	{
		my_mutex_lock(&gs_InitList_mutex, __FUNCTION__, __LINE__);
		ioEntry = findAndRemoveIOEntry(&ioInitiatedList, filenum, tag);
		my_mutex_unlock(&gs_InitList_mutex, __FUNCTION__, __LINE__);
	}

	logMsg(TRACE, "fileIOHandler: oInitiatedList after findAndRemoveIOEntry=");
	printList(ioInitiatedList.head);

	if (ioEntry != NULL) {
		IOEntry_t newEntry;
		newEntry.buffer = ioEntry->buffer;
		newEntry.fileNum = ioEntry->fileNum;
		newEntry.status = error;
		newEntry.tag = ioEntry->tag;
		newEntry.xferCount = xferCount;

		{
			my_mutex_lock(&gs_compList_mutex, __FUNCTION__, __LINE__);
			addIOEntry(&ioCompletedList, newEntry);
			my_mutex_unlock(&gs_compList_mutex, __FUNCTION__, __LINE__);
		}

		free(ioEntry);

		/*
		 * Now that we have added an entry into the completed list
		 * tell all the threads that are waiting on the condition
		 * when an entry is added to the completed list.
		 * There will be typically one thread that will be waiting.
		 * That thread would have called the method jniReadAny and
		 * if there is no response to be returned jniReadAny just waits
		 * for a response.
		 */
		pthread_cond_signal(&gs_response_available_cond);

		logMsg(TRACE, "fileIOHandler: ioCompletedList after moving entry from"
		" ioIntiatedList=");
		printList(ioCompletedList.head);
	}

	/* Notify the waiting threads about the completion of IO */
	put_wakeup(filenum, tag, xferCount, error);

}

/* BEGIN: JNI Functions (functions callable from Java) */
/*
 * Should not be initialized again and again. If once initialized set a flag which says that it is
 * initialsed.
 */
JNIEXPORT jobject JNICALL Java_com_hp_nonstop_io_process_impl_NSProcessChannel_jniInit(
		JNIEnv * env, jclass j_class) {

	if (initialized == 0) {
		int ret = 0;
		logMsg(TRACE, "jniInit called.");
		ret = pthread_mutex_init(&gs_InitList_mutex, NULL);
		HANDLEERROR("jniInit:pthread_mutex_init",-1,ret);
		ret = pthread_mutex_init(&gs_compList_mutex, NULL);
		HANDLEERROR("jniInit:pthread_mutex_init",-1, ret);
		ret = pthread_cond_init(&gs_response_available_cond, NULL);
		HANDLEERROR("jniInit:pthread_cond_init",-1, ret);

		initializeStatusClass(env);
		if (channelData_class == NULL || channelStatus_constructor == NULL || channelData_constructor == NULL) {
			char message[255] = "";
			sprintf(message, "Unable to initialize %s", STATUS_CLASSNAME);
			return createStatusObject(env, -1, 0, message);
		}

		/*TODO
		 * if any of the values are -1 then return failure to initialize
		 */
		JNI_ERROR = getErrorValue(env, "JNI_ERROR");
		AWAITIO_ERROR = getErrorValue(env, "AWAITIO_ERROR");
		INVALID_FILENUM_ERROR = getErrorValue(env, "INVALID_FILENUM_ERROR");
		NULL_PROCESS_NAME_ERROR = getErrorValue(env, "NULL_PROCESS_NAME_ERROR");
		NULL_REQDATA_ERROR = getErrorValue(env, "NULL_REQDATA_ERROR");
		NULL_REPLY_DATA_ERROR = getErrorValue(env, "NULL_REPLY_DATA_ERROR");
		TRANSFER_OVERFLOW_ERROR = getErrorValue(env, "TRANSFER_OVERFLOW_ERROR");
		INVALID_STATE = getErrorValue(env, "INVALID_STATE");
		CLOSE_INTERRUPED = getErrorValue(env, "CLOSE_INTERRUPED");
		CLASS_NOT_FOUND = getErrorValue(env, "CLASS_NOT_FOUND");
		TIMEDOUT = getErrorValue(env, "TIMEDOUT");
		/* Used SYSTEMENTRYPOINT_RISC_ to check for the existance of FILE_WRITEREADL_ since this library  can be tested on older RVU's where FILE_WRITEREADL_ is not present*/
		fptr_file_writereadl_ = (file_writereadl_) SYSTEMENTRYPOINT_RISC_(
				"FILE_WRITEREADL_", (short) strlen("FILE_WRITEREADL_"));

		initialized = 1;
	}
	return createStatusObject(env, -1, 0, "Successfully Initialized");
}

JNIEXPORT jobject JNICALL Java_com_hp_nonstop_io_process_impl_NSProcessChannel_jniOpen(
		JNIEnv * env, jclass class, jobject joptions) {
	short fileNum;
	short status;

	put_error_t put_status;
	_cc_status cc;
	short error = 0;
	OPEN_OPTIONS_T options;

	HANDLEINITERROR("jniOpen");
	logMsg(TRACE, "jniOpen:");

	if (joptions == NULL) {
		return createStatusObject(env, -1, JNI_ERROR,
				"Process open options is NULL");
	}
	/* This block just extracts the contents of 'joptions' object to 'options' structure */
	{
		jmethodID getter;
		jclass options_class = (*env)->FindClass(env, OPENOPTIONS_CLASSNAME);

		if (options_class == NULL) {
			char message[128];
			sprintf(message, "Could not find class %s", OPENOPTIONS_CLASSNAME);
			return createStatusObject(env, -1, JNI_ERROR, message);
		}

		{
			/* Now first get the process name */
			jstring jpname;
			int plen;

			getter = (*env)->GetMethodID(env, options_class, "getProcessName",
					"()Ljava/lang/String;");

			if (getter == NULL) {
				char message[128];
				sprintf(
						message,
						"Unable to locate the method getProcessName with signature ()Ljava/lang/String;");
				return createStatusObject(env, -1, JNI_ERROR, message);
			}

			jpname = (*env)->CallObjectMethod(env, joptions, getter);
			if (jpname == NULL) {
				/* We do not expect jpname to be null. If it is then an exception could be thrown. Check ...*/
				jthrowable ex = (*env)->ExceptionOccurred(env);
				if (ex == NULL) {
					/*There is no exception either. This means that jname is indeed NULL*/
					char message[128];
					sprintf(message, "Process name is NULL");
					return createStatusObject(env, -1, JNI_ERROR, message);
				} else {
					/* We do not have to do anything here. There is indeed an exception and it will be thrown */
					return NULL;
				}
			}
			memset(options.pname, NULL, sizeof(options.pname));
			plen = (*env)->GetStringLength(env, jpname);
			/* assuming that plen is not greater than sizeof(options.pname) */
			(*env)->GetStringUTFRegion(env, jpname, 0, plen, options.pname);
		}

		{
			/* Now get the access parameter value */
			jshort jaccess;
			getter
			= (*env)->GetMethodID(env, options_class, "getAccess",
					"()S");
			if (getter == NULL) {
				char message[128];
				sprintf(message,
						"Unable to locate the method getAccess with signature ()S");
				return createStatusObject(env, -1, JNI_ERROR, message);
			}
			jaccess = (*env)->CallShortMethod(env, joptions, getter);
			options.access = jaccess;
		}

		{
			/* Now get the exclusion parameter value */
			jshort jexclusion;
			getter = (*env)->GetMethodID(env, options_class, "getExclusion",
					"()S");
			if (getter == NULL) {
				char message[128];
				sprintf(message,
						"Unable to locate the method getExclusion with signature ()S");
				return createStatusObject(env, -1, JNI_ERROR, message);
			}
			jexclusion = (*env)->CallShortMethod(env, joptions, getter);
			options.exclusion = jexclusion;
		}

		{
			/* Now get the nowaitdepth parameter value */
			jshort jnowaitDepth;
			getter = (*env)->GetMethodID(env, options_class, "getNowaitDepth",
					"()S");
			if (getter == NULL) {
				char message[128];
				sprintf(message,
						"Unable to locate the method getNowaitDepth with signature ()S");
				return createStatusObject(env, -1, JNI_ERROR, message);
			}
			jnowaitDepth = (*env)->CallShortMethod(env, joptions, getter);
			if(jnowaitDepth != 1) {
				char message[128];
				sprintf(message,
						"Nowait depth should be 1. The value is %d",jnowaitDepth);
				return createStatusObject(env, -1, JNI_ERROR, message);

			}
			options.nowaitDepth = jnowaitDepth;
		}
	}

	logMsg(
			TRACE,
			"jniOpen: About to call FILE_OPEN with following parameters [%s][access:%d][exclusion:%d][nowait-depth:%d].",
			options.pname, options.access, options.exclusion,
			options.nowaitDepth);

	/* Open connection to the process */
	status = FILE_OPEN_(options.pname, (short) strlen(options.pname), &fileNum,
			options.access, options.exclusion, options.nowaitDepth);

	logMsg(TRACE, "jniOpen: After file open.");

	if (status) {

		logMsg(ERROR, "jniOpen: File Open failed with status %d. Filenum[%d]",
				status, fileNum);
		return createStatusObjectFrom(env, fileNum, status);
	}

	/*User buffers are allowed for this file after this call
	 regardless of the previous setting. User buffers being
	 allowed does not guarantee that the user buffers will be
	 used; the system is still free to select the
	 most efficient buffers to use. In practice, I/O less than
	 4096 bytes will use PFS buffers*/

	/*README THOROUGHLY
	 * we could have enabled use of user I/O buffer everywhere by invoking useUserIOBuffer function.
	 * We did not do that because at the time of writing this comment there seems to be a bug with user IO buffer
	 * and terminal IO. What this means is if the use of user IO buffer is enabled globally then some system calls
	 * that communicate with the terminal (such as printf) halt the CPU. The exact reason for this halt is being
	 * investigated.
	 * So unless you know exactly what you are doing do not remove the SETMODE function below
	 */

	cc = SETMODE(fileNum, SETMODE_FUNCTION, SETMODE_FUNCTION_PARAM_1);
	if (_status_lt(cc)) {
		FILE_GETINFO_(fileNum, &error);
		logMsg(ERROR, "jniOpen: SETMODE failed with an error %d", error);
		return createStatusObjectFrom(env, fileNum, error);
	}

	logMsg(TRACE, "jniOpen: Opened file number=%d.", fileNum);

	/* Register fileIOHandler() method as a call back method, for
	 * any IO completions on the opened file. This call back is
	 * required to awake the threads, if any, that waiting to receive
	 * the reply after a write operation has been done.
	 */

	put_status = put_regFileIOHandler(fileNum, (put_FileIOHandler_p)
			& fileIOHandler);

	if (put_status != PUT_SUCCESS) {
		logMsg(ERROR, "jniOpen: Error '%d' thrown on registering a file"
				" IO Handler.", status);

		/* Close the open file connection */
		status = FILE_CLOSE_(fileNum);

		if (status) {
			logMsg(ERROR, "jniOpen: Error in closing the just opened file:%d",
					status);
		}

		/* Return Error code*/
		return createStatusObjectFrom(env, fileNum, put_status);
	}

	return createShortObject(env, fileNum);
}

/* Writes the passed request data to open file connection
 * The request data should not exceed the maximum limit for 57344.
 */
JNIEXPORT jobject JNICALL Java_com_hp_nonstop_io_process_impl_NSProcessChannel_jniWrite(
		JNIEnv *env, jclass j_object, jshort j_fileNum, jint j_tag,
		jbyteArray j_reqData, jint j_reqSize, jint j_resSize) {
	short fileNum;
	int tag;
	int reqData_len;
	int status;
	int reqMsgSize, resMsgSize, isLargeMessage = 0;
	jbyte* reqData;
	IOEntry_t ioEntry;

	HANDLEINITERROR("jniWrite");

	fileNum = (short) j_fileNum;
	tag = j_tag;
	reqMsgSize = (int) j_reqSize;
	resMsgSize = (int) j_resSize;

	if (fileNum < 0) {
		logMsg(ERROR, "jniWrite: Invalid fileNum %d", fileNum);
		return createStatusObject(env, fileNum, INVALID_FILENUM_ERROR,
				"File Number must be a whole number.");
	}

	if (j_reqData == NULL) {
		logMsg(ERROR, "jniWrite: fileNum=%d : Null Request Data", fileNum);
		return createStatusObject(env, fileNum, NULL_REQDATA_ERROR,
				"Request data cannot be null.");
	}

	reqData_len = (*env)->GetArrayLength(env, j_reqData);
	if (reqData_len> IO_BUFFER_MAX_SIZE) {
		logMsg(ERROR, "jniWrite: fileNum=%d: Message length exceeds the"
				" supported limit. Message length = %d ", fileNum, reqData_len);
		return createStatusObject(env, fileNum, TRANSFER_OVERFLOW_ERROR,
				"Request-message size exceeds the supported message limit.");
	}

	/* Initialize the IOEntry */
	ioEntry.fileNum = fileNum;
	ioEntry.tag = tag;
	ioEntry.xferCount = 0;
	ioEntry.status = 0;
	if (reqMsgSize> resMsgSize) {
		ioEntry.buffer = (char *) my_malloc(reqMsgSize + 1);
		if (reqMsgSize> DEFAULT_IO_BUFFER_SIZE)
		isLargeMessage = 1;
	} else {
		ioEntry.buffer = (char *) my_malloc(resMsgSize + 1);
		if (resMsgSize> DEFAULT_IO_BUFFER_SIZE)
		isLargeMessage = 1;
	}

	reqData = (*env)->GetByteArrayElements(env, j_reqData, 0);
	memcpy(ioEntry.buffer, (char *) reqData, reqData_len);
	ioEntry.buffer[reqData_len] = '\0';
	(*env)->ReleaseByteArrayElements(env, j_reqData, reqData, JNI_ABORT);

	logMsg(TRACE, "jniWrite: About to write: fileNum=%d, reqData_len=%d",
			fileNum, reqData_len);

	if (isLargeMessage) {
		logMsg(TRACE, "jniWrite: Inside FILE_WRITEREADL_");
		if (fptr_file_writereadl_ == NULL) {
			free(ioEntry.buffer);
			/*
			 * TODO
			 * Very bad strategy. Why is this throwing an exception while all other methods just return an object ????
			 */
			throwUnsatisfiedLinkError(env,
					"Unable to find symbol : FILE_WRITEREADL_");
			return;
		}

		/*TODO
		 * FS team thinks that using the same buffer for both input and output will turn off using user IO buffer
		 * check this.
		 */
		status = fptr_file_writereadl_(fileNum, ioEntry.buffer,, reqData_len,resMsgSize, , tag);
		logMsg(TRACE, "jniWrite: After FILE_WRITEREADL_");

		if (status) {
			logMsg(ERROR, "jniWrite: FILE_WRITEREADL_ failed for fileNum=%d"
					" with error=%d.", fileNum, status);
			free(ioEntry.buffer);
			return createStatusObjectFrom(env, fileNum, status);
		}
	} else {
#ifdef _LP64

       status=FILE_WRITEREAD64_(fileNum, ioEntry.buffer, ,reqData_len, resMsgSize, , tag);
#else
       status = WRITEREADX(fileNum, ioEntry.buffer, (unsigned short) reqData_len, resMsgSize, , tag);
#endif

			logMsg(TRACE, "jniWrite: After WRITEREADX");

		if (status) {
			short error = 0;
			FILE_GETINFO_(fileNum, &error);
			logMsg(ERROR, "jniWrite: WRITEREADX failed for fileNum=%d"
					" with error=%d.", fileNum, error);
			free(ioEntry.buffer);
			return createStatusObjectFrom(env, fileNum, error);
		}
	}

	{
		my_mutex_lock(&gs_InitList_mutex,__FUNCTION__,__LINE__);
		addIOEntry(&ioInitiatedList, ioEntry);
		my_mutex_unlock(&gs_InitList_mutex,__FUNCTION__,__LINE__);
	}

	return createStatusObjectFrom(env, fileNum, status);
}

/* The timeout value (last parameter) is in seconds.
 */
/*TODO
 * Remove this entire function jniRead. It has to be first removed from NSProcessChannel.java
 * and then here
 */
JNIEXPORT jobject JNICALL Java_com_hp_nonstop_io_process_impl_NSProcessChannel_jniRead(
		JNIEnv *env, jclass j_class, jshort fileNum, jint ioTag, jint j_timeOut) {

	IOEntry_t* ioEntry;
	int error;
	int put_error;
	int xferCount;
	int timeOut;
	jobject dataObject = NULL;

	HANDLEINITERROR("jniRead");

	if (fileNum < 0) {
		logMsg(ERROR, "jniRead: Invalid fileNum %d", fileNum);
		return createStatusObject(env, fileNum, INVALID_FILENUM_ERROR,
				"File number cannot be negative.");
	}

	logMsg(TRACE, "jniRead: fileNum=%d, tag=%d.", fileNum, ioTag);

	/* spt_awaitio accepts timeout value in 100th of a seconds. Hence the
	 * passed parameter is multiplied by 100 before passing to spt_awaitio.
	 */
	timeOut = ((j_timeOut> 0) ? j_timeOut * 100 : j_timeOut);

	{
		my_mutex_lock(&gs_compList_mutex,__FUNCTION__,__LINE__);
		ioEntry = findAndRemoveIOEntry(&ioCompletedList, fileNum, ioTag);
		my_mutex_unlock(&gs_compList_mutex,__FUNCTION__,__LINE__);
	}

	if (ioEntry == NULL) {
		/* Wait for the IO to complete. */

		/* No log statements should be added here (between the calls to
		 * findAndRemoveIOEntry and spt_awaitio); to avoid any context-switch
		 * to regFileIOHandler thread. */

		//spt_error = spt_awaitio(fileNum, ioTag, timeOut, (long *) &xferCount,
		#ifdef _LP64
		put_error = put_awaitio(fileNum, ioTag, timeOut,  &xferCount,
				&error, NULL);
		#else
		put_error = put_awaitio(fileNum, ioTag, timeOut,  &xferCount,
				&error, NULL);
		#endif

		logMsg(TRACE, "jniRead: xferCount=%d.", xferCount);

		ioEntry = findAndRemoveIOEntry(&ioCompletedList, fileNum, ioTag);

		if (put_error || (error && (error != FE_CONTINUE)) || (ioEntry == NULL)) {
			/* We will be here in case of any FS error, Timeout or an Interrupt */

			logMsg(TRACE, "jniRead: put_awaitio failed : fileNum=%d, ioTag=%d,"
								" error=%d, put_error=%d.", fileNum, ioTag, error, put_error);

			if ((error == FILE_NOT_OPEN_ERROR_CODE)) {
				/* We will be here only if the read operation was interrupted by
				 * file closure operation. In this case, we should not return
				 * any error, but just an empty byte array.
				 */

				logMsg(TRACE, "jniRead: Read operation interrupted by file"
						" closure.");

				return createDataObject(env, fileNum, 0, 0, NULL, 0);
			}

			if (error) {
				return createStatusObjectFrom(env, fileNum, error);
			}

			if (put_error) {
				return createStatusObjectFrom(env, fileNum, put_error);
			}

			return createStatusObject(env, fileNum, AWAITIO_ERROR,
					"Reply data not found in completed list.");

			/* QUESTION: Do we need to cancel the request or are we expecting
			 * user to issue "read" once again?
			 */
		}
	} else {
		logMsg(TRACE, "jniRead: IO Already completed.");
	}

	if (ioEntry->xferCount> IO_BUFFER_MAX_SIZE) {
		/* This should never happen */
		logMsg(TRACE,
				"jniRead: xferCount '%d' exceeds the maximum buffer limit.",
				ioEntry->xferCount);

		free(ioEntry->buffer);
		free(ioEntry);

		return createStatusObject(env, fileNum, TRANSFER_OVERFLOW_ERROR,
				"transferCount exceeds the maximum buffer limit");
	}

	logMsg(TRACE, "jniRead: Read Entry: xferCont=%d, status=%d: ",
			ioEntry->xferCount, ioEntry->status);

	if (ioEntry->xferCount < IO_BUFFER_MAX_SIZE) {

		/* This "if" block is used just for logging of the buffer */
		ioEntry->buffer[ioEntry->xferCount] = '\0';
		{
			char temp[2000];
			memcpy(temp, ioEntry->buffer, sizeof(temp) - 1);
			temp[sizeof(temp) - 1] = '\0';
			logMsg(TRACE,
					"jniRead: Read Entry: buffer[printing max 2000 chars]"
					"=%s", temp);
		}
	}

	dataObject = createDataObject(env, fileNum, error, ioEntry->tag,
			ioEntry->buffer, ioEntry->xferCount);

	/* Free up the read char-array and the IOEntry itself */
	free(ioEntry->buffer);
	free(ioEntry);

	return dataObject;
}

JNIEXPORT jobject JNICALL Java_com_hp_nonstop_io_process_impl_NSProcessChannel_jniReadAny(
		JNIEnv * env, jclass j_class, jint j_timeOut) {
	IOEntry_t *ioEntry;
	int timeout = 0;
	int ret = 0;

	HANDLEINITERROR("jniReadAny");

	timeout = (int) j_timeOut;

	logMsg(TRACE, "jniReadAny: Trying to get the lock");

	my_mutex_lock(&gs_compList_mutex,__FUNCTION__,__LINE__);
	ioEntry = findAndRemoveOldestEntry(&ioCompletedList);

	/*
	 * READ CAREFULLY TO UNDERSTAND HOW THE MUTEXes ARE USED
	 * First get hold of the mutex gs_compList_mutex. this is the mutex used to make
	 * modifications to the completed IO list.
	 * After getting hold of this mutex then the conditioned timed wait is invoked
	 * with the same mutex. the function pthread_cond_timedwait first releases the mutex
	 * and then either waits for a condition signal (given by fileIOHandler) or a timeout.
	 * Once pthread_cond_timedwait actually obtains the mutex before completing the call
	 * and handing over the control to the thread that invoked the cond_timedwait call.
	 * Once the thread gets control from the cond_timedwait it checks to see if any entry has
	 * been made to ioCompletedList and then releases the mutex.
	 */

	logMsg(
			TRACE,
			"jniReadAny: This is what I got from findAndRemoveOldestEntry: %ld",
			ioEntry);
	if (ioEntry == NULL) {
		int maxWaitTime = 10000000;
		struct timeval tv;
		struct timespec ts;

		gettimeofday(&tv, NULL);
		ts.tv_sec = tv.tv_sec + (timeout == -1 ? maxWaitTime : timeout);
		ts.tv_nsec = tv.tv_usec * 1000;

		logMsg(TRACE, "jniReadAny:executing conditioned wait");
		ret = pthread_cond_timedwait(&gs_response_available_cond,
				&gs_compList_mutex, &ts);
		logMsg(TRACE, "jniReadAny: pthread_cond_timedwait [%d][%s]", ret,
				strerror(ret));

		ioEntry = findAndRemoveOldestEntry(&ioCompletedList);
	}
	my_mutex_unlock(&gs_compList_mutex,__FUNCTION__,__LINE__);

	if (ret == ETIMEDOUT) {
		/*If the wait timeout. That is timeout specified by the user expired*/
		return createStatusObject(env, -1, TIMEDOUT, "Operation Timed Out");
	}
	if (ioEntry == NULL) {
		/* This condition should normally not occur. It can occur if the user application has called both
		 * readAny and read in two different threads and the "read" thread executes first thus returning data
		 */
		return createStatusObject(env, -1, INVALID_STATE,
				"Response data already read");
	}
	/*At this point we can be pretty sure that we have either the right data or an FS error*/
	{
		jobject dataObject;

		if (ioEntry->status != FE_CONTINUE && ioEntry->status != 0) {
			/*In case the file system returned error*/
			/* On occurance of a FS error the buffer will
			 * remain unchanged and its redundant to send
			 * the unmodified buffer back. Sending some junk string.
			 * This string will anyway not be used. This string is just
			 * for proper creating of the data object
			 */
			char dummy[10]="**ERROR**";
			dataObject = createDataObject(env, ioEntry->fileNum, ioEntry->status, ioEntry->tag, dummy,strlen(dummy));
		} else {
			dataObject = createDataObject(env, ioEntry->fileNum, ioEntry->status,
					ioEntry->tag, ioEntry->buffer, ioEntry->xferCount);
		}
		/* Free up the read char-array and the IOEntry itself */
		free(ioEntry->buffer);
		free(ioEntry);
		return dataObject;
	}
}

/*NOTE
 * This function does not explicitly cancel and IO
 * Its left to the file system to do all the clean up of outstanding IOs
 */
JNIEXPORT jobject JNICALL Java_com_hp_nonstop_io_process_impl_NSProcessChannel_jniClose(
		JNIEnv * env, jclass j_class, jshort j_fileNum) {
	short error;
	put_error_t put_error;
	short fileNum = (short) j_fileNum;

	HANDLEINITERROR("jniClose");

	if (fileNum < 0) {
		logMsg(ERROR, "jniClose: Invalid fileNum %d", fileNum);
		return createStatusObject(env, fileNum, INVALID_FILENUM_ERROR,
				"File number cannot be negative.");
	}

	//logMsg(TRACE, "jniClose: Before spt_unregFile for fileNum=%d", fileNum);
	logMsg(TRACE, "jniClose: Before put_unregFile for fileNum=%d", fileNum);

	//spt_unregFile(fileNum);
	put_unregFile(fileNum);

	/*Interrupt all the spt_awaitio threads, waiting on this file number*/

	//spt_error = spt_interrupt(fileNum, SPT_INTERRUPTED);
	put_error = put_interrupt(fileNum, PUT_INTERRUPTED);

	//if (spt_error == -1) {
	//	logMsg(ERROR, "jniClose: spt_interrupt failed with an error %d",
	//			spt_error);
	//}
	if (put_error == -1) {
		logMsg(ERROR, "jniClose: put_interrupt failed with an error %d",
				put_error);
	}

	logMsg(TRACE, "jniClose: About to close fileNum=%d", fileNum);
	error = FILE_CLOSE_(fileNum);
	logMsg(TRACE, "jniClose: After FILE CLOSE. fileNum=%d", fileNum);

	if (error != 0) {
		logMsg(ERROR, "jniClose: File Close failed with status %d", error);
	}

	/* Clean up all the IOEntries from Initiated and Completed lists,
	 * matching with this file number*/
	{
		my_mutex_lock(&gs_InitList_mutex,__FUNCTION__,__LINE__);
		removeIOEntry(&ioInitiatedList, fileNum);
		my_mutex_unlock(&gs_InitList_mutex,__FUNCTION__,__LINE__);

		my_mutex_lock(&gs_compList_mutex,__FUNCTION__,__LINE__);
		removeIOEntry(&ioCompletedList, fileNum);
		my_mutex_unlock(&gs_compList_mutex,__FUNCTION__,__LINE__);
	}

	return createStatusObjectFrom(env, fileNum, error);
}

/*
 * JNI Method for setting the log level for this file.
 * Class:     com_hp_tandem_io_NSProcessChannel
 * Method:    jniSetLogLevel
 * Signature: (I)V
 */
JNIEXPORT jobject JNICALL Java_com_hp_nonstop_io_process_impl_NSProcessChannel_jniSetLogLevel(
		JNIEnv *env, jclass class, jshort level) {
	HANDLEINITERROR("jniSetLogLevel");
	setLogLevel(level);
	return createStatusObject(env, -1, 0, "Successfully set log level");
}

/* END: JNI Functions (functions callable from Java). */

/* BEGIN: Functions to create different java objects */

static int getErrorValue(JNIEnv *env, char* field) {
	jfieldID id;
	id = (*env)->GetStaticFieldID(env, channelData_class, field, "I");
	if (id == NULL) {
		return -1;
	}
	{
		jint value;
		value = (*env)->GetStaticIntField(env, channelData_class, id);
		return (int) value;
	}
}

static void initializeStatusClass(JNIEnv *env) {
	if (channelData_class == NULL) {
		jclass replyClass;
		replyClass = (*env)->FindClass(env, STATUS_CLASSNAME);

		if (replyClass == NULL) {
			logMsg(ERROR, "createStatusObject: Unable to find the class %s.",
			STATUS_CLASSNAME);
			return;
		}
		channelData_class = (*env)->NewGlobalRef(env, replyClass);

		if (channelData_class == NULL) {
			logMsg(ERROR, "createStatusObject: Unable to create GlobalRef"
			" for class %s.", STATUS_CLASSNAME);
			return;
		}
	}

	if (channelStatus_constructor == NULL) {
		const char* INIT_SIGN = "(SILjava/lang/String;)V";
		channelStatus_constructor = (*env)->GetMethodID(env, channelData_class, "<init>", INIT_SIGN);
		if (channelStatus_constructor == NULL) {
			logMsg(ERROR, "createStatusObject: Unable to find the constructor"
			" '%s' in class %s.", INIT_SIGN, STATUS_CLASSNAME);
			return;
		}
	}

	if (channelData_constructor == NULL) {
		const char* INIT_SIGN = "(SII[B)V";
		channelData_constructor = (*env)->GetMethodID(env, channelData_class, "<init>", INIT_SIGN);
		if (channelData_constructor == NULL) {
			logMsg(ERROR, "createDataObject: Unable to find the constructor"
			" '%s' in class %s.", INIT_SIGN, STATUS_CLASSNAME);
			return;
		}
	}
}

/*The CANCEL( ) function is used to cancel the oldest incomplete operation on a Guardian
 file opened for nowait I/O. The canceled operation might or might not have had effects. */
JNIEXPORT jobject JNICALL Java_com_hp_nonstop_io_process_impl_NSProcessChannel_jniCancel(
		JNIEnv *env, jclass class, jshort fileNum, jint tag) {
	short error = 0;
	_cc_status cc;
	/*error = SPT_CANCEL(fileNum);*/
	cc = CANCEL(fileNum);
	if (_status_lt(cc)) {
		FILE_GETINFO_(fileNum, &error);
		logMsg(ERROR, "jniCancel: CANCEL failed with an error %d", error);
		return createStatusObjectFrom(env, fileNum, error);
	}

	/* Clean up the last IO initiated IOEntries from Initiated and Completed lists,
	 * matching with this file number*/
	{
		my_mutex_lock(&gs_InitList_mutex,__FUNCTION__,__LINE__);
		removeIOEntry(&ioInitiatedList, fileNum);
		my_mutex_unlock(&gs_InitList_mutex,__FUNCTION__,__LINE__);

		my_mutex_lock(&gs_compList_mutex,__FUNCTION__,__LINE__);
		removeIOEntry(&ioCompletedList, fileNum);
		my_mutex_unlock(&gs_compList_mutex,__FUNCTION__,__LINE__);
	}

	return createStatusObjectFrom(env, fileNum, error);
}

/* Creates an object java/lang/Short from the passed jshort number */
static jobject createShortObject(JNIEnv *env, jshort num) {
	char* SHORT_CLASSNAME = "java/lang/Short";
	jclass replyClass;
	jobject replyObject;
	const char* INIT_SIGN = "(S)V";

	if (short_class == NULL) {
		replyClass = (*env)->FindClass(env, SHORT_CLASSNAME);

		if (replyClass == NULL) {
			logMsg(ERROR, "createShortObject: Unable to find the class %s.",
			SHORT_CLASSNAME);
			return NULL;
		}
		short_class = (*env)->NewGlobalRef(env, replyClass);

		if (short_class == NULL) {
			logMsg(ERROR, "createShortObject: Unable to create GlobalRef"
			" for class %s.", SHORT_CLASSNAME);
			return NULL;
		}
	}

	if (short_constructor == NULL) {
		short_constructor = (*env)->GetMethodID(env, short_class, "<init>", INIT_SIGN);
		if (short_constructor == NULL) {
			logMsg(ERROR, "createShortObject: Unable to find the constructor"
			" '%s' in class %s.", INIT_SIGN, SHORT_CLASSNAME);
			return NULL;
		}
	}

	replyObject = (*env)->NewObject(env, short_class, short_constructor, num);

	return replyObject;
}

/* Creates an object of Status class, from the passed error code */
static jobject createStatusObjectFrom(JNIEnv *env, short filenum, int code) {
	char descr[1024];
	getErrorDesciption(code, descr);
	return createStatusObject(env, filenum, code, descr);
}

static jobject createStatusObject(JNIEnv *env, short filenum, int code,
		char* descr) {
	jobject replyObject;
	jstring descr_jstr;

	descr_jstr = (*env)->NewStringUTF(env, descr);

	if (descr_jstr == NULL) {
		logMsg(ERROR, "createStatusObject: Error in constructing a UTF"
		" java string from %s.", descr);
		return NULL;
	}

	replyObject = (*env)->NewObject(env, channelData_class, channelStatus_constructor,
			filenum, code, descr_jstr);

	replyObject = (*env)->NewObject(env, channelData_class, channelStatus_constructor,
			filenum, code, descr_jstr);
	/* The DeleteLocalRef call at the end of the loop ensures that the virtual
	 machine does not run out of the memory used to hold JNI references such as descr_jstr*/
	(*env)->DeleteLocalRef(env, descr_jstr);
	return replyObject;
}

static jobject createDataObject(JNIEnv *env, short filenum, int code, int tag,
		char* data, int dataLength) {
	jobject replyObject;
	jbyteArray jData;

	/* Convert the data char-array into jByteArray */
	jData = (*env)->NewByteArray(env, dataLength);

	logMsg(DEBUG, "createDataObject: jData array initialized with size %d",
	dataLength);

	(*env)->SetByteArrayRegion(env, jData, 0, dataLength, data);

	logMsg(TRACE, "createDataObject: jData array fetched with data.");

	if (jData != NULL) {
		logMsg(DEBUG, "createDataObject: jData array initialized with size %d",
		dataLength);
		(*env)->SetByteArrayRegion(env, jData, 0, dataLength, data);
		logMsg(DEBUG, "createDataObject: jData array fetched with data.");
		replyObject = (*env)->NewObject(env, channelData_class,
				channelData_constructor, filenum, code, tag, jData);
		(*env)->DeleteLocalRef(env, jData);
		logMsg(DEBUG, "createDataObject: return Object created.");
		return replyObject;
	} else {
		logMsg(ERROR, "createDataObject: Byte Array obtained is NULL.");
		return createStatusObject(env, filenum, NULL_REPLY_DATA_ERROR,
				"Byte Array obtained is NULL.");
	}
}

/* END: Functions to create different java objects */

/* BEGIN: Functions used for logging purpose */
static void logMsg(int level, char *format, ...) {
	char msg[4096];
	va_list argp;

	if (level < currentLogLevel) {
		return;
	}

	va_start(argp, format);
	vsprintf(msg, format, argp);
	va_end(argp);
	printf("%s\n", msg);
}

/* Gets the full description for an error, from its error code. The receive
 * buffer should be adequately big to receive the message description. */
static void getErrorDesciption(int error, char description[]) {

	if (error) {
		/* TODO: Get error description from the system.*/
		strcpy(description, "Unknown Description.");
		return;
	}
	strcpy(description, "Success.");
}

/* Logs the contents of the list, whose head is passed as the parameter.
 * This method is used only for logging.
 */
static void printList(IOEntryNode_t* listHead) {
	IOEntry_t ioEntry;
	IOEntryNode_t* current = listHead;

	if (getLogLevel() > TRACE)
		return;

	if (current == NULL) {
		logMsg(TRACE, " []\n");
		return;
	} else {
		logMsg(TRACE, " [");
		do {
			ioEntry = current->ioEntry;
			logMsg(TRACE, "  fileNum=%d, tag=%d, xferCount=%d, status=%d;",
			ioEntry.fileNum, ioEntry.tag, ioEntry.xferCount,
			ioEntry.status);
			current = current->next;
		} while (current != NULL);
		logMsg(TRACE, " ]\n");
	}
}

static void setLogLevel(short level) {
	if (level < TRACE) {
		currentLogLevel = TRACE;
	} else if (level > NOLOGS) {
		currentLogLevel = NOLOGS;
	} else {
		currentLogLevel = level;
	}
}

static short getLogLevel() {
	return currentLogLevel;
}

static void *my_malloc(size_t size) {
	void *ret;
	/*
	 * we are intentionally using malloc_pages instead of malloc.
	 * This is to prevent a 1 in a million chance of a corrupt
	 * file pointer touching a memory space locked by
	 * file system calls
	 *
	 */
	/*TODO
	 * doing a malloc_pages for allocating a few bytes ???
	 * Find a solution to this. Either elimate the need to malloc small memories or .......
	 */
	ret = malloc_pages(size);
	if (ret == NULL) {
		logMsg(ERROR, "malloc_pages() failed\n");
		exit(-1);
	}
	return ret;
}
/* END: Functions used for logging purpose. */

