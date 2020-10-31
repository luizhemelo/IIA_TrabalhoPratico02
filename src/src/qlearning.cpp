// #####################################################
// ##             TRABALHO PRATICO 2                  ##
// ##  INTRODUCAO A INTELIGENCIA ARTIFICIAL - 2020/1  ##
// ##                                                 ##
// ##         LUIZ HENRIQUE DE MELO SANTOS            ##
// ##            OTAVIO AUGUSTO SILVA                 ##
// #####################################################

#include "qlearning.hpp"


// Inicia uma nova instancia para a execucao do algoritmo Q-Learning
AGV::AGV (double _alpha, double _epsilon, double _lambda, int _N) {
    int i, j;
    this->steps = 0;
    this->alpha = _alpha;
    this->lambda = _lambda;
    this->epsilon = _epsilon;
    this->N = _N;

	// Recebimento das informacoes sobre o mapa
	std::cin >> m >> n >> W;

    // Criacao do mapa do AGV
	std::vector<char> world_row;
	for (i=0 ; i<n ; i++) {  // Colunas
		world_row.push_back('*');
	}
	for (i=0 ; i<m ; i++) {  // Linhas
		this->world.push_back(world_row);
	}

    // Recebimento dos dados contidos em cada posicao do mapa do AGV
	for (i=0 ; i<m ; i++) {
		for (j=0 ; j<n ; j++) {
			std::cin >> this->world[i][j];
		}
	}

    // Criacao e inicializacao da Q-Table com zeros
	q_values qv;
	qv.l=0; qv.r=0; qv.d=0; qv.u=0;
	std::vector<q_values> q_row;
	for (i=0 ; i<m ; i++) {  // Linha
		q_row.clear();
		for (j=0 ; j<n ; j++) {  // Coluna
			qv.pos.first = i;
			qv.pos.second = j;
			q_row.push_back(qv);
		}
		q_table.push_back(q_row);
	}

    // Inicializacao do vetor de posicoes possiveis de acao pelo AGV
	for (i=0 ; i<m ; i++) {
		for (j=0 ; j<n ; j++) {
			if (this->world[i][j]=='.' || this->world[i][j]=='#') {
				this->locs.push_back(&q_table[i][j]);
			}
		}
	}

    // Criacao do arquivo de saida do algortimo
    this->pi.open("pi.txt");
}


// Impressao das politicas escolhidas em cada movimento feito pelo AGV
void AGV::printPi (char mov) {
    this->pi << "(" << this->agent.first << ", " << this->agent.second << ", " << this->steps << "), ";
    if (mov=='R') {
        pi << "RIGHT" << ", " << std::setprecision(2) << this->q_table[this->agent.first][this->agent.second].r << std::endl;
    }
    else if (mov=='L') {
        pi << "LEFT" << ", " << std::setprecision(2) << this->q_table[this->agent.first][this->agent.second].l << std::endl;
    }
    else if (mov=='U') {
        pi << "UP" << ", " << std::setprecision(2) << this->q_table[this->agent.first][this->agent.second].u << std::endl;
    }
    else if (mov=='D') {
        pi <<  "DOWN" << ", " << std::setprecision(2) << this->q_table[this->agent.first][this->agent.second].d << std::endl;
    }
}


// Identifica o maior Q-Value presente em uma determinada posicao do mapa
double AGV::maxQ (std::pair<int,int> pos) {
    double max=0;
	if (this->q_table[pos.first][pos.second].r > max) {  // Analisa a DIREITA
		max = this->q_table[pos.first][pos.second].r;
	}
	if (this->q_table[pos.first][pos.second].l > max) {  // Analisa a ESQUERDA
		max = this->q_table[pos.first][pos.second].l;
	}
	if (this->q_table[pos.first][pos.second].u > max) {  // Analisa ACIMA
		max = this->q_table[pos.first][pos.second].u;
	}
	if (this->q_table[pos.first][pos.second].d > max) {  // Analisa ABAIXO
		max = this->q_table[pos.first][pos.second].d;
	}
	return max;  // Retorna o maior peso
}


// Atualiza os valores da Q-Table após um determinado movimento feito
void AGV::updateQTable (int action, int reward, double _Q, double Q) {
	if (action == 0) {  // Movimento para posicao a DIREITA
		this->q_table[this->agent.first][this->agent.second].r += (this->alpha*(reward + ((this->lambda*_Q) - Q)));
	}
	else if (action == 1) {  // Movimento para posicao a ESQUERDA
		this->q_table[this->agent.first][this->agent.second].l += (this->alpha*(reward + ((this->lambda*_Q) - Q)));
	}
	else if (action == 2) {  // Movimento para posicao ACIMA
		this->q_table[this->agent.first][this->agent.second].u += (this->alpha*(reward + ((this->lambda*_Q) - Q)));
	}
	else if (action == 3) {  // Movimento para posicao a ABAIXO
		this->q_table[this->agent.first][this->agent.second].d += (this->alpha*(reward + ((this->lambda*_Q) - Q)));
	}
}


// Identifica qual sera a recompensa a ser recebida pelo AGV apos a escolha de um determinado movimento
int AGV::rewardFunction (std::pair<int,int> pos) {
    if (pos.first<0 || pos.first>=this->m || pos.second<0 || pos.second>=this->n || this->world[pos.first][pos.second]=='*') {  // Parede / Invalido - TERMINAL
		return INVALID;
    }
    else if (this->world[pos.first][pos.second]=='.') {  // Ponto vazio
        return EMPTY;
    }
    else if (this->world[pos.first][pos.second]=='#') {  // Ponto de localizacao
        return LOCATION;
    }
    else {  // Ponto de coleta - TERMINAL
        return COLLECT;
    }
}


// Promove o movimento do AGV no mapa da fabrica
// RETORNO: 'true' para estado terminal / 'false' para estado nao-terminal
bool AGV::moveAgent (int action) {
    int i, j;
    int reward;
	char mov;

    if (action==0) {  // Movimento para posicao a DIREITA
		mov = 'R';
        i=this->agent.first; j=this->agent.second+1;

		reward = rewardFunction({i, j});
		if (this->steps==this->W && reward!=COLLECT) {  // Caso o AGV nao tenha mais movimentos possiveis - TERMINAL			return true;
		}
		else if (reward==INVALID) {  // Caso seja uma posicao invalida - TERMINAL
			updateQTable(action, reward, this->q_table[this->agent.first][this->agent.second].r, this->q_table[this->agent.first][this->agent.second].r);
			this->printPi(mov); this->steps += 1;			return true;
		}
		else if (reward==EMPTY) {  // Caso seja uma posicao valida - VALIDA PARA MOVIMENTO
			updateQTable(action, reward, maxQ({i,j}), this->q_table[this->agent.first][this->agent.second].r);
			this->printPi(mov); this->steps += 1;
			this->agent.second+=1;
		}
		else if (reward==LOCATION) {  // Caso seja um ponto de localizacao - VALIDA PARA MOVIMENTO
			updateQTable(action, reward, maxQ({i,j}), this->q_table[this->agent.first][this->agent.second].r);
			this->printPi(mov); this->steps = 0;
			this->agent.second+=1;
		}
		else if (reward==COLLECT) {  // Caso seja um ponto de coleta - TERMINAL
			updateQTable(action, reward, 0, this->q_table[this->agent.first][this->agent.second].r);
			this->printPi(mov); this->steps += 1;			return true;
		}
    }
    else if (action==1) {  // Movimento para posicao a ESQUERDA
		mov = 'L';        i=this->agent.first; j=this->agent.second-1;
		reward = rewardFunction({i, j});
		if (this->steps==this->W && reward!=COLLECT) {  // Caso o AGV nao tenha mais disponibilidade de movimentos - TERMINAL			return true;
		}
		else if (reward==INVALID) {  // Caso seja uma posicao invalida - TERMINAL
			updateQTable(action, reward, this->q_table[this->agent.first][this->agent.second].l, this->q_table[this->agent.first][this->agent.second].l);
			this->printPi(mov); this->steps += 1;			return true;
		}
		else if (reward==EMPTY) {  // Caso seja uma posicao valida - VALIDA PARA MOVIMENTO
			updateQTable(action, reward, maxQ({i,j}), this->q_table[this->agent.first][this->agent.second].l);
			this->printPi(mov); this->steps += 1;
			this->agent.second-=1;
		}
		else if (reward==LOCATION) {  // Caso seja um ponto de localizacao - VALIDA PARA MOVIMENTO
			updateQTable(action, reward, maxQ({i,j}), this->q_table[this->agent.first][this->agent.second].l);
			this->printPi(mov); this->steps = 0;
			this->agent.second-=1;
		}
		else if (reward==COLLECT) {  // Caso seja um ponto de coleta - TERMINAL
			updateQTable(action, reward, 0, this->q_table[this->agent.first][this->agent.second].l);
			this->printPi(mov); this->steps += 1;
			return true;
		}
    }
    else if (action==2) {  // Movimento para posicao ACIMA
		mov = 'U';
        i=this->agent.first-1; j=this->agent.second;

		reward = rewardFunction({i, j});
		if (this->steps==this->W && reward!=COLLECT) {  // Caso o AGV nao tenha mais disponibilidade de movimentos - TERMINAL
			return true;
		}
		else if (reward==INVALID) {  // Caso seja uma posicao invalida - TERMINAL
			updateQTable(action, reward, this->q_table[this->agent.first][this->agent.second].u, this->q_table[this->agent.first][this->agent.second].u);
			this->printPi(mov); this->steps += 1;
			return true;
		}
		else if (reward==EMPTY) {  // Caso seja uma posicao valida - VALIDA PARA MOVIMENTO
			updateQTable(action, reward, maxQ({i,j}), this->q_table[this->agent.first][this->agent.second].u);
			this->printPi(mov); this->steps += 1;
			this->agent.first-=1;
		}
		else if (reward==LOCATION) {  // Caso seja um ponto de localizacao - VALIDA PARA MOVIMENTO
			updateQTable(action, reward, maxQ({i,j}), this->q_table[this->agent.first][this->agent.second].u);
			this->printPi(mov); this->steps = 0;
			this->agent.first-=1;
		}
		else if (reward==COLLECT) {  // Caso seja um ponto de coleta - TERMINAL
			updateQTable(action, reward, 0, this->q_table[this->agent.first][this->agent.second].u);
			this->printPi(mov); this->steps += 1;			return true;
		}
    }
    else if (action==3) {  // Movimento para posicao ABAIXO
		mov = 'D';        i=this->agent.first+1; j=this->agent.second;
		reward = rewardFunction({i, j});
		if (this->steps==this->W && reward!=COLLECT) {  // Caso o AGV nao tenha mais disponibilidade de movimentos - TERMINAL
			return true;
		}
		else if (reward==INVALID) {  // Caso seja uma posicao invalida - TERMINAL
			updateQTable(action, reward, this->q_table[this->agent.first][this->agent.second].d, this->q_table[this->agent.first][this->agent.second].d);
			this->printPi(mov); this->steps += 1;
			return true;
		}
		else if (reward==EMPTY) {  // Caso seja uma posicao valida - VALIDA PARA MOVIMENTO
			updateQTable(action, reward, maxQ({i,j}), this->q_table[this->agent.first][this->agent.second].d);
			this->printPi(mov); this->steps += 1;
			this->agent.first+=1;
		}
		else if (reward==LOCATION) {  // Caso seja um ponto de localizacao - VALIDA PARA MOVIMENTO
			updateQTable(action, reward, maxQ({i,j}), this->q_table[this->agent.first][this->agent.second].d);
			this->printPi(mov); this->steps = 0;
			this->agent.first+=1;
		}
		else if (reward==COLLECT) {  // Caso seja um ponto de coleta - TERMINAL
			updateQTable(action, reward, 0, this->q_table[this->agent.first][this->agent.second].d);
			this->printPi(mov); this->steps += 1;
			return true;
		}
    }
    return false;
}


// Aplicacao da politica de Exploracao no AGV
bool AGV::explore () {
    if (this->moveAgent(rand()%4)==true) return true;
	return false;
}


// Aplicacao da politica de Exploit no AGV
bool AGV::exploit () {
    if ((this->q_table[this->agent.first][this->agent.second].r==0) && (this->q_table[this->agent.first][this->agent.second].l==0) &&
	    (this->q_table[this->agent.first][this->agent.second].u==0) && (this->q_table[this->agent.first][this->agent.second].d)) {
		// Caso em que todas as acoes possuam Q-Value igual a zero
		// Este condicional previne com que o algoritmo entre em loop
		return this->explore();
	}
    else {
		int action=0;
		double max=this->q_table[this->agent.first][this->agent.second].r;

		if (this->q_table[this->agent.first][this->agent.second].l > max) {
			max=this->q_table[this->agent.first][this->agent.second].l; 
			action=1;
		}
		if (this->q_table[this->agent.first][this->agent.second].u > max) {
			max=this->q_table[this->agent.first][this->agent.second].u; 
			action=2;
		}
		if (this->q_table[this->agent.first][this->agent.second].d > max) {
			max=this->q_table[this->agent.first][this->agent.second].d; 
			action=3;
		}
		if (moveAgent(action)==true) return true;
	}
	return false;
}


// Execucao do algoritmo Q-Learning para um AGV sobre um determinado mapa valido de uma fabrica
void AGV::qLearning () {
    int pos;
	bool mark;
	std::vector<std::vector<char>> world_copy=world;

    /*
    Promove a criacao de uma distribuicao uniforme entre (0,1)
	Sera utilizada para a definicao do Exploit/Explore e para a
    definicao da localizacao inicial do AGV em cada um dos episodios
    */
    srand((unsigned) time(0));
	std::random_device rd;
	std::mt19937 gen(rd());  
	std::uniform_real_distribution<> dist(0, 1);

    // Execucao do algoritmo para cada uma dos episodios
    for (int it=0 ; it<this->N ; it++) {
        // Reinicializacao das variaveis para novo episodio
		mark = false;
        this->steps = 0;
		this->world = world_copy;

        // Definicao aleatoria de uma posicao inicial para o agente
		pos = (rand() % this->locs.size());
		this->agent.first = this->locs[pos]->pos.first;
		this->agent.second = this->locs[pos]->pos.second;

        // Seleciona qual politica sera feita de acordo com o Epsilon recebido
        while(!mark) {
			if (dist(gen) < epsilon) {
				if (this->explore()==true) mark=true;
			}
			else {
				if (this->exploit()==true) mark=true;
			}
		}
    }
    pi.close();
}
