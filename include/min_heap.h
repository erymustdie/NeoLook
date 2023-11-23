#ifndef MIN_HEAP_H
#define MIN_HEAP_H
#include <iostream>
#include <stdexcept>

template <typename type>
class min_heap {
   private:
    int (*comparator)(type, type);  // Ponteiro para a função de comparação.
    int m_size;                     // Quantidade de elementos da heap.
    int m_capacity;                 // Capacidade da heap.
    type* ptr;                      // Vetor de ponteiros para os elementos da heap.

    // Move um elemento para cima na heap, garantindo que o pai seja menor que
    // os filhos. O(log n)
    void up(int index) {
        // Verifica se o elemento é a raiz, pois se for, não tem pai.
        if (index == 0) {
            return;
        }

        // Calcula o índice do pai.
        int daddy = (index - 1) / 2;

        // Verifica se o pai é maior que seu filho.
        // Se for, troca o pai com o filho e chama a função up para o pai, pois
        // o pai pode ser maior que o seu pai.

        if (comparator(ptr[daddy], ptr[index]) == 1) {
            swap(ptr[daddy], ptr[index]);
            up(daddy);
        }
    }

    // Move um elemento para baixo na heap, garantindo que o filho seja maior
    // que os pais. O(log n)
    void down(int index) {
        // Calcula o índice do filho esquerdo, e assume que o filho esquerdo é o
        // menor filho.
        int index_smaller = (index * 2) + 1;

        // Verifica se existe filho esquerdo.
        if (index_smaller >= this->m_size) {
            return;
        }

        // Verifica se existe o filho direito.
        // Se existir, verifica se o filho direito é menor que o filho esquerdo
        // e atualiza o índice do menor filho.
        if ((index_smaller + 1) < this->m_size) {
            if (comparator(ptr[index_smaller], ptr[index_smaller + 1]) == 1) {
                index_smaller += 1;
            }
        }

        // Verifica se o elemento i (pai) é maior que o menor filho.
        // Se for, troca o elemento i com o menor filho e chama a função down
        // para o menor filho, pois o menor filho pode ser maior que os seus
        // filhos.
        if (comparator(ptr[index], ptr[index_smaller]) == 1) {
            swap(ptr[index], ptr[index_smaller]);
            down(index_smaller);
        }
    }

    // Função que dobra a capacidade da heap. O(n)
    void reserve() {
        // Cria um novo vetor com o dobro da capacidade e copia os elementos do
        // vetor antigo para o novo vetor.
        type* aux = new type[m_capacity * 2];
        for (int i = 0; i < m_size; i++) {
            aux[i] = ptr[i];
        }

        // Deleta o vetor antigo e atualiza o ponteiro.
        delete[] ptr;
        ptr = aux;

        // Dobra a capacidade.
        m_capacity *= 2;
    }

    // Função que troca dois elementos de posição. O(1)
    void swap(type& a, type& b) {
        type aux = a;
        a = b;
        b = aux;
    }

   public:
    // Construtor da heap, inicializa o ponteiro para função comparator de um
    // determinado tipo, o vetor de tipos, o tamanho e a capacidade da heap.
    // O(1).
    min_heap() {
        this->m_size = 0;
        this->m_capacity = 1;
        this->ptr = new type[m_capacity];
    }


    // Destrutor da heap, desaloca a memória do vetor. O(1)
    ~min_heap() { 
        delete[] ptr; 
    }

    // Função que define a função de comparação. O(1)
    void set_comparator(int (*comparator)(type, type)) {
        this->comparator = comparator;
    }

    // Retorna a quantidade de elementos na heap. O(1)
    int size() { 
        return this->m_size; 
    }

    // Retorna a capacidade da heap. O(1)
    int capacity() { 
        return this->m_capacity; 
    }

    // Retorna se a heap está vazia. O(1)
    bool empty() { 
        return this->m_size == 0; 
    }

    // Insere um elemento na heap. O(n) no pior caso(capacidade cheia) e
    // O(log n) no resto dos casos.
    void push(type value) {
        // Insere elemento no final do heap e incrementa o tamanho.
        ptr[m_size] = value;
        m_size++;

        // Dobra a capacidade do vetor se necessário. O(n)
        if (m_size == m_capacity) {
            reserve();
        }

        // Chama a função up para colocar o elemento na posição correta.
        // O(log n)
        up(m_size - 1);
    }

    // Remove o elemento da raiz da heap. O(log n)
    type pop() {
        // Verifica se a heap está vazia, e retorna nullptr se estiver.
        if (m_size == 0) {
            return nullptr;
        }

        // Troca o elemento da raiz com o último elemento da heap e
        // decrementa o tamanho.
        swap(ptr[0], ptr[m_size - 1]);
        m_size--;

        // Chama a função down para colocar o novo elemento da raiz na
        // posição correta. O(log n)
        down(0);

        return ptr[m_size];
    }

    // Função que imprime os elementos da heap. O(n) (não é usado no simulador)
    void print() {
        for (int i = 0; i < m_size; i++) {
            std::cout << ptr[i] << " ";
        }
        std::cout << std::endl;
    }

    // Função que imprime a heap em forma de árvore. O(n) (Não é usado no simulador)
    void printTree(int index, int level = 0) {
        // Verifica se o índice é maior ou igual ao tamanho da heap, pois se
        // for, não tem filho.
        if (index >= this->m_size) {
            return;
        }

        // Chama a função recursivamente para o filho direito.
        printTree((index * 2) + 2, level + 1);

        // Imprime a tabulação.
        for (int i = 0; i < level; i++) {
            std::cout << "\t";
        }

        // Imprime o elemento.
        std::cout << ptr[index] << std::endl;

        // Chama a função recursivamente para o filho esquerdo.
        printTree((index * 2) + 1, level + 1);
    }
};
#endif