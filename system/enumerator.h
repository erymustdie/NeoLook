#ifndef ENUMERATOR_H
#define ENUMERATOR_H

// Enumerador que armazena as políticas de escalonamento.
typedef enum priority_policy { FCFS, SJF } priority_policy;

// Enumerador que armazena os tipos de eventos.
typedef enum type_event {
    INITIALIZE_PROCESS,
    WAIT_CPU,
    ACCESS_CPU,
    WAIT_DISK,
    ACCESS_DISK,
    WAIT_NETWORK,
    ACCESS_NETWORK,
    FINISH_PROCESS
} type_event;

#endif
