// #####################################################
// ##             TRABALHO PRATICO 2                  ##
// ##  INTRODUCAO A INTELIGENCIA ARTIFICIAL - 2020/1  ##
// ##                                                 ##
// ##         LUIZ HENRIQUE DE MELO SANTOS            ##
// ##            OTAVIO AUGUSTO SILVA                 ##
// #####################################################

#include "qlearning.hpp"

int main (int argc, char* argv[]) {
    // Checa se entrada esta correta
	if (argc != 5) {
		std::cout << "Error: invalid parameters" << std::endl;
		return 0;
	}

    // Cria uma instancia para o algoritmo
    AGV agv = AGV(atof(argv[1]), atof(argv[2]), atof(argv[3]), atoi(argv[4]));

    // Executa o algoritmo
    agv.qLearning();

	return 0;
}
