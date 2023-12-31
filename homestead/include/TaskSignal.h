#ifndef _HOMESTEAD_TASK_SIGNAL_H_
#define _HOMESTEAD_TASK_SIGNAL_H_

typedef enum {
	SIG_ID_EMERGENCY,
    SIG_ID_MAINTENANCE,
	SIG_ID_DOG_RELOADED,
	SIG_ID_MESSAGE_Q,
	SIG_ID_LOG_SPOOL,
    SIG_ID_BROADCAST,
	SIG_ID_MAX,
} SignalIdType;

/* Definition of Signal */
#define TASK_EMERGENCY_SIG        (1 << SIG_ID_EMERGENCY)
#define TASK_MAINTENANCE_SIG      (1 << SIG_ID_MAINTENANCE)
#define TASK_WDOG_RELOADED_SIG    (1 << SIG_ID_DOG_RELOADED)
#define TASK_MESSAGE_Q_SIG        (1 << SIG_ID_MESSAGE_Q)
#define TASK_LOG_SPOOL_SIG        (1 << SIG_ID_LOG_SPOOL)
#define TASK_BROADCAST_SIG        (1 << SIG_ID_BROADCAST)


#endif  /* _HOMESTEAD_TASK_SIGNAL_H_ */