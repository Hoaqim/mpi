#include "main.h"
#include "watek_komunikacyjny.h"

/* wątek komunikacyjny; zajmuje się odbiorem i reakcją na komunikaty */
void *startKomWatek(void *ptr)
{
    MPI_Status status;
    int is_message = FALSE;
    packet_t pakiet;
    /* Obrazuje pętlę odbierającą pakiety o różnych typach */
    while ( stan!=InFinish ) {
        packet_t pakiet;
        MPI_Status status;
	    debug("czekam na recv");
        MPI_Recv( &pakiet, 1, MPI_PAKIET_T, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        
        //Przychodzi pakiet do wejścia na pyrkon, dostajemy broadcast
        pthread_mutex_lock(&clockMutex);
        if(clock >= pakiet.ts){
            clock += 1 //wtedy dostajemy bilet i operacja zwiększa lamporta?
        } else{
            clock = pakiet.ts + 1; 
        }
        //aktualizacja clocka lamporta (max z własnego clocka i timestampu pakietu + 1)
        pthread_mutex_unlock(&clockMutex)

        //Obsługa żądań
        //Tu by trzbea określić jakiś workshop, który jest teraz przetwarzany chyba
        id_workshopu = workshop[rank][workshop_count[rank]] // <- aktualny workshop 
        if(status.MPI_TAG == WANT_TICKET){
            //zakładając, że warsztaty idą od 1, a 0 to bilet na pyrkon
            //i że mamy id workshopu w structcie pakietu
            if(pakiet.id_workshopu == 0){
                //Akcept na bilet na pyrkon
            } else{
                //Akcept na bilet na warsztat
            }
        }
        //Jak zaakceptowany na aktualnie przetwarzany warsztat
        if(status.MPI_TAG == WANT_TICKET_ACK && pakiet.id_workshopu == id_workshopu){
            zaakceptowani[rank] += 1; //chyba 
        }
        else if(status.MPI_TAG == WANT_TICKET){
            if(pakiet.id_workshopu == 0){
                //Request na pyrkon
            }
            else if(id_workshopu == pakiet.id_workshopu){ // jak request na aktualny workshop
                if(pakiet.ts < timestamp_requesta_warsztatu || pakiet.ts == timestamp_requesta_warsztatu && status.MPI_SOURCE < rank){
                    //Jak ma albo mniejszy timestamp niż timestamp requesta, albo jak ma taki sam ale mniejszą range
                    //Wysyłam akcepta na warsztat
                }
                else{
                    //kolejka oczekujących na warsztat.append(MPI_SOURCE)
                }
            }
            else{ //jak request na inny warsztat

            }   
        } else if(status.MPI_TAG == FINISH)
            zakończono[rank] += 1
        }



        // switch ( status.MPI_TAG ) {
	    // case FINISH: 
        //         changeState(InFinish);
        //         zakończono[rank] += 1;
	    // break;
	    // case APP_PKT: 
        //         debug("Dostałem pakiet od %d z danymi %d",pakiet.src, pakiet.data);
	    // break;
	    // default:
	    // break;
        // }
    }
