#ifndef COMPUTER_H
#define COMPUTER_H

#include <iostream>

#include "../include/min_heap.h"
#include "../include/queue.h"
#include "process.h"

// Classe que armazena um computador.
using namespace std;
template <typename Type>
class computer {
   public:
    Type* cpu;                // Fila de espera da CPU.
    Type* disk_1;             // Fila de espera do disco 1.
    Type* disk_2;             // Fila de espera do disco 2.
    process* running_cpu;     // Processo em execução na CPU.
    process* running_disk_1;  // Processo em execução no disco 1.
    process* running_disk_2;  // Processo em execução no disco 2.

    // Construtor, inicializa as filas e os processos em execução.
    computer() {
        cpu = new Type();
        disk_1 = new Type();
        disk_2 = new Type();
        running_cpu = nullptr;
        running_disk_1 = nullptr;
        running_disk_2 = nullptr;
    }

    // Destrutor, desaloca as filas e os processos em execução.
    ~computer() {
        delete cpu;
        delete disk_1;
        delete disk_2;
        delete running_cpu;
        delete running_disk_1;
        delete running_disk_2;
    }

    // Verifica se existe algum processo em execução ou nas filas.
    bool has_process() {
        return (!this->cpu->empty()) || (!this->disk_1->empty()) ||
               (!this->disk_2->empty()) || has_process_in_cpu() ||
               has_process_in_disk_1() || has_process_in_disk_2();
    }

    // Verifica se existe algum processo em execução na CPU.
    bool has_process_in_cpu() { 
        return running_cpu != nullptr; 
    }

    // Verifica se existe algum processo em execução no disco 1.
    bool has_process_in_disk_1() { 
        return running_disk_1 != nullptr; 
    }

    // Verifica se existe algum processo em execução no disco 2.
    bool has_process_in_disk_2() { 
        return running_disk_2 != nullptr; 
    }

    // Adiciona um processo na fila de espera da CPU.
    void add_process_cpu(process* process) { 
        cpu->push(process); 
    }

    // Adiciona um processo na fila de espera de um dos discos.
    void add_process_disk() {
        // Verifica se existe algum processo em execução na CPU.
        if (this->running_cpu != nullptr) {
            // Cria um número aleatório entre 0 e 1.
            int random = rand() % 2;

            // Com base no número aleatório, adiciona o processo na fila de espera do disco 1 ou 2.
            if (random == 0) {
                disk_1->push(this->running_cpu);
            } else {
                disk_2->push(this->running_cpu);
            }
        }

        // Limpa o processo em execução na CPU.
        this->running_cpu = nullptr;
    }

    // Consome uma unidade de tempo do processo em execução na CPU.
    void consume_cpu() {
        if (running_cpu != nullptr && running_cpu->demand_cpu > 0) {
            running_cpu->demand_cpu--;
        }
    }

};

#endif