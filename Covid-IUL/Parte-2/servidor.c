/******************************************************************************
 ** ISCTE-IUL: Trabalho prático de Sistemas Operativos
 **
 ** Aluno: Nº: 98475  Nome: Salvador de Oliveira Carvalho Nunes Domingues  
 ** Nome do Módulo: servidor.c
 ** Descrição/Explicação do Módulo: 
 **
 **
 ******************************************************************************/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include "common.h"

Vaga vagas[NUM_VAGAS];
Enfermeiro* enf;
int n_enf;
int flag = 0;

void registo_servidor(){
    FILE* s = fopen(FILE_PID_SERVIDOR, "w");
    int a = access(FILE_PID_SERVIDOR, F_OK); 
    if (a == -1){
        erro("S1) Não consegui registar o servidor!");
    } else {
    int p = getpid();
    fprintf(s, "%d\n", p);
    sucesso("S1) Escrevi no ficheiro FILE_PID_SERVIDOR o PID: %d", p);
    fclose (s);
    }
}

void enfermeiros(){ // Nº de enfermeiros = 10
    FILE* le;
    int a = access(FILE_ENFERMEIROS, F_OK); 
    le = fopen(FILE_ENFERMEIROS, "rb");
    if (a == -1){   
        erro("S2) Não consegui ler o ficheiro FILE_ENFERMEIROS!"); 
        exit(-1); 
        }
    fseek(le, 0, SEEK_END);
    long size = ftell(le);
    n_enf = size/sizeof(Enfermeiro);
    fseek(le, 0, SEEK_SET);
    debug("Li %d", size);
    enf = (Enfermeiro*) malloc (size * sizeof(Enfermeiro)); 
    if (enf == NULL){
        perror("Não consegui reservar memória!\n");
        exit(0);
    } else {
        fread(enf, n_enf * sizeof(Enfermeiro), 1, le);
        sucesso("S2) Ficheiro FILE_ENFERMEIROS tem %d bytes, ou seja, %d enfermeiros", size, n_enf);
    }
}

void check_arrays(){

    if( DEBUG_MODE == 1){
        debug("\nID - Cedula - Nome - CS - Vacinas Dadas - Disponibilidade" );
        for( int i = 0;  i < n_enf; i++){
            printf("Enfermeiro: [%d] [%d] [%s] [%s] [%d] [%d] \n", i , enf[i].ced_profissional, enf[i].nome, enf[i].CS_enfermeiro, enf[i].num_vac_dadas, enf[i].disponibilidade); 
        }
        debug("\nID - PID Filho - Index Enfermeiro - PID Cidadão" );
        for( int i = 0;  i < NUM_VAGAS ; i++){
            printf("Vagas: [%d] [%d] [%d] [%d] \n", i, vagas[i].PID_filho, vagas[i].index_enfermeiro, vagas[i].cidadao.PID_cidadao); 
        }
    }
}

void iniciar_lista_de_vagas(){
    for( int i = 0;  i < NUM_VAGAS ; i++){
    vagas[i].index_enfermeiro = -1;
    }
    sucesso("S3) Iniciei a lista de %d vagas", NUM_VAGAS);
}

int ler_dados(){
    Cidadao c;
    const char u[300];
    FILE* v;
    v = fopen(FILE_PEDIDO_VACINA, "r");
    int a = access(FILE_PEDIDO_VACINA, F_OK);
    if (a == -1){
        erro("S5.1) Não foi possível abrir o ficheiro FILE_PEDIDO_VACINA");
        return -1;
    } else if (errno == EACCES) {
        erro("S5.1) Não foi possível ler o ficheiro FILE_PEDIDO_VACINA");
        return -1;
    } else if (v) {
        fscanf(v, "%[^\n]", u);
        sscanf(u, "%d:%[^:]:%d:%[^:]:%[^:]:%d:%d", &c.num_utente, c.nome, &c.idade, c.localidade, c.nr_telemovel, &c.estado_vacinacao, &c.PID_cidadao);
        printf("Chegou o cidadão com o pedido nº %d, com nº utente %d, para ser vacinado no Centro de Saúde %s\n", c.PID_cidadao, c.num_utente, c.localidade);
        sucesso("Dados Cidadão: %d; %s; %d; %s; %s; 0", c.num_utente, c.nome, c.idade, c.localidade, c.nr_telemovel);
        fclose (v);
    }
    char centro_saude[] = "CS";
    strcat(centro_saude, c.localidade);
    for( int i = 0;  i < n_enf; i++){
        if(strcmp(centro_saude, enf[i].CS_enfermeiro) == 0){ // Procura o enfermeiro disponível para a localidade inserida pelo cidadão
            if(enf[i].disponibilidade != 1){ // Caso o enfermeiro esteja indisponível
                erro("S5.2.1) Enfermeiro %d indisponível para o pedido %s para o Centro de Saúde %s", i, &c.PID_cidadao, enf[i].CS_enfermeiro);
                kill(c.PID_cidadao, SIGTERM);
                return -1;
            } else {
                sucesso("S5.2.1) Enfermeiro %d disponível para o pedido %d", i, c.PID_cidadao);
                for( int j = 0;  j < NUM_VAGAS ; j++){
                    debug("Olha, aqui estou eu!\n");
                    if (vagas[j].index_enfermeiro == -1) { // Existem vagas para vacinação 
                        sucesso("S5.2.2) Há vaga para vacinação para o pedido %d", c.PID_cidadao);
                        vagas[j].index_enfermeiro = i; 
                        vagas[j].cidadao = c;
                        sucesso("S5.3) Vaga nº %d preenchida para o pedido %d", j + 1, c.PID_cidadao);
                       
                        int pidFilho = fork();
                        if (-1 == pidFilho) {
                        erro("S5.4) Não foi possível criar o servidor dedicado");
                        } else if (0 == pidFilho) {
                            vagas[j].PID_filho = getpid();
                            sucesso("S5.4) Servidor dedicado %d criado para o pedido %d", getpid(), c.PID_cidadao);
                            exit(1);
                        } else {
                            vagas[j].PID_filho = pidFilho;
                            sucesso("S5.5.1) Servidor dedicado %d na vaga %d", pidFilho, j + 1);
                            sucesso("S5.5.2) Servidor aguarda fim do servidor dedicado %d", pidFilho);
                        }
                        return i;
                    }
                } 
                erro("S5.2.2) Não há vaga para vacinação para o pedido %d", c.PID_cidadao); // Não existem vagas para vacinação
                kill(c.PID_cidadao, SIGTERM);
                //} else if (i == 10){ //if (strcmp(centro_saude, enf[i].CS_enfermeiro) != 0)
                //perror("Não está registado nenhum enfermeiro para a localidade inserida.\n"); // mensagem de erro para caso não haja enfermeiro para a localidade 
                //} 
            }
        }
    }
}

// 5.5.1 e 5.5.2 e 5.6_todo (mesma função)
// 5.5.3 e meia (main)
// 5.1 uma função
// 5.2 e 5.3 descobrir vaga - função

void armar_sinais(int s){
    if (s == SIGUSR1){
        flag = 2;
    } else if (s == SIGTERM) {
        flag = 3;
    } else if (s == SIGCHLD) {
        flag = 4;
    }
}

int main(){
    registo_servidor();
    enfermeiros();
    iniciar_lista_de_vagas();
    signal(SIGUSR1, armar_sinais);
    signal(SIGTERM, armar_sinais);
    signal(SIGCHLD, armar_sinais);

    while (1){
        check_arrays();
        if (flag == 2){
           ler_dados();
        } else if (flag == 4){
            // vai fazer o 5.5.3
        } 
        check_arrays();
        sucesso("S4) Servidor espera pedidos");
        pause();
    }
}