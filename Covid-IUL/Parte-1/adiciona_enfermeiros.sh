#!/bin/bash

###############################################################################
## ISCTE-IUL: Trabalho prático de Sistemas Operativos
##
## Aluno: Nº: 98475   Nome: Salvador de Oliveira Carvalho Nunes Domingues
## Nome do Módulo: adiciona_enfermeiros.sh
## Descrição/Explicação do Módulo: Este módulo visa facilitar o registo dos enfermeiros no sistema Covid-IUL, de forma a que cada enfermeiro 
## fique devidamente registado, tendo consigo agregado o seu número de cédula profissional, o centro de saúde a que pertence, o número de vacinações
## efetuadas e a sua disponibilidade. Permite-nos ainda que não haja registos de mais do que um enfermeiro por centro de saúde, e que, um enfermeiro não esteja registado
## em mais do que um centro de saúde.
## Explicação do código ao longo do script:
###############################################################################

## Criação do ficheiro enfermeiros.txt, vazio:
touch enfermeiros.txt

## Atribuição de uma variável ao número de vacinações efetuadas, sendo que, apesar de iniciar a 0, irá futuramente ser alterado:
nve=0;

## Atribuição de variáveis aos valores que se quer verificar se já estão ou não presentes no ficheiro enfermeiros.txt, nomeadamente o
## centro de saúde e o número de cédula profissional:
cs=$3;
ncp=$2;

##r=$( awk -v search="$cs" '$3 ~ search' enfermeiros.txt | wc -l )

## Utilização do comando 'grep' para procurar a possível repetição dos parâmetros do centro de saúde e do nº de cédula profissional no ficheiro 
## enfermeiros.txt, e contar as linhas que satisfaçam. Atribuição deste comando a uma respetiva variável:
r=$( grep "$cs" enfermeiros.txt | wc -l )
n=$( grep "$ncp" enfermeiros.txt | wc -l )


## Neste 'if' iremos verificar se a síntaxe dos dados introduzidos está correta, pela utilização de expressões regulares, e caso não esteja, irá 
## de imediato ser apresentado um erro, de forma a que o utilizador introduza os dados na forma correta para que o registo seja efetuado com sucesso.
## Irá garantir que não sejam introduzidos nem mais nem menos do que 4 argumentos, que no campo 'nome' e 'centro de saúde' não sejam aceites dígitos
## bem como garantir que a sigla "CS" esteja sempre presente na indicação do centro de saúde. Garantir ainda que nos campos do 'número de útente' e na 
## 'disponibilidade' sejam apenas aceites dígitos, e restrigir as hipóteses de apenas "0" ou "1" no campo da 'disponibilidade'.
if [[ ! $# =~ 4 || ! $1 =~ [A-Za-z] || ! $2 =~ [0-9] || ! $3 =~ CS[a-zA-Z] || ! $4 =~ [0-1] ]];
then
  echo "Erro de Síntaxe: O formato aceitável para adicionar um enfermeiro é: <nome> <nº de cédula profissional> <centro de saúde associado> <disponibilidade>
  Notas:
  -O nome do enfermeiro tem de estar entre aspas;
  -O campo do centro de saúde terá também de estar entre aspas, assim como terá que ter sempre a sigla «CS» antes da localidade desse mesmo centro de saúde."

## Nestes dois 'elif'(else if), irá ecoar os seguintes erros, caso a nossa anterior pesquisa e contagem das linhas dê '1', pois assim significa que
## a tentativa de inserir os respetivos dados, nomeadamente relativo ao centro de saúde e ao nº de cédula profissional, estarão já presentes no ficheiro
## enfermeiros.txt, não respeitando o pedido do sistema para que cada centro de saúde só tenha um enfermeiro registado, e para que, cada enfermeiro
## só esteja registado apenas num centro de saúde:
elif [[ $r == 1 ]];
then
  echo "Erro: O Centro de Saúde introduzido já tem um enfermeiro registado"
elif [[ $n == 1 ]];
then
  echo "Erro: O enfermeiro introduzido já está registado noutro Centro de Saúde"

## Por fim, mas não menos importante, mediante os dados introduzidos corretamente, e sem "repetição" de dados, irão assim ser adicionados ao ficheiro
## enfermeiros.txt no formato pedido, que após a execução do script, irá mostrar a lista dos enfermeiros atualizada:
else
  echo $2":"$1":"$cs":"$nve":"$4 >> enfermeiros.txt
  cat enfermeiros.txt
fi