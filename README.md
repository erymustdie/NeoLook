# NeoLook 
 
Trabalho da cadeira Estrutura de Dados, do curso de Ciência da Computação da UFC - Campus Quixada, lecionada pelo professor Atílio Gomes Luiz, no semestre 2023.2, Realizado pelos alunos Kauan Pablo e Erica de Castro.



## Descrição do Projeto
O trabalho consiste em criar um sistema chamado NeoLook que tem como objetivo simular um sistema executando uma série de processos de acordo com a política de escalonamento FCFS ou SJF, para mais informações do projeto, leia o relatório do projeto na pasta docs.

## Como executar o projeto
Acesse pasta src e execute o comando g++ main.cpp -o main para compilar o projeto.

Em seguida execute o comando ./main <política (SJF ou FCFS)> <arquivo_de_entrada> <número_de_computadores> para executar o projeto.

Onde o arquivo_de_entrada deve está na pasta src/in. 
Ao final da execução será gerado um arquivo de saída na pasta src/out com as métricas do sistema.

## Exemplo de execução
cd src (acessar a pasta src)

g++ main.cpp -o main (compilar o projeto)

Windows:
main SJF teste01.txt 2 (executar o projeto)

Linux:
./main SJF teste01.txt 2 (executar o projeto)
