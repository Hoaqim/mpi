#include "main.h"
#include "watek_glowny.h"


void resetPyrkon(){
    for(int i =0;i<number_of_participants;i++){
        finished[i] = 0;
        zaakceptowani[i] = 0;
        workshop_count[i] = 0;
        for(int j=0;j<number_of_people_per_workshop+1;j++){
            my_workshops[i][j] = 0;
        }
    }

    for(int i=0;i<=number_of_workshops;i++){
        indexes_for_waiting_queue[i] = 0;
        for(int j=0;j<=number_of_participants;i++){
            waiting_queue[i][j] = 0;
        }
    }

    
    
}

void mainLoop()
{
    srandom(rank);
    int tag;
    int perc;
    packet_t pakiet;
    int prevWorkshop;
    MPI_Status status;
    while (stan != InFinish) {
        
        switch(stan){
            //Przydałyby się stany:
            //beginPyrkon inicjalizacja eventu
            //duringPyrkon
            //wantPyrkon tutaj kolejka, bo nie ma miejsca
            //beginWorkshop chcemy i zaczynamy
            //wantWorkshop tutaj kolejka, bo nie ma miejsca
            //duringWorkshop
            //finishedWorkshops

            case beginPyrkon:

                my_workshops[rank][0] = 0; //pyrkon
                //rozmieszczenie uczestników po warsztatach
                for(int i=1;i<=number_of_workshops_per_participant;i++){
                    int participant = random()%number_of_workshops+1;
                    for(int j=0;j<i;j++){
                        participant = random()%number_of_workshops+1;
                        j = 0;
                    }
                    my_workshops[rank][i] = participant;
                }
                for(int i=1;i<=number_of_workshops_per_participant;i++){
                    //wypisanie wybrał jaki workshop
                    println("%d: my workshop: %d\n",rank,my_workshops[rank][i])
                }
                changeState(duringPyrkon); //trzeba zaimplementować funkcje do zmiany stanów, jakieś mutexy czy coś

            case duringPyrkon:
                // perc = random()%100;
                // if(perc<25) nwm w sumie po co to 
                if(workshop_count[rank] == 0){
                    println("I want to go to Pyrkon")
                } else{
                    println("I want to get to %d workshop",id_workshopu)
                }
                //Wysyłanie requestów
                packet_t *pakiet = malloc(sizeof(packet_t));
                pakiet -> data = perc;
                pakiet -> id_workshopu = my_workshops[rank][workshop_count[rank]];
                zaakceptowani[rank] = 0;
                for(int i =0; i<=size-1;i++){ //broadcast
                    if (i!= rank){
                        sendPacket(pakiet,i,WANT_TICKET,pakiet->id_workshopu);
                        if(pakiet->id_workshopu == 0){
                            println("Sending request for Pyrkon");

                        } else{
                            println("Sending request for workshop %d",pakiet -> id_workshopu);
                        }
                    }
                }
                if(workshop_count[rank] == 0){
                    changeState(wantPyrkon);
                } else {
                    changeState(wantWorkshop);
                }
                free(pakiet);
            case wantPyrkon:
                //Oczekiwanie na wejscie na pyrkon
                if(zaakceptowani[rank] >= number_of_participants - number_of_tickets){
                    //wejście na pyrkon
                    workshop_count[rank] += 1;
                    zaakceptowani[rank] = 0;
                    println("I've entered pyrkon");
                    //on_pyrkon[rank] = 1; //jakaś flaga, nwm czy mamy coś takiego
                    //można wyprinotować kolejke na pyrkon
                    changeState(duringPyrkon);
                }
                break;
            case wantWorkshop:
                if(zaakceptowani[rank] >= number_of_participants - number_of_people_per_workshop){
                    //wejscie na warsztat
                    println("I've entered %d workshop",id_workshopu);
                    zaakceptowani[rank] = 0;
                    workshop_count[rank] += 1;
                    changeState(duringWorkshop);
                }
                break;
            case duringWorkshop:
                sleep(2); //siedi sobie w warsztacie
                prevWorkshop = my_workshops[rank][workshop_count[rank]-1];
                println("Leaving %d workshop",prevWorkshop);
                //tutaj jakiś broadcast że wyszedł
                pakiet->data = perc;
                for(int i=0;i<=number_of_participants-1;i++){ //broadcast do wszystkich na warsztacie
                    if(i!=rank){
                        sendPacket(0,i,WORKSHOP_FINISH,prevWorkshop);
                    }
                }
                for(int i=0;i<indexes_for_waiting_queue[prevWorkshop];i++){
                    sendPacket(0,waiting_queue[prevWorkshop][i],WANT_TICKET_ACK,prevWorkshop);
                    //Tu broadcast ACK dla wszystkich czekających
                }
                indexes_for_waiting_queue[prevWorkshop] = 0;//tu niby zerowanie tej kolejki workshopa z którego wyszedł ale nwm o co cho
                if(workshop_count[rank]>number_of_workshops_per_participant){
                //jak skończył cały przydział warsztatów
                    println("Leaving pyrkon");
                    //on_pyrkon[rank] = 0; //tu ta flaga ponownie
                    //wysyłanie na broadcast, że opuszcza pyrkon dla uczestników
                    for(int i=0; i<=number_of_participants-1;i++){
                        if(i!=rank){
                            sendPacket(0,i,PYRKON_FINISH,0);
                        }
                    } 
                    //wysyłanie na broadcast, że opuszcza pyrkon do całej kolejki z akceptem
                    for(int i =0;indexes_for_waiting_queue[0];i++){
                        sendPacket(0,waiting_queue[0][i],PYRKON_FINISH,0);
                    }
                    indexes_for_waiting_queue[0] = 0;
                    changeState(finishedWorkshops); //jak skonczy wszystkie
                    
                } else{
                    changeState(duringPyrkon); //jak nie skonczył wszystkich
                }
                
                break;

            case finishedWorkshops:
                println("Done with Pyrkon");
                //czekanie aż każdy skończy Pyrkon, nwm 
                //coś innego niż aktywne czekanie może, na razie aktywne
                while(finished[rank] < number_of_participants-1){
                    //czeka aż wszyscy skończą
                }
                //reset zmiennych
                resetPyrkon();
                println("Pyrkon is over.");
                sleep(5);
                changeState(beginPyrkon);
                break;
            default:
                break;
            




        }

    
    //TO ze szkieletu jest
    //     if (perc<STATE_CHANGE_PROB) {
    //         if (stan==InRun) {
	// 	debug("Zmieniam stan na wysyłanie");
	// 	changeState( InSend );
	// 	packet_t *pkt = malloc(sizeof(packet_t));
	// 	pkt->data = perc;
	// 	perc = random()%100;
	// 	tag = ( perc < 25 ) ? FINISH : APP_PKT;
	// 	debug("Perc: %d", perc);
		
	// 	sendPacket( pkt, (rank+1)%size, tag);
	// 	changeState( InRun );
	// 	debug("Skończyłem wysyłać");
    //         } else {
    //         }
    //     }
    //     sleep(SEC_IN_STATE);
    }
}
