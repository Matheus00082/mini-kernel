# Mini-Kernel Multithread

**Matheus Baessa e Rafaela Sousa**

Trabalho Prático de Sistemas Operacionais (INF15980) — simulador de escalonamento
de processos multithread em C com pthreads, em duas versões: **monoprocessador**
e **multiprocessador** (2 CPUs). Suporta três políticas de escalonamento:
**FCFS**, **Round-Robin** (quantum de 500 ms) e **Prioridade Preemptiva**.

## Compilação e execução

```bash
make monoprocessador     # ou: make multiprocessador
./trabSO <arquivo_de_entrada>
```

O resultado é gravado em `log_execucao_minikernel.txt`.

## Entrada

Primeira linha: número `n` de processos. Para cada processo: duração (ms),
prioridade (1 = mais alta, 5 = mais baixa), número de threads e tempo de
chegada (ms). Última linha: política (1 = FCFS, 2 = RR, 3 = Prioridade).

## Organização do código

| Diretório | Conteúdo |
|---|---|
| `control_structures/` | BCP (bloco de controle de processo) e TCB (bloco de controle de thread) |
| `schedulers/` | Laço do escalonador e as filas de prontos de cada política |
| `data_structures/` | Deque circular (FCFS/RR), heap de mínimo (Prioridade) e vetor |
| `monocore/` / `multicore/` | `main` de cada versão |

## Decisões de implementação

- Cada processo é um **BCP** com mutex próprio e duas variáveis de condição:
  uma libera as threads para executar, a outra avisa o escalonador que um
  burst terminou. Cada thread recebe um **TCB** (ponteiro para o BCP + índice)
  e divide igualmente a duração do processo (`duração / nº de threads`).
- A execução é feita em **bursts de até 500 ms concedidos pelo escalonador**:
  a cada fatia/quantum é liberada uma "vaga" e exatamente uma thread a consome.
  No monoprocessador as threads do processo revezam a CPU; no multiprocessador
  cada processador concede a sua própria vaga (modelagem 1:1 de threads).
- A **fila de prontos é genérica** (ponteiros de função): deque circular com
  crescimento dinâmico para FCFS/RR e heap de mínimo para Prioridade, com
  desempate por ordem de chegada à fila.
- **RR/Prioridade**: o processo segura o processador até a interrupção de
  timer mesmo que precise de menos que o quantum; a preempção (inclusive por
  prioridade) ocorre nas fronteiras de quantum, reinserindo o processo na fila.
- **FCFS multiprocessador**: os processadores dão vazão aos processos; um
  núcleo livre, sem ninguém na fila de prontos, ajuda nas threads do processo
  em execução no outro núcleo (uma CPU por thread de cada vez).
- **Encerramento**: o escalonador termina quando todos os processos já
  chegaram, a fila está vazia e nenhum núcleo está ativo. O `main` faz join de
  todas as threads e libera toda a memória (execução limpa no valgrind).