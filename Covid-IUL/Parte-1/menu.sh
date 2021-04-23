#!/bin/bash

###############################################################################
## ISCTE-IUL: Trabalho prático de Sistemas Operativos
##
## Aluno: Nº: 98475  Nome: Salvador de Oliveira Carvalho Nunes Domingues
## Nome do Módulo: menu.sh
## Descrição/Explicação do Módulo: Este módulo serve para dar ao utilizador a possibilidade de escolher, por opções, 
## a informação a que quer aceder.
##
###############################################################################

## Utilização de um ciclo (while) para que o o utilizador possa realizar múltiplas operações iterativamente:
while true
do
    echo "MENU:"
    echo " 1. Listar cidadãos"
    echo " 2. Adicionar enfermeiro"
    echo " 3. Stats"
    echo " 4. Agendar vacinação"
    echo " 0. Sair"

    read -p "Opção: " o

    ## Caso a opção escolhida seja a '1', irá ser executado o script lista_cidadaos.sh:
    if [[ $o == 1 ]] 
    then 
        ./lista_cidadaos.sh

    ## Caso a opção escolhida seja a '2', irá ser pedido ao utilizador informaçóes referentes ao enfermeiro que quer registar no sistema, e posterior
    ## apresentação da lista atualizada:
    elif [[ $o == 2 ]]
    then
        echo "Adicione as informações necessárias para adicionar um enfermeiro:"

        read -p "Nome: " n 
        read -p "Nº de Cédula Profissional: " nu
        read -p "Centro de Saúde: " cs
        read -p "Disponibilidade: " d
        echo " "
        ./adiciona_enfermeiros.sh "$n" "$nu" "$cs" "$d"

    ## Caso a opção escolhida seja a '3', irá ser dada a possibilidade de o utilizador escolher uma das três opções disponíveis, para que lhe sejam
    ## apresentadas as respetivas informações:
    elif [[ $o == 3 ]]
    then
        echo "Prima a opção respetiva à informação que quer consultar:"
        echo "1. Número de cidadãos registados numa dada localidade;"
        echo "2. Lista dos cidadãos registados na plataforma com mais de 60 anos;"
        echo "3. Lista dos enfermeiros disponíveis na plataforma."

        read -p "Opção: " s
        if [[ $s == "1" ]]
        then
            read -p "Insira a localidade: " l
            ./stats.sh cidadaos $l
        
        elif [[ $s == "2" ]]
        then
        ./stats.sh registados

        elif [[ $s == "3" ]]
        then
        ./stats.sh enfermeiros

        fi

    ## Caso a opção escolhida seja a '4', irá ser executado o script agendamento.sh:
    elif [[ $o == 4 ]]
    then
        ./agendamento.sh

    ## Caso a opção escolhida seja a '0', irá assim sair do menu:
    elif [[ $o == 0 ]]
    then
        exit

    ## Caso a opção escolhida seja algum dígito que não esteja presente no 'MENU', irá ser apresentada uma mensagem de "erro":
    else
        echo "*Opção Inválida*"
    fi
done