/******************************************************************************
 ** ISCTE-IUL: Trabalho prático de Sistemas Operativos
 **
 ** Aluno: Nº: 98475  Nome: Salvador de Oliveira Carvalho Nunes Domingues 
 ** Nome do Módulo: cidadao.c
 ** Descrição/Explicação do Módulo: 
 **
 **
 ******************************************************************************/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include "common.h"

int flag = 0;
Cidadao novo_utente(){

    FILE* v = fopen(FILE_PEDIDO_VACINA, "r");
    int a = access(FILE_PEDIDO_VACINA, F_OK);
    Cidadao c;

    printf("Insira o seu número de utente: \n");
    scanf("\n%d[^\n]", &c.num_utente);
    
    if (c.num_utente >= 1){
    
        printf("Nome: ");
        scanf("\n%100[^\n]", c.nome);
    
        printf("Idade: ");
        scanf("\n%d[^\n]", &c.idade);
    
        printf("Localidade: ");
        scanf("\n%100[^\n]", c.localidade);
    
        printf("Nº de Telemóvel: ");
        scanf("\n%10[^\n]", c.nr_telemovel);
    
        c.estado_vacinacao = 0; 
        printf("Estado de Vacinação: %d\n", c.estado_vacinacao);
    
        sucesso("C1) Dados Cidadão: %d; %s; %d; %s; %s; %d", c.num_utente, c.nome, c.idade, c.localidade, c.nr_telemovel, c.estado_vacinacao);   
        
        c.PID_cidadao = getpid();
        sucesso("C2) PID Cidadão: %d", c.PID_cidadao);

        if (a != -1){
        erro("C3) Não é possível iniciar o processo de vacinação neste momento");
        exit(EXIT_SUCCESS);
        } else {
            sucesso("C3) Ficheiro FILE_PEDIDO_VACINA pode ser criado"); 
            v = fopen(FILE_PEDIDO_VACINA, "w");
            if (v) {
            fprintf(v, "%d:%s:%d:%s:%s:%d:%d\n", c.num_utente, c.nome, c.idade, c.localidade, c.nr_telemovel, c.estado_vacinacao, c.PID_cidadao);
            sucesso("C4) Ficheiro FILE_PEDIDO_VACINA criado e preenchido");
            fclose (v);
            } else {
                erro("C4) Não é possível criar o ficheiro FILE_PEDIDO_VACINA");
                exit(EXIT_SUCCESS);
            }
        } 
    } else {
        erro("Número de utente inválido!\n");
        exit(EXIT_SUCCESS);
    }
}       

void armar_sinais(int s){
    int c = getpid();
    if (s == SIGINT){
    sucesso("C5) O cidadão cancelou a vacinação, o pedido nº %d foi cancelado", c);
    flag = 1;
    } else if (s == SIGUSR1){
        sucesso("C7) Vacinação do cidadão com o pedido nº %d em curso", c);
        flag = 2;
    } else if (s == SIGUSR2){
        sucesso("C8) Vacinação do cidadão com o pedido nº %d concluída", c);
        flag = 3;
    }
}

int le_processo(){
    int c;
    FILE* s = fopen(FILE_PID_SERVIDOR, "r");
    int a = access(FILE_PID_SERVIDOR, F_OK);
    if (a == -1){
        erro("C6) Não existe ficheiro FILE_PID_SERVIDOR!");
    } else { 
        fscanf(s, "%d", &c);
        sucesso("C6) Sinal enviado ao Servidor: %d", c);
        kill(c, SIGUSR1);
        fclose(s);
        return 0;
    }
}

int main(){

    novo_utente();      
    le_processo();
    signal(SIGINT, armar_sinais);
    signal(SIGUSR1, armar_sinais);
    signal(SIGUSR2, armar_sinais);
    signal(SIGTERM, armar_sinais);

    while (-1){
        if (flag == 1 || flag == 4 ){
            remove(FILE_PEDIDO_VACINA);
            exit(EXIT_SUCCESS);
        } else if (flag == 2){
            remove(FILE_PEDIDO_VACINA);
        } else if (flag == 3){
            exit(EXIT_SUCCESS);
        }
        pause();
    }                
}                                                                                       