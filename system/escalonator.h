#ifndef ESCALONATOR_H
#define ESCALONATOR_H

#include "../include/queue.h"
#include "computer.h"
#include "enumerator.h"
#include "event.h"
#include "metrics.h"
#include "resource.h"

// Classe que representa o escalonador.
template <typename Type>
class escalonator {
public:
    queue<event*> events;             // Fila de eventos
    resource<Type>* system;           // Sistema (um recurso)
    queue<process*>* process_queue;   // Fila de processos

    event** events_vec;               // Vetor de ponteiros para eventos (serão registrados 8 eventos por processo)
    int size_events;                  // Tamanho do vetor de eventos
    int qtd_process;                  // Quantidade de processos

    event*** log;                     // Matriz de ponteiros para eventos, cada linha é dedicada a um processo possuindo todos os eventos relacionados a ele.

    // Construtor (recebe a quantidade de computadores, a política de escalonamento e a fila de processos a serem processados).
    escalonator(int amount, priority_policy p, queue<process*>* process_queue) {
        this->system = new resource<Type>(amount, p);  // Inicializando o sistema
        this->process_queue = process_queue;          // Inicializando a fila de processos
        qtd_process = process_queue->size();          // Inicializando a quantidade de processos

        // Cada processo será registrado em 8 eventos:
        //   - Iniciar processo.
        //   - Esperar CPU.
        //   - Acessar CPU.
        //   - Esperar Disco.
        //   - Acessar Disco.
        //   - Esperar Rede.
        //   - Acessar Rede.
        //   - Terminar Processo

        // O vetor de eventos terá o tamanho de 8 vezes a quantidade de processos.
        events_vec = new event*[qtd_process * 8];

        // Inicializando o tamanho do vetor de eventos.
        size_events = 0;

        // Inicializando a matriz de eventos.
        log = new event**[qtd_process];

        // Inicializando cada linha da matriz de eventos.
        for (int i = 0; i < qtd_process; i++) {
            log[i] = new event*[8];
        }
    }

    // Método que executa o escalonamento.
    void run(string filename) {
        int time = 0;  // Instante de tempo

        // Loop que executa o escalonamento.
        while (true) {
            bool has_process_in_system = false;  // Flag que indica se há processo no sistema.

            // Verificando se há processo a ser processado no sistema.
            for (int i = 0; i < this->system->amount; i++) {
                // Se houver processo no sistema, a flag é setada para true e o loop é quebrado.
                if (this->system->computer_type[i].has_process()) {
                    has_process_in_system = true;
                    break;
                }
            }

            // Se não houver processo na fila de processos, no sistema e na rede, o escalonamento é finalizado.
            if (process_queue->empty() &&
                !this->system->has_process_in_network() &&
                !has_process_in_system) {
                break;
            }

            // Enquanto houver processo na fila de processos e o instante de tempo for igual ao instante de chegada do processo, o processo é adicionado ao sistema.
            while (!process_queue->empty() && process_queue->front()->instant == time) {
                process* p = process_queue->pop();  // Retirando o processo da fila de processos.

                // Registrando o evento de inicialização do processo.
                events_vec[size_events] = new event(time, INITIALIZE_PROCESS, new process(p->id, p->instant, p->demand_cpu, p->demand_disk, p->demand_network));

                // Incrementando o tamanho do vetor de eventos.
                size_events++;

                // Registrando o evento de espera da CPU.
                events_vec[size_events] = new event(time, WAIT_CPU, new process(p->id, p->instant, p->demand_cpu, p->demand_disk, p->demand_network));

                // Incrementando o tamanho do vetor de eventos.
                size_events++;

                // Adicionando o processo ao sistema.
                system->add_process(p);
            }

            // Percorrendo cada computador do sistema.
            for (int i = 0; i < system->amount; i++) {
                // Criando um ponteiro para o computador atual.
                computer<Type>* pc = &this->system->computer_type[i];

                // PROCESS_CPU
                // Verificando se não existe processo em execução na CPU.
                if (!pc->has_process_in_cpu()) {
                    // Retirando o processo da fila de espera da CPU.
                    process* p = pc->cpu->pop();

                    // Verificando se existe processo na fila de espera da CPU.
                    if (p != nullptr) {
                        // Se existir, o processo é adicionado à execução da CPU.
                        pc->running_cpu = p;

                        // Registrando o evento de acesso à CPU.
                        events_vec[size_events] = new event(time, ACCESS_CPU, new process(p->id, p->instant, p->demand_cpu, p->demand_disk, p->demand_network));

                        // Incrementando o tamanho do vetor de eventos.
                        size_events++;
                    }
                } else {
                    // Se existir processo em execução na CPU, a demanda de CPU do processo é decrementada.
                    pc->consume_cpu();

                    // Caso a demanda de CPU do processo seja igual a 0, o processo é retirado da execução da CPU.
                    if (pc->running_cpu->demand_cpu == 0) {
                        // Salvando o processo em execução na CPU.
                        process* p = pc->running_cpu;

                        // Registrando o evento de espera do disco.
                        events_vec[size_events] = new event(time, WAIT_DISK, new process(p->id, p->instant, p->demand_cpu, p->demand_disk, p->demand_network));

                        // Incrementando o tamanho do vetor de eventos.
                        size_events++;

                        // Adiciona o processo na fila de espera de um dos discos.
                        pc->add_process_disk();

                        // Retirando o processo da execução da CPU.
                        pc->running_cpu = nullptr;

                        p = pc->cpu->pop();

                        if (p != nullptr) {
                            // Se existir, o processo é adicionado à execução da CPU.
                            pc->running_cpu = p;

                            // Registrando o evento de acesso à CPU.
                            events_vec[size_events] = new event(time, ACCESS_CPU, new process(p->id, p->instant, p->demand_cpu, p->demand_disk, p->demand_network));

                            // Incrementando o tamanho do vetor de eventos.
                            size_events++;
                        }
                    }
                }

                // PROCESS_DISK_1
                // Verificando se não existe processo em execução no disco 1.
                if (!pc->has_process_in_disk_1()) {
                    // Retirando o processo da fila de espera do disco 1.
                    process* p = pc->disk_1->pop();

                    // Verificando se existe processo na fila de espera do disco 1.
                    if (p != nullptr) {
                        // Adicionando o processo à execução do disco 1.
                        pc->running_disk_1 = p;

                        // Registrando o evento de acesso ao disco.
                        events_vec[size_events] = new event(time, ACCESS_DISK, new process(p->id, p->instant, p->demand_cpu, p->demand_disk, p->demand_network));

                        // Incrementando o tamanho do vetor de eventos.
                        size_events++;
                    }
                } else {
                    // Se existir processo em execução no disco 1, a demanda de disco do processo é decrementada.
                    pc->running_disk_1->demand_disk--;
                }

                // PROCESS_DISK_2
                // Verificando se não existe processo em execução no disco 2.
                if (!pc->has_process_in_disk_2()) {
                    // Retirando o processo da fila de espera do disco 2.
                    process* p = pc->disk_2->pop();

                    // Verificando se existe processo na fila de espera do disco 2.
                    if (p != nullptr) {
                        // Adicionando o processo à execução do disco 2.
                        pc->running_disk_2 = p;

                        // Registrando o evento de acesso ao disco.
                        events_vec[size_events] = new event(time, ACCESS_DISK, new process(p->id, p->instant, p->demand_cpu, p->demand_disk, p->demand_network));

                        // Incrementando o tamanho do vetor de eventos.
                        size_events++;
                    }
                } else {
                    // Se existir processo em execução no disco 2, a demanda de disco do processo é decrementada.
                    pc->running_disk_2->demand_disk--;
                }

                // CONSUME_DISK
                process* disk1_p = pc->running_disk_1;  // Processo em execução no disco 1.

                // Verificando se o processo em execução no disco 1 terminou.
                if (pc->has_process_in_disk_1() && disk1_p->demand_disk == 0) {
                    // Salvando o processo em execução no disco 1.
                    process* p = disk1_p;
                    // Retirando o processo da execução do disco 1.
                    pc->running_disk_1 = nullptr;

                    // Registrando o evento de espera da rede.
                    events_vec[size_events] = new event(time, WAIT_NETWORK, new process(p->id, p->instant, p->demand_cpu, p->demand_disk, p->demand_network));

                    // Incrementando o tamanho do vetor de eventos.
                    size_events++;

                    // Adicionando o processo à fila de espera da rede.
                    this->system->network->push(p);
                }

                process* disk2_p = pc->running_disk_2;  // Processo em execução no disco 2.

                // Verificando se o processo em execução no disco 2 terminou.
                if (pc->has_process_in_disk_2() && disk2_p->demand_disk == 0) {
                    // Salvando o processo em execução no disco 2.
                    process* p = disk2_p;
                    // Retirando o processo da execução do disco 2.
                    pc->running_disk_2 = nullptr;

                    // Registrando o evento de espera da rede.
                    events_vec[size_events] = new event(time, WAIT_NETWORK, new process(p->id, p->instant, p->demand_cpu, p->demand_disk, p->demand_network));

                    // Incrementando o tamanho do vetor de eventos.
                    size_events++;

                    // Adicionando o processo à fila de espera da rede.
                    this->system->network->push(p);
                }

                // PROCESS_NETWORK
                // Verificando se não existe processo em execução na rede.
                if (this->system->running_network == nullptr) {
                    // Retirando o processo da fila de espera da rede.
                    process* p = this->system->network->pop();

                    // Verificando se existe processo na fila de espera da rede.
                    if (p != nullptr) {
                        // Adicionando o processo à execução da rede.
                        this->system->running_network = p;

                        // Registrando o evento de acesso à rede.
                        events_vec[size_events] = new event(time, ACCESS_NETWORK, new process(p->id, p->instant, p->demand_cpu, p->demand_disk, p->demand_network));

                        // Incrementando o tamanho do vetor de eventos.
                        size_events++;
                    }
                } else {
                    // Se existir processo em execução na rede, a demanda de rede do processo é decrementada.
                    this->system->running_network->demand_network--;
                }

                // CONSUME_NETWORK
                // Verificando se o processo em execução na rede terminou.
                if (this->system->running_network != nullptr &&
                    this->system->running_network->demand_network == 0) {

                    // Salvando o processo em execução na rede.
                    process* p = this->system->running_network;
                    // Retirando o processo da execução da rede.
                    this->system->running_network = nullptr;

                    // Registrando o evento de término do processo.
                    events_vec[size_events] = new event(time, FINISH_PROCESS, new process(p->id, p->instant, p->demand_cpu, p->demand_disk, p->demand_network));

                    // Incrementando o tamanho do vetor de eventos.
                    size_events++;

                    // Deletando o processo. (Lembrando que os eventos guardam uma cópia do processo, 
                    // então não há problema em deletar o processo aqui)
                    delete p;
                }
            }

            // Incrementando o instante de tempo.
            time++;
        }

        // Cria uma fila de eventos para cada processo (8 eventos por processo)
        for (int i = 0; i < (qtd_process * 8); i++) {
            events.push(events_vec[i]);
        }

        // Contador de eventos registrados para cada processo
        int* counter_event = new int[qtd_process];

        // Inicializando contadores
        for (int i = 0; i < qtd_process; i++) {
            counter_event[i] = 0;
        }

        // Cada evento é colocado em sua devida linha de processo de acordo com
        // o ID do processo, os eventos de cada processo serão colocados em ordem
        // cronológica.
        for (int i = 0; i < (qtd_process * 8); i++) {
            // Retirando o evento da fila de eventos.
            event* e = events.pop();

            // e->print();  // Usado para debug.
            // std::cout << std::endl;  // Usado para debug.

            // Registrando o evento na matriz de eventos.
            log[e->process_a->id][counter_event[e->process_a->id]] = e;
            // Incrementando o contador de eventos do processo.
            counter_event[e->process_a->id]++;
        }

        // Deletando o vetor do contador de eventos.
        delete[] counter_event;

        // Calculando as métricas e salvando em um objeto.
        metrics* m = new metrics(time - 1, qtd_process, log);

        // Imprimindo as métricas.
        m->print();

        // Salvando as métricas em um arquivo.
        m->save(filename, system->amount, system->policy == SJF ? "SJF" : "FCFS");
    }

    // Destrutor, desaloca os eventos, o sistema e a matriz de eventos.
    ~escalonator() {
        // Deletando a fila de eventos.
        while (!events.empty()) {
            event* e = events.pop();
            delete e;
        }

        // Deletando o sistema.
        delete system;

        // Deletando cada evento do vetor de eventos.
        for (int i = 0; i < size_events; i++) {
            delete events_vec[i];
        }

        // Deletando o vetor de eventos.
        delete[] events_vec;

        // Deletando cada linha da matriz de eventos.
        for (int i = 0; i < qtd_process; i++) {
            delete[] log[i];
        }

        // Deletando a matriz de eventos.
        delete[] log;
    }
};

#endif

