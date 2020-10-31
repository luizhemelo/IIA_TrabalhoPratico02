// #####################################################
// ##             TRABALHO PRATICO 2                  ##
// ##  INTRODUCAO A INTELIGENCIA ARTIFICIAL - 2020/1  ##
// ##                                                 ##
// ##         LUIZ HENRIQUE DE MELO SANTOS            ##
// ##            OTAVIO AUGUSTO SILVA                 ##
// #####################################################

#ifndef QLEARNING_H
#define QLEARNING_H

#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstdlib>
#include <fstream>
#include <random>
#include <vector>
#include <ctime>


#define EMPTY -1     // Recompensa para espacos vazios ou sem perigos
#define INVALID -10  // Recompensa para paredes ou localizacoes fora do mapa - TERMINAL
#define LOCATION 1   // Recompensa para pontos de localizacao
#define COLLECT 10   // Recompensa para pontos de coleta - TERMINAL


// Armazenara os Q-Values para os movimentos possiveis em cada posicao da Q-Table
struct q_values {
    double r, l, u, d;  // Right / Left / Up / Down
    std::pair<int,int> pos;  // Armazena a posicao destes Q-Values no mapa do AGV
};


class AGV {
    private:  // Variaveis utilizadas pelo algoritmo
        int W;  // Numero maximo de passos que podem ser feitos pelo AGV
        int m, n;  // Numero de linhas / Numero de colunas
        int steps;  // Numero de passos dados pelo AGV em um determinado episodio

        double alpha;  // Taxa de aprendizado
        double epsilon;  // Taxa de exploracao
        double lambda;  // Fator de desconto
        int N;  // Numero de iteracoes

        std::pair<int,int> agent;  // Posicao do AGV no mundo
        std::vector<std::vector<char>> world;  // Representacao do mundo que o AGV deve percorrer
        std::vector<std::vector<q_values>> q_table;  // Tabela que ira armazenar os Q-Values para cada posicao
        std::vector<q_values*> locs; // Armazena as localizacoes do mundo VIAVEIS para atuacao do agente
        
        std::ofstream pi;  // Arquivo de saída com formatacao definida em especificacao do Trabalho

    public:  // Funcoes utilizadas pelo algoritmo
        AGV (double _alpha, double _epsilon, double _lambda, int _N);

        void printPi (char mov);

        double maxQ (std::pair<int,int> pos);
        void updateQTable (int action, int reward, double _Q, double Q);
        int rewardFunction (std::pair<int,int> pos);

        bool moveAgent (int action);
        bool explore ();
        bool exploit ();

        void qLearning ();
};


#endif
