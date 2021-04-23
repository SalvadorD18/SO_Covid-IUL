#!/bin/bash

###############################################################################
## ISCTE-IUL: Trabalho prático de Sistemas Operativos
##
## Aluno: Nº: 98475  Nome: Salvador de Oliveira Carvalho Nunes Domingues
## Nome do Módulo: stats.sh
## Descrição/Explicação do Módulo: Este módulo serve para apresentar informação relativa às estatísticas referentes ao número  
## de cidadãos registados na localidade indicada pelo utilizador, assim como apresentar uma lista dos utentes com mais de 60 anos
## registados na plataforma e ainda uma lista dos enfermeiros disponíveis da plataforma.
## Explicação do código ao longo do script:
###############################################################################

## Criação de uma variável relativa ao segundo argumento que o utilizador irá fornecer, que será, neste caso a localidade, pois é 
## o único pedido que exige um 2º argumento. Assim, irá utilizar a variável para contar o número de linhas do ficheiro cidadaos.txt,
## que satisfaçam a localidade inserida pelo utilizador:
l=$2
nrl=$(grep "$l" cidadaos.txt | wc -l)

## Caso o 1º argumento ($1) indicado pelo utilizador seja "cidadaos", o script irá indicar a informação do nº de cidadãos registados nessa localidade:
if [ $1 = "cidadaos" ]
then echo "O número de cidadãos registados em $l é $nrl."

## Caso o 1º argumento ($1) indicado pelo utilizador seja "registados", o script irá apresentar uma lista. Esta lista deve-se ao comando awk
## que neste caso nos limita aos utentes maiores de 60 anos, e que "imprime" a lista com o nome, nº de utente e idade, e que o sort irá ordenar
## do mais velho para o mais novo (-nrk3) em que o 'n' é para ordenar numericamente, o 'r' é para ordenar inversamente, pois por pré-definição iria
## ordenar do menor númeor para o maior e o 'k3' serve para indicar a coluna à qual o sort se deve guiar. Por fim, o cut irá retirar a idade dessa lista,
## sendo que apenas nos servia para a ordenar:
elif [ $1 = "registados" ]
then awk -F'[:]' '{if ($3 > 60) print $2":"$1":"$3|"sort -nrk3 -t ':' "}' cidadaos.txt | cut -d':' -f1,2

## Caso o 1º argumento ($1) indicado pelo utilizador seja "enfermeiros", o script irá a apresentar também uma lista, em que apresenta o nome dos
## enfermeiros disponíveis na plataforma. Esta lista é-nos também fornecida através do comando awk, em que irá avaliar a 5ª coluna do ficheiro
## enfermeiros.txt, e caso esta coluna apresente o valor '1', o que significa que o enfermeiro está disponível, irá assim "imprimir" uma lista
## com a 2ª coluna do ficheiro enfermeiros.txt, que é referente ao nome dos enfermeiros.
elif [ $1 = "enfermeiros" ]
then awk -F'[:]' '{if ($5 == 1) print $2}' enfermeiros.txt
fi