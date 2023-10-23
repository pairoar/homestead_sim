#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#include "TaskInfo.h"
#include "Log.h"

#include "console.h"

static TaskContextType *TaskContexts[TASK_ID_MAX];
static SemaphoreHandle_t  mutex = NULL;


static void create_mutex(void) {
	vSemaphoreCreateBinary(mutex);

    if( mutex == NULL ) {
        /* The semaphore was created successfully and can be used. */
		console_print("Error: can't create mutex\n");
    }
}

static void delete_mutex(void) {
    vSemaphoreDelete(mutex);
    mutex = NULL;
}

static void lock(void) {
    xSemaphoreTake( mutex, portMAX_DELAY );
}

static void unlock(void) {
    xSemaphoreGive( mutex );
}

static void init(void) {
	create_mutex();
}

static void deinit(void) {
	delete_mutex();
}


int task_info_regist(TaskContextType *context) {
	if (context == NULL)
		return -1;

	if (context->id < 0 || context->id >= TASK_ID_MAX)
		return -1;

	/* already registed ? */
	console_print("[%s %04d] id=%d, name=%s, stack_size=%d\n", __func__, __LINE__, context->id, context->name, context->stackSize);

	lock();
	TaskContexts[context->id] = context;
	unlock();
	
	return 0;
}


int task_info_unregist(TaskContextType *context) {
	if (context == NULL)
		return -1;

	if (context->id < 0 || context->id >= TASK_ID_MAX)
		return -1;

	if (TaskContexts[context->id]->task == context->task) {
		lock();
		TaskContexts[context->id] = NULL;
		unlock();
		return 0;
	} else
		return -1;
}

int task_info_get_taskname(const int id, const char *name) {
	if (id < 0 || id >= TASK_ID_MAX)
		return -1;

	name = TaskContexts[id]->name;
	return 0;
}


void task_info_init(void) {
	init();
}

TaskIdType GetTaskIdviaHandle(TaskHandle_t task) {
	configASSERT(task);
#if 0
	for (int index = 0; index < TASK_ID_MAX; index++) {
		if (TaskSvc.tasks[index] == NULL)
			continue;

		if (*TaskSvc.tasks[index]->task == task)
			return TaskSvc.tasks[index]->taskId;
	}
#endif
	return TASK_ID_MAX;
}

void PostBroadcastSignal(const TaskIdType taskId, const uint32_t sig) {
#if 0
	osStatus_t status;
	configASSERT((sig == TASK_NOTIFY_BROADCAST_SIG) | (sig == TASK_NOTIFY_WDOG_RELOADED_SIG));

	if (!IsAuthorizedTaskForBroadcast(taskId)) {
		log_debug("Error: %d has no permission.");
		//configASSERT(0);
		return;
	}

	for (int index = 0; index < TASK_ID_MAX; index++) {
		if (TaskSvc.tasks[index] == NULL)
			continue;

		status = osThreadFlagsSet(*TaskSvc.tasks[index]->task, sig);
		if (status != osOK) {
			// error handling
			//configASSERT(status == osOK);
		}
	}
#endif
}