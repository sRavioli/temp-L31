# Gioco dell'oca

Implementazione basilare del gioco dell'oca.

Per i cambiamenti al progetti è presente un molto basilare [CHANGELOG.md](./CHANGELOG.md)

## Come navigare nei menù

I menù hanno un carattere tra parentesi quadre:

```txt
MAIN MENU
[n]ew game
[s]aved games
[l]eaderboard
[h]elp
[q]uit
```

Il tasto tra parentesi quadre è quello che va premuto per lanciare l'opzione
corrispondente. Es. premendo `n` (in `[n]ew game`) avvierà una nuova partita, e
così via. Ogni voce di menù ha una lettera tra parentesi quadre che indica che
tasto premere per selezionare quella voce. Non è necessario premere invio in questi
casi.

## Logger

L'implementazione in C contiene un logger basilare per facilitare il debugging del
programma, loggando su file (src/goose.log). Tutte le info riguardo questo sono
presenti in [logger.h](./src/common/inc/logger.h) e [logger.c](./src/common/impl/logger.c)

## Licenza

La licenza è la GNU-GPLv3.0 ([LICENSE.txt](./LICENSE.txt) )
