#!/bin/bash

###############################################################################
## ISCTE-IUL: Trabalho prático de Sistemas Operativos
##
## Aluno: Nº: 98475    Nome: Salvador de Oliveira Carvalho Nunes Domingues
## Nome do Módulo: lista_cidadaos.sh
## Descrição/Explicação do Módulo: Este módulo irá recolher os dados dos utentes presentes no ficheiro 'listagem.txt', procedendo à criação
## de um novo ficheiro com o nome 'cidadaos.txt'. Este módulo, quando executado, apresentará a "lista dos cidadãos do sistema Covid-IUL" no devido formato.
## Explicação do código ao longo do script:
###############################################################################

## cat listagem.txt: Mostrar os dados presentes no ficheiro listagem.txt;
## -F'[:  -]': Definir que o separador entre parâmetros do ficheiro listagem.txt é ":" ou " " ou "-";
## 'BEGIN{ev=0}: Irá começar com o "ev" (estado vacinação) a 0;
## {print(NR) + 10000":"$2 " "$3":"((2021-$10))":"$13":"$16":"ev}' > cidadaos.txt: O "NR" serve para numerar cada linha do ficheiro listagem.txt
## de forma a que se possa somar 10000 para definir o número de cada utente. Como um dos separadores é "-", assim nos facilita o cálculo da idade
## sendo apenas necessário fazer o cálculo do ano atual (2021) e subtrair o ano de nascimento do utente. Assim por fim, dá-se à criação do ficheiro 
## cidadaos.txt com a informação recolhida a partir do ficheiro listagem.txt, apagando os dados anteriormente introduzidos, e atualizando com os novos:  
awk -F'[:  -]' 'BEGIN{ev=0} {print(NR) + 10000":"$2 " "$3":"((2021-$10))":"$13":"$16":"ev}' listagem.txt > cidadaos.txt

## Permite-nos a visualização do ficheiro cidadaos.txt já com os dados atualizados:
cat cidadaos.txt