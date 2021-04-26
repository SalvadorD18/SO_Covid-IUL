/******************************************************************************
 ** ISCTE-IUL: Trabalho prático de Sistemas Operativos
 **
 ** Aluno: Nº: 98475  Nome: Salvador de Oliveira Carvalho Nunes Domingues  
 ** Nome do Módulo: servidor.c
 ** Descrição/Explicação do Módulo: 
 ** Irá assim neste módulo ser gerido o processo de vacinação, tendo sempre em
 ** conta os enfermeiros que estão registados no sistema Covid-IUL, a sua disponibilidade, 
 ** o centro de saúde em que operam, e o número de vagas do sistema.
 ******************************************************************************/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include "common.h"

Vaga vagas[NUM_VAGAS];
Enfermeiro* enf;
Cidadao c;
int n_enf = 0;
int flag = 0;
int pid_f = 0;
int percorrer_enfermeiros;
int percorrer_vagas;

void registo_servidor(){ 
    FILE* s = fopen(FILE_PID_SERVIDOR, "w");
    if (access(FILE_PID_SERVIDOR, F_OK) == -1){
        erro("S1) Não consegui registar o servidor!");
    } else {
    int p = getpid();
    fprintf(s, "%d\n", p);
    sucesso("S1) Escrevi no ficheiro FILE_PID_SERVIDOR o PID: %d", p);
    fclose (s);
    }
}

void armar_sinais(int s){ // função para gerir os sinais, com a atribuição de valores inteiros à variável global 'flag'
    if (s == SIGUSR1){
        flag = 2;
    } else if (s == SIGTERM) {
        flag = 3;
        for( int j = 0;  j < NUM_VAGAS ; j++){
            if(vagas[j].PID_filho == getpid())
            kill(vagas[j].cidadao.PID_cidadao, SIGTERM);
        }
        sucesso(" S5.6.1) SIGTERM recebido, servidor dedicado termina Cidadão");
        exit(EXIT_SUCCESS);
    } else if (s == SIGCHLD) {
        flag = 4;
        pid_f = wait(NULL);
    } else if (s == SIGINT){
        flag = 1;
        for( int j = 0;  j < NUM_VAGAS ; j++){
            if (vagas[j].index_enfermeiro != -1)
                kill(vagas[j].PID_filho, SIGTERM);
        }
        remove(FILE_PID_SERVIDOR);
        sucesso("S6) Servidor terminado");
        exit(EXIT_SUCCESS);
    }
}

void enfermeiros(){ // S2 - criação de uma estrutura de dados dinâmica em memória, relativamente aos enfermeiros presentes no ficheiro FILE_ENFERMEIROS
    FILE* le;
    le = fopen(FILE_ENFERMEIROS, "rb");
    if (access(FILE_ENFERMEIROS, F_OK) == -1){   
        erro("S2) Não consegui ler o ficheiro FILE_ENFERMEIROS!"); 
        exit(-1); 
        }
    fseek(le, 0, SEEK_END);
    long size;
    size = ftell(le);
    n_enf = size/sizeof(Enfermeiro);
    fseek(le, 0, SEEK_SET);
    debug("Li %d", size);
    enf = (Enfermeiro*) malloc (n_enf*sizeof(Enfermeiro)); // criação da estrutura dinâmica
    if (enf == NULL){
        perror("Não consegui reservar memória!\n");
        exit(0);
    } else {
        fread(enf, n_enf*sizeof(Enfermeiro), 1, le);
        sucesso("S2) Ficheiro FILE_ENFERMEIROS tem %d bytes, ou seja, %d enfermeiros", size, n_enf);
    }
    fclose (le);
}

void check_arrays(){ // função em modo debug, para avaliar as atualizações feitas ao inserir novos processos de vacinação

    if( DEBUG_MODE == 1){
        debug("\nID - Cedula - Nome - CS - Vacinas Dadas - Disponibilidade" );
        for( int i = 0;  i < n_enf; i++){
            printf("Enfermeiro: [%d] [%d] [%s] [%s] [%d] [%d] \n", i , enf[i].ced_profissional, enf[i].nome, enf[i].CS_enfermeiro, enf[i].num_vac_dadas, enf[i].disponibilidade); 
        }
        debug("\nID - PID Filho - Index Enfermeiro - PID Cidadão" );
        for( int j = 0;  j < NUM_VAGAS ; j++){
            printf("Vagas: [%d] [%d] [%d] [%d] \n", j, vagas[j].PID_filho, vagas[j].index_enfermeiro, vagas[j].cidadao.PID_cidadao); 
        }
    }
}

void iniciar_lista_de_vagas(){ // S3 - faz uma "limpeza" à lista de vagas, relativamente ao index dos enfermeiros
    for( int j = 0;  j < NUM_VAGAS ; j++){
    vagas[j].index_enfermeiro = -1; // coloca os index de cada enfermeiro a -1
    }
    sucesso("S3) Iniciei a lista de %d vagas", NUM_VAGAS);
}

int receber_pedidos(){ // S5.1 - aqui irão ser processados os dados enviados pelo processo cidadao
    const char u[400];
    FILE* v;
    v = fopen(FILE_PEDIDO_VACINA, "r+");
    if (access(FILE_PEDIDO_VACINA, F_OK) == -1){
        erro("S5.1) Não foi possível abrir o ficheiro FILE_PEDIDO_VACINA");
        return -1;
    } else if (errno == EACCES) { // verifica as permissões de leitura do ficheiro FILE_PEDIDO_VACINA
        erro("S5.1) Não foi possível ler o ficheiro FILE_PEDIDO_VACINA");
        return -1;
    } else if (v) {
        fscanf(v, "%[^\n]", u);
        sscanf(u, "%d:%[^:]:%d:%[^:]:%[^:]:%d:%d", &c.num_utente, c.nome, &c.idade, c.localidade, c.nr_telemovel, &c.estado_vacinacao, &c.PID_cidadao);
        printf("Chegou o cidadão com o pedido nº %d, com nº utente %d, para ser vacinado no Centro de Saúde %s\n", c.PID_cidadao, c.num_utente, c.localidade);
        sucesso("Dados Cidadão: %d; %s; %d; %s; %s; 0", c.num_utente, c.nome, c.idade, c.localidade, c.nr_telemovel);
        fclose (v);
    }
}

int verificar_condicoes_vacinacao(){ // S5.2, S5.3, S5.4, S5.5 e S5.6
    char centro_saude[] = "CS";
    strcat(centro_saude, c.localidade);
    for( int i = 0;  i < n_enf; i++){
        if(strcmp(centro_saude, enf[i].CS_enfermeiro) == 0){ // procura o enfermeiro disponível para a localidade inserida pelo cidadão
            if(enf[i].disponibilidade != 1){ // caso o enfermeiro esteja indisponível
                erro("S5.2.1) Enfermeiro %d indisponível para o pedido %s para o Centro de Saúde %s", i, &c.PID_cidadao, enf[i].CS_enfermeiro);
                kill(c.PID_cidadao, SIGTERM);
                return -1;
            } else { // caso o enfermeiro esteja disponível
                sucesso("S5.2.1) Enfermeiro %d disponível para o pedido %d", i, c.PID_cidadao);
                for( int j = 0;  j < NUM_VAGAS ; j++){
                    debug("Olha, aqui estou eu!\n");
                    if (vagas[j].index_enfermeiro == -1) { // caso existam vagas para vacinação 
                        sucesso("S5.2.2) Há vaga para vacinação para o pedido %d", c.PID_cidadao);
                        vagas[j].index_enfermeiro = i; 
                        vagas[j].cidadao = c;
                        enf[i].disponibilidade = 0;
                        sucesso("S5.3) Vaga nº %d preenchida para o pedido %d", j + 1, c.PID_cidadao);
                        int pidFilho = fork();
                        if (-1 == pidFilho) {
                        erro("S5.4) Não foi possível criar o servidor dedicado");
                        } else if (0 == pidFilho) { // código do filho
                            signal(SIGUSR2, armar_sinais);
                            signal(SIGTERM, armar_sinais);
                            vagas[j].PID_filho = getpid();
                            sucesso("S5.4) Servidor dedicado %d criado para o pedido %d", getpid(), c.PID_cidadao);
                            kill(c.PID_cidadao, SIGUSR1);
                            sucesso("S5.6.2) Servidor dedicado inicia consulta de vacinação");
                            sleep(TEMPO_CONSULTA);  
                            sucesso("S5.6.3) Vacinação terminada para o cidadão com o pedido nº %d", c.PID_cidadao);
                            kill(c.PID_cidadao, SIGUSR2);
                            sucesso("S5.6.4) Servidor dedicado termina consulta de vacinação");
                            exit(1);
                        } else { // código do pai
                            vagas[j].PID_filho = pidFilho;
                            sucesso("S5.5.1) Servidor dedicado %d na vaga %d", pidFilho, j + 1);
                            percorrer_enfermeiros = i;
                            percorrer_vagas = j;
                            sucesso("S5.5.2) Servidor aguarda fim do servidor dedicado %d", pidFilho);
                        }
                        return i;
                    }
                }
            }
        }
    }
    erro("S5.2.2) Não há vaga para vacinação para o pedido %d", c.PID_cidadao); // não existem vagas para vacinação
    kill(c.PID_cidadao, SIGTERM); // logo, envia um sinal ao processo cidadao para que este termine o seu processo
}

int atualizacao_vacinacoes(){ // S5.5.3 - atualiza a disponibilidade e o nº de vacinações dadas do enfermeiro
    int index_e = 0;
    index_e = vagas[percorrer_vagas].index_enfermeiro;
    vagas[percorrer_vagas].index_enfermeiro = -1;
    sucesso("S5.5.3.1) Vaga %d que era do servidor dedicado %d libertada", percorrer_vagas + 1, pid_f);
    enf[index_e].disponibilidade = 1;
    sucesso("S5.5.3.2) Enfermeiro %d atualizado para disponível", index_e);
    enf[index_e].num_vac_dadas++;
    int nvc = enf[index_e].num_vac_dadas;
    sucesso("S5.5.3.3) Enfermeiro %d atualizado para %d vacinas dadas", index_e, nvc);
    FILE * fb = fopen (FILE_ENFERMEIROS, "w");
    if (fb){
        fwrite (enf, sizeof(Enfermeiro)*n_enf, 1, fb); // atualiza o ficheiro FILE_ENFERMEIROS com os novos dados do enfermeiro
        fclose(fb);
        sucesso("S5.5.3.4) Ficheiro FILE_ENFERMEIROS %d atualizado para %d vacinas dadas", index_e, enf[index_e].num_vac_dadas);
    }else{
        perror("Não foi possível abrir o ficheiro FILE_ENFERMEIROS. Actualizações não concretizadas.\n");
    }
}    

int main(){
    registo_servidor();
    enfermeiros();
    iniciar_lista_de_vagas();
    signal(SIGUSR1, armar_sinais);
    signal(SIGCHLD, armar_sinais);
    signal(SIGINT, armar_sinais);

    while (1){
        check_arrays();
        if (flag == 2){
           receber_pedidos();
           verificar_condicoes_vacinacao();
        } else if (flag == 4){
            atualizacao_vacinacoes();
            sucesso("S5.5.3.5) Retorna");
        }
        check_arrays();
        sucesso("S4) Servidor espera pedidos");
        pause();
    }  
}