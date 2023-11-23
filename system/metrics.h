#ifndef METRICS_H
#define METRICS_H

#include <iomanip>
#include <iostream>
#include <fstream>
#include <string>

#include "event.h"

// Classe que armazena as métricas de execução do simulador.
class metrics {
   private:
    int total_time;          // Tempo total de execução.
    double average_time;     // Tempo médio de execução.
    double average_wait;     // Tempo médio de espera.
    double processing_rate;  // Taxa de processamento.

   public:
    // Construtor, calcula e inicializa as métricas.
    metrics(int total_time, int num_process, event*** log) {
        // Tempo medio de resposta = (tempo de espera + tempo de execucao) / numero de processos
        double average_time = 0;

        // Tempo medio de espera = tempo de espera / numero de processos
        double average_wait = 0;

        // Taxa de processamento = numero de processos / termino do ultimo processo - inicio do primeiro processo
        double processing_rate = static_cast<double>(num_process) / (total_time - log[0][0]->instant);

        // Percorrendo cada processo.
        for (int i = 0; i < num_process; i++) {
            double demand_cpu = log[i][0]->process_a->demand_cpu;            // Demanda da CPU de quando o processo foi inicializado.
            double demand_disk = log[i][0]->process_a->demand_disk;          // Demanda do disco de quando o processo foi inicializado.
            double demand_network = log[i][0]->process_a->demand_network;    // Demanda da rede de quando o processo foi inicializado.

            double wait_cpu = log[i][2]->instant - log[i][1]->instant;      // Tempo de espera na CPU. (instante de acesso a CPU - instante de espera da CPU)
            double wait_disk = log[i][4]->instant - log[i][3]->instant;     // Tempo de espera no disco. (instante de acesso ao disco - instante de espera do disco)
            double wait_network = log[i][6]->instant - log[i][5]->instant;  // Tempo de espera na rede. (instante de acesso a rede - instante de espera da rede)

            // Tempo de execucao = tempo de espera geral + demanda geral
            average_time += (wait_cpu + wait_disk + wait_network + demand_cpu + demand_disk + demand_network);

            // Tempo de espera = tempo de espera na CPU + tempo de espera no disco + tempo de espera na rede
            average_wait += (wait_cpu + wait_disk + wait_network);
        }
        // Tempo medio de execucao = tempo medio de execucao / numero de processos
        average_time /= num_process;
        // Tempo medio de espera = tempo medio de espera / numero de processos
        average_wait /= num_process;

        // Setando as métricas.
        this->total_time = total_time;
        this->average_time = average_time;
        this->average_wait = average_wait;
        this->processing_rate = processing_rate;
    }

    // Printa as métricas.
    void print() {
        std::cout << "Tempo total de execução: " << total_time << std::endl;
        std::cout << "Tempo médio de execução: " << std::fixed << std::setprecision(3) << average_time << std::endl;
        std::cout << "Tempo médio de espera: " << std::fixed << std::setprecision(3) << average_wait << std::endl;
        std::cout << "Taxa de processamento: " << std::fixed << std::setprecision(8) << processing_rate << std::endl;
    }

    // Salvando as métricas em um arquivo.
    void save(string filename, int num_computers, string policy) {
    std::ofstream file;
    string directory = "out/" + policy + "_" + filename;
    file.open(directory);
    file << "Política de escalonamento: " << policy << std::endl;
    file << "Número de computadores: " << num_computers << std::endl;
    file << "Tempo total de execução: " << total_time << std::endl;
    file << "Tempo médio de execução: " << std::fixed << std::setprecision(3) << average_time << std::endl;
    file << "Tempo médio de espera: " << std::fixed << std::setprecision(3) << average_wait << std::endl;
    file << "Taxa de processamento: " << std::fixed << std::setprecision(8) << processing_rate << std::endl;
    file.close();
    }

};

#endif