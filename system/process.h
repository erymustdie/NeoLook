#ifndef PROCESS_H
#define PROCESS_H

// Classe que armazena um processo.
class process {
   public:
    int id;              // Identificador do processo.
    int instant;         // Instante de chegada do processo.
    int demand_cpu;      // Demanda de CPU do processo.
    int demand_disk;     // Demanda de disco do processo.
    int demand_network;  // Demanda de rede do processo.

    // Construtor, inicializa os atributos.
    process(int id, int instant, int demand_cpu, int demand_disk,
            int demand_network) {
        this->id = id;
        this->instant = instant;
        this->demand_cpu = demand_cpu;
        this->demand_disk = demand_disk;
        this->demand_network = demand_network;
    }

    // Printa o processo.
    void print() {
        printf("Instant: %d\n", this->instant);
        printf("Demand CPU: %d\n", this->demand_cpu);
        printf("Demand Disk: %d\n", this->demand_disk);
        printf("Demand Network: %d\n", this->demand_network);
    }
};

/**
 * Uma breve explicação sobre os comparadores:
 *
 * Os comparadores são funções que comparam dois processos e retornam um inteiro
 * que indica a relação entre os dois processos, eles são utilizados para
 * ordenar os processos nas filas de prioridade, já que a min_heap precisa de
 * algum parâmetro para ordenar os processos que depende de onde o processo
 * está.
 *
 */

// Comparador de processos em relação a demanda de CPU.
int compare_process_cpu(process* a, process* b) {
    if (a->demand_cpu < b->demand_cpu) {
        return -1;
    } else if (a->demand_cpu > b->demand_cpu) {
        return 1;
    } else {
        return 0;
    }
}

// Comparador de processos em relação a demanda de disco.
int compare_process_disk(process* a, process* b) {
    if (a->demand_disk < b->demand_disk) {
        return -1;
    } else if (a->demand_disk > b->demand_disk) {
        return 1;
    } else {
        return 0;
    }
}

// Comparador de processos em relação a demanda de rede.
int compare_process_network(process* a, process* b) {
    if (a->demand_network < b->demand_network) {
        return -1;
    } else if (a->demand_network > b->demand_network) {
        return 1;
    } else {
        return 0;
    }
}

#endif