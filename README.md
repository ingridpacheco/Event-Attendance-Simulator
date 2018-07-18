# Trabalho De Avaliação Em Desempenho (MAB 515)

* **Alunos**: 
    
        Ingrid Quintanilha Pacheco
        Júlio Rama Krsna Mandoju

* **Professor**: Paulo Aguiar

## Objetivo:

O trabalho prático foi feito como conclusão do curso de Avaliação em Desempenho, ministrado pelo professor Paulo Aguiar, no período de 2018.1. Ele tinha como objetivo a implementação de um simulador com um único servidor e duas filas, de dados e voz, que possui prioridade sobre o primeiro. Além disso, podia haver preempção ou não, e sua disciplina de atendimento era **FCFS** (First Come - First Served).

> Uma descrição mais detalhada do trabalho pode ser encontrada na primeira seção do relatório.

## Código:

Todo o código do trabalho se encontra dentro da pasta **src**, tendo como arquivo inicial: **simulator.cpp**, e arquivo principal (que executa a maioria das funções): **run_queues.cpp**

Ele foi escrito na linguagem **C++**, pela proximidade dos integrantes com ela, e para geração dos gráficos utilizados no relatório, foi-se construído um pequeno arquivo em **Python**, que lia o arquivo gerado no código principal com os dados e os plotava em um gráfico.

> Os arquivos em Python para plotagem dos gráficos se encontra dentro da pasta graphs.

## Relatório:

Também foi necessário fazer um relatório para maior detalhamento do funcionamento do programa e questões extras. Ele se encontra no caminho raiz do projeto, com o nome: **Trabalho_De_AD.pdf**
