#include "main.h"
#include "watek_glowny.h"


void resetPyrkon(){
    for(int i =0;i<number_of_participants;i++){
        finished[i] = 0;
        number_of_acks[i] = 0;
        workshop_count[i] = 0;
        on_pyrkon[i] = 0;
        for(int j=0;j<workshopCapacity+1;j++){
            myWorkshops[i][j] = 0;
        }
    }

    for(int i=0;i<=workshopsNumber;i++){
        queue[i] = 0;
        for(int j=0;j<=number_of_participants;i++){
            queue[i][j] = 0;
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
    MPI_Status = status;
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

                myWorkshops[rank][0] = 0; //pyrkon
                //rozmieszczenie uczestników po warsztatach
                for(int i=1;i<=workshopsPerPerson;i++){
                    int participant = random()%workshopsNumber+1;
                    for(int j=0;j<i;j++){
                        participant = random()%workshopsNumber+1;
                        j = 0;
                    }
                    myWorkshops[rank][i] = participant;
                }
                for(int i=1;i<=workshopsPerPerson;i++){
                    //wypisanie wybrał jaki workshop
                    println("%d: my workshop: %d\n",rank,myWorkshops[rank][i])
                }
                changeState(duringPyrkon); //trzeba zaimplementować funkcje do zmiany stanów, jakieś mutexy czy coś

            case duringPyrkon:
                // perc = random()%100;
                // if(perc<25) nwm w sumie po co to 
                if(workshop_count[rank] == 0){
                    println("I want to go to Pyrkon")
                } else{
                    println("I want to get to %d workshop",workshop_id)
                }
                //Wysyłanie requestów
                packet_t *pakiet = malloc(sizeof(packet_t));
                pakiet -> data = perc;
                pakiet -> id_workshopu = myWorkshops[rank][workshop_count[rank]];
                number_of_acks[rank] = 0;
                for(int i =0; i<=size-1;i++){ //broadcast
                    if (i!= rank){
                        sendPacket(pakiet,i,REQUEST,pakiet->id_workshopu);
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
                free(pakiet)
            case wantPyrkon:
                //Oczekiwanie na wejscie na pyrkon
                if(number_of_acks[rank] >= numer_of_participants - number_of_tickets){
                    //wejście na pyrkon
                    workshop_count[rank] += 1;
                    number_of_acks[rank] = 0;
                    println("I've entered pyrkon");
                    on_pyrkon[rank] = 1; //jakaś flaga, nwm czy mamy coś takiego
                    //można wyprinotować kolejke na pyrkon
                    changeState(duringPyrkon);
                }
                break;
            case wantWorkshop:
                if(number_of_acks[rank] >= number_of_participant - workshopCapacity){
                    //wejscie na warsztat
                    println("I've entered %d workshop",workshop_id);
                    number_of_acks[rank] = 0;
                    workshop_count[rank] += 1;
                    changeState(duringWorkshop);
                }
                break;
            case duringWorkshop:
                sleep(2); //siedi sobie w warsztacie
                prevWorkshop = myWorkshops[rank][workshop_count[rank]-1];
                println("Leaving %d workshop",prevWorkshop);
                //tutaj jakiś broadcast że wyszedł
                pakiet->data = perc;
                for(int i=0;i<=number_of_participants-1;i++){ //broadcast do wszystkich na warsztacie
                    if(i!=rank){
                        sendPacket(0,i,RELEASE,prevWorkshop);
                    }
                }
                for(int i=0;i<queue[prevWorkshop];i++){
                    sendPacket(0,queue[prevWorkshop][i],ACK,prevWorkshop);
                    //Tu broadcast ACK dla wszystkich czekających
                }
                queue[prevWorkshop] = 0;//tu niby zerowanie tej kolejki workshopa z którego wyszedł ale nwm o co cho
                if(workshop_count[rank]>workshopsPerPerson){
                //jak skończył cały przydział warsztatów
                    println("Leaving pyrkon");
                    on_pyrkon[rank] = 0; //tu ta flaga ponownie
                    //wysyłanie na broadcast, że opuszcza pyrkon dla uczestników
                    for(int i=0; i<=number_of_participant-1;i++){
                        if(i!=rank){
                            sendPacket(0,i,FINISH,0);
                        }
                    } 
                    //wysyłanie na broadcast, że opuszcza pyrkon do całej kolejki z akceptem
                    for(int i =0;queue[0];i++){
                        sendPacket(0,queue[0][i],ACK,0);
                    }
                    queue[0] = 0;
                    changeState(finishedWorkshops); //jak skonczy wszystkie
                    
                } else{
                    changeState(duringPyrkon); //jak nie skonczył wszystkich
                }
                
                break;

            case finishedWorkshops:
                println("Done with Pyrkon");
                //czekanie aż każdy skończy Pyrkon, nwm 
                //coś innego niż aktywne czekanie może, na razie aktywne
                while(finished[rank] < numer_of_participants-1){
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
