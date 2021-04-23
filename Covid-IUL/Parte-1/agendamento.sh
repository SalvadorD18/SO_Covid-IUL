#!/bin/bash

###############################################################################
## ISCTE-IUL: Trabalho prático de Sistemas Operativos
##
## Aluno: Nº: 98475  Nome: Salvador de Oliveira Carvalho Nunes Domingues 
## Nome do Módulo: agendamento.sh
## Descrição/Explicação do Módulo: Este módulo irá servir para agendar as vacinações, de cada cidadão por localidade, 
## mediante disponibilidade do enfermeiro nessa mesma localidade.
##
###############################################################################

## Eliminação do ficheiro agenda.txt, caso este já exista:
rm agenda.txt

## Criação de variável para a data atual, no formato pedido, 'ANO-MÊS-DIA':
d=$(date +"%Y-%m-%d")

## Neste ciclo, o ficheiro enfermeiros.txt irá ser lido linha a linha, com o intuito de "captar" os enfermeiros que estejam disponíveis, ou seja, que a última coluna
## seja igual a "1". Dá-se também à criação de 3 variáveis (nome_e, cedula e localidade), para mais tarde serem utilizadas no registo das vacinações no ficheiro agenda.txt,
## de forma a que cada cidadão fique registado com o enfermeiro da respetiva localidade. E ainda uma outra variável (disponibilidade) que serve para ter como condição (if),
## apenas os enfermeiros disponíveis.
while read -r ed
do
  nome_e=$( echo "$ed" | cut -d ':' -f2)
  cedula=$( echo "$ed" | cut -d ':' -f1)
  localidade=$( echo "$ed" | cut -d ':' -f3 | sed 's/.*CS//')
  disponibilidade=$( echo "$ed" | cut -d ':' -f5)
  if [[ $disponibilidade == 1 ]]
  then
    lc=$(grep "$localidade" cidadaos.txt) ## Procura de cidadãos que sejam da mesma localidade que o enfermeiro marcado como disponível.
    while read -r line_c 
    do
      ## Criação de duas variáveis para facilitar a criação da lista que vai ficar registada no ficheiro agenda.txt, relativas ao nome do cidadão e ao número do mesmo:
      nome_c=$( echo "$line_c" | cut -d ':' -f2)
      numero_u=$( echo "$line_c" | cut -d ':' -f1)
      localidade_c=$( echo "$line_c" | cut -d ':' -f4)

      if [[ $localidade_c == $localidade ]]
      then

        ## Por fim, iremos assim juntar as respetivas informações, e registá-las, com o formato pedido, no ficheiro agenda.txt:
        echo "$nome_e:$cedula:$nome_c:$numero_u:CS$localidade:$d" >> agenda.txt
      fi


    done <<< $lc
  fi
done < enfermeiros.txt

## Após a execução do script, irá ser apresentada a lista dos utentes com as vacinações agendadas, registadas no ficheiro agenda.txt:
cat agenda.txt