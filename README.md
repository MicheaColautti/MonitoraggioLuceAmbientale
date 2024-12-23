# Prog15 - Monitoraggio Luce Ambientale

## Descrizione del Progetto
**Prog15 - Monitoraggio luce ambientale** è un sistema progettato per misurare la quantità di luce ambientale e ottimizzare l'illuminazione artificiale. Utilizzando una scheda BasysMX3 e un sensore di luce Grove (TAOS-TSL2561), il sistema controlla i LED della board in base alla luce naturale rilevata.

### Funzionalità principali:
1. **Avvio del monitoraggio luce ambientale**
   - Visualizzazione in tempo reale dell'intensità luminosa in LUX e del numero di LED accesi.
2. **Visualizzazione dell'ultima misurazione in LUX**
   - Recupero dell'ultima misura registrata dalla memoria flash.
3. **Reset della misurazione luminosa**
   - Cancellazione della memoria flash.

### Hardware Utilizzato:
- **Microcontrollore:** PIC32MX370F512L
- **Sensore di luce:** TAOS-TSL2561
- **Scheda:** BasysMX3

### Specifiche Tecniche:
- **Ingressi/Uscite:** UART, I2C, GPIO
- **Altre periferiche:** PMP (per LCD), SPI (memoria flash), Timer (PWM e Delay)
- **Eventi:** Interrupt esterno (BTNC)

## Cronologia del Progetto
| **Data di Inizio** | **Data di Consegna** |
|---------------------|----------------------|
| 13 dicembre 2024    | 17 gennaio 2025     |

## Sviluppatori
- **Michea Colautti**
- **Julian Cummaudo**
- **Giada Galdiolo**
- **Ambra Giuse Graziano**
- **Roeld Hoxa**

## Consegna
Per maggiori dettagli sul progetto, consulta la consegna completa:  
[Prog15 - Monitoraggio luce ambientale.pdf](./Documents/Prog15%20-%20Monitoraggio%20luce%20ambientale.pdf)

## Documentazione
Per maggiori dettagli sul progetto, consulta la consegna completa:  
[Prog15 - Monitoraggio luce ambientale.pdf](./Documents/Prog15%20-%20Monitoraggio%20luce%20ambientale.pdf)