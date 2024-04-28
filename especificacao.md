# Projeto - Especificação Completa ::: 

O projeto consiste na implementação de um simulador do núcleo de um SO, com uma interface gráfica elaborada de modo a permitir escolhas (controle do sistema) por parte de quem executa a simulação. A seguir temos a descrição geral do projeto e os links (quando dispoíveis) para os entregáveis parciais.
 
## Descrição Geral
 
Implementar o "núcleo" de um SO, partindo da implementação das threads definidas a seguir. A chamada por uma delas será controlada através de duas funções que retornam valores inteiros, que correspondem ao código identificador de cada thread:
 
*interruptControl*, acionada na ocorrência de eventos externos aos processos sendo executados, e
*sysCall*, acionada na ocorrência de eventos gerados pelos processos executados.
 
A partir do inteiro obtido de uma dessas funções o núcleo ativa a thread correspondente, bem como o escalonamento do próximo processo usando o algoritmo SRTF.
As origens possíveis para os eventos a serem tratados pelo núcleo, sendo que os números entre parenteses identificam cada evento, são:

* *processInterrupt* (1) -- interrupção gerada pela execução da função processCreate e pelo término de operações de E/S
* *semaphoreP* (10) -- tratamento de bloqueio de processo
* *semaphoreV* (11) -- tratamento de desbloqueio de processo
* *DiskRequest* (4) -- chamada de operação de entrada/saída para disco
* *DiskFinish* (5) -- sinalização de final de E/S para disco
* *PrintRequest* (14) -- chamada de operação de entrada/saída para impressão
* *PrintFinish* (15) -- sinalização de final de E/S para impressão
* *memLoadReq* (6) -- chamada de operação de carregamento na memória
* *memLoadFinish* (7) -- sinalização de final de carregamento
* *fsRequest* (8) -- chamada para operação no sistema de arquivos
* *fsFinish* (9) -- sinalização de final de operação no sistema de arquivos
* *processCreate* (2) -- chamada para iniciar a criação de um processo no BCP*
* *processFinish* (3) -- chamada para terminar a existência de um processo no BCP* 

\* BCP (Bloco de Controle de Processos) é a estrutura que armazena os dados dos processos em execução 
 
 
## Funcionamento do sistema

O tratamento do sistema deve ser feito a partir do conceito de um relógio virtual, que avança de acordo com a duração das operações executadas. As funções *interruptControl* e *sysCall* devem atuar a partir de eventos determinados por comandos de programas sintéticos (definidos a seguir). Essas duas funções retornam valores que serão usados para ativar algum dos threads descritos anteriormente.
 
## Definição de programa sintético

Um programa sintético é definido através de um arquivo (lido pelo SO) que é composto por duas partes. A primeira (cabeçalho) contém dados sobre:
- nome do programa (uma palavra)
- identificador do segmento (um inteiro)
- prioridade original do programa (um inteiro)
- tamanho do segmento (em kbytes)
- lista de semáforos usados pelo programa (separados por espaços em branco)

A segunda parte contém uma sequência de comandos que serão virtualmente executados pelo SO. Os comandos inicialmente previstos são:
- exec t (indicando que o programa executará por t unidades de tempo)
- read k (indicando que o programa lerá do disco, na trilha k)
- write k (indicando que o programa escreverá no disco, na trilha k)
- P(s) (indicando que o programa acessará a região crítica guardada por s)
- V(s) (indicando que o programa liberará a região crítica guardada por s)
- print t (indicando que o programa imprimirá resultados por t unidades de tempo)
 
[**Exemplo de programa sintético**](https://www.dcce.ibilce.unesp.br/~aleardo/cursos/sisop/synthetic_2)
 
## Observações
Para melhor entendimento de como programar usando threads veja esse [documento](https://www.dcce.ibilce.unesp.br/~aleardo/cursos/hpc/unixpar2024.pdf).

Para implementar os threads descritos, e suas interações, faça uso das bibliotecas de manipulação de semáforos/threads "semaphore.h" e "pthreads.h", presentes no Linux.

Já as chamadas de P(s) e V(s) que apareçam nos programas sintéticos devem ser emuladas com o uso das implementações de SemaphoreP e SemaphoreV descritas anteriormente.
