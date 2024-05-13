#ifndef SIMULACRO_SO_EVENTOS_H
#define SIMULACRO_SO_EVENTOS_H

// interrupção gerada pela execução da função processCreate e pelo término de operações de E/S
void processInterrupt();

// tratamento de bloqueio de processo
void semaphoreP(int v);

// tratamento de desbloqueio de processo
void semaphoreV(int v);

// chamada de operação de E/S para disco
void DiskRequest();

// sinalização de final de E/S para disco
void DiskFinish();

// chamada de operação de E/S para impressão
void PrintRequest();

//  sinalização de final de E/S para impressão
void PrintFinish();

// chamada de operação de carregamento na memória
void memLoadReq();

// sinalização de final de carregamento
void memLoadFinish();

//  chamada para operação no sistema de arquivos
void fsRequest();

// sinalização de final de operação no sistema de arquivos
void fsFinish();

// chamada para iniciar a criação de um processo no BCP
void * processCreate();

// chamada para terminar a existência de um processo no BCP
void processFinish();

#endif //SIMULACRO_SO_EVENTOS_H
