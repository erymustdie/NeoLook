#ifndef RESOURCE_H
#define RESOURCE_H

#include "../include/min_heap.h"
#include "../include/queue.h"
#include "computer.h"
#include "enumerator.h"
#include "process.h"

template <typename type>
class resource {
 public:
  computer<type> *computer_type;  // Vetor de computadores.
  int amount;                     // Quantidade de computadores.
  priority_policy policy;         // Política de escalonamento.
  type *network;                  // Fila de espera da rede.
  process *running_network;       // Processo que está sendo executado na rede.

  // Construtor, inicializa os computadores e a rede.
  resource(int amount, priority_policy policy) {
    this->amount = amount;
    this->policy = policy;
    this->computer_type = new computer<type>[amount];
    this->network = new type();
    this->running_network = nullptr;

    // Caso a política seja SJF, seta os comparadores da min_heap nas respectivas filas.
    if (policy == SJF) {
      // Para cada computador, seta o comparador da CPU e dos discos.
      for (int i = 0; i < amount; i++) {
        this->computer_type[i].cpu->set_comparator(&compare_process_cpu);
        this->computer_type[i].disk_1->set_comparator(&compare_process_disk);
        this->computer_type[i].disk_2->set_comparator(&compare_process_disk);
      }

      // Setando o comparador da rede.
      this->network->set_comparator(&compare_process_network);
    }
  }

  // Adiciona um processo em uma das CPUs.
  void add_process(process *process) {
    // Cria um index aleatório.
    int random_index = rand() % amount;
    // Adiciona o processo na CPU do index aleatório.
    computer_type[random_index].add_process_cpu(process);
  }

  // AVerifica se existe processo na rede.
  bool has_process_in_network() {
    return !network->empty() || (running_network != nullptr);
  }

  ~resource() {
    delete[] computer_type;
    delete network;
    delete running_network;
  }
};

#endif