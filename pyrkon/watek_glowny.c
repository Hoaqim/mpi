#include "main.h"
#include "watek_glowny.h"

void reset_variables() {
    for (int i = 0; i < number_of_participants; i++) {
		finished[i] = 0;
        zaakceptowani[i] = 0;
        workshop_count[i] = 0;
        on_pyrkon[i] = 0;
        for (int j = 0; j < number_of_people_per_workshop+1; j++) {
            my_workshops[i][j] = 0;
        }
    }
    for (int i = 0; i < number_of_workshops + 1; i++) {
        indexes_for_waiting_queue[i] = 0;
        for (int j = 0; j < number_of_participants; j++) {
            waiting_queue[i][j] = 0;
        }
    }
}



void mainLoop()
{
    srandom(rank);
    int tag;
    int perc;

	int previous_workshop_id;

	MPI_Status status;
    // int is_message = FALSE;
    packet_t pakiet;
  
	packet_t *pkt = malloc(sizeof(packet_t));
    while (stan != InFinish) {
		id_workshopu = my_workshops[rank][workshop_count[rank]];
	switch (stan) {
		case beginPyrkon:
		my_workshops[rank][0] = 0;
		for (int i=1;i<=number_of_workshops_per_participant;i++){
			int candidate = random()%number_of_workshops + 1;
			for(int j=0;j<i;j++){
				if (my_workshops[rank][j] == candidate){
					candidate = random()%number_of_workshops + 1;
					j = 0;
				}
			}
			my_workshops[rank][i] = candidate;
		}

		for (int i=0;i<=number_of_workshops_per_participant;i++){
			println("%d: Mój warsztat %d\n",rank ,my_workshops[rank][i]);
		}
		fflush(0);
		changeState(duringPyrkon);

	    case duringPyrkon: 
		
		    debug("Perc: %d", perc);
			if(workshop_count[rank] == 0){
				println("Chcę wejść na pyrkon")
			}
			else{
				println("Chcę wejść na warsztat %d", id_workshopu)
			}
		    debug("Zmieniam stan na wysyłanie");
		    packet_t *pkt = malloc(sizeof(packet_t));
		    pkt->data = perc;
			pkt->id_workshopu = my_workshops[rank][workshop_count[rank]];
		    zaakceptowani[rank] = 0;
		    for (int i=0;i<=size-1;i++)
			if (i!=rank){
			    sendPacket(pkt, i, REQUEST_TICKET, pkt->id_workshopu);
				if(pkt->id_workshopu == 0){
					//request na bilet na pyrkon
				}
				else{
					// request na bilet na warsztat
				}
			}
			if(workshop_count[rank] == 0){
				changeState(wantPyrkon);
			}
			else{
				changeState(wantWorkshop);
			}
		    free(pkt);
		
		//debug("Skończyłem myśleć");
		break;

	    case wantPyrkon:
		//czeka na accepty
		if (zaakceptowani[rank] >= number_of_participants - number_of_tickets){
			workshop_count[rank] += 1;
			zaakceptowani[rank] = 0;
			println("Jestem na pyrkonie")
			on_pyrkon[rank] = 1;	
		    changeState(duringPyrkon);
		} 
		break;
		case wantWorkshop:
		println("mój ack_count %d Czekam na wejście na warsztat %d", zaakceptowani[rank],id_workshopu)
		if (zaakceptowani[rank] >= number_of_participants - number_of_people_per_workshop){
			println("Jestem na warsztacie %d", id_workshopu)
			zaakceptowani[rank] = 0;
			workshop_count[rank] += 1;
		    changeState(duringWorkshop);
		}
		break;
		case duringWorkshop:
			sleep(2);
		    //debug("Perc: %d", perc);
			previous_workshop_id = my_workshops[rank][workshop_count[rank]-1];
		    println("Wychodzę z warsztatu %d", previous_workshop_id)
		    debug("Zmieniam stan na wysyłanie");
		    
		    pkt->data = perc;
			for (int i=0;i<=number_of_participants-1;i++){
				if (i!=rank){
					sendPacket( 0, i, RELEASE_TICKET, previous_workshop_id); //Release ticket dla uczestników
				}
			}
			for (int i=0;i<indexes_for_waiting_queue[previous_workshop_id];i++){
				sendPacket( 0, waiting_queue[previous_workshop_id][i], ACCEPT_TICKET, previous_workshop_id); //Accept ticket dla kolejki
			}
			indexes_for_waiting_queue[previous_workshop_id] = 0;
			if (workshop_count[rank] > number_of_workshops_per_participant){ //konczy wszystkie workshopy
				println("Wychodzę z pyrkonu")
				on_pyrkon[rank] = 0;
				debug("Zmieniam stan na wysyłanie");
				for (int i=0;i<=number_of_participants-1;i++){
					if (i!=rank){
						sendPacket( 0, i, FINISH, 0);
					}
				}
				for (int i=0;i<indexes_for_waiting_queue[0];i++){
					// println("wysylam ACK na pyrkon do %d", i);
					sendPacket( 0, waiting_queue[id_workshopu][i], ACCEPT_TICKET, 0); //accept dla wszystkich czekajacych na pyrkon
				}
				indexes_for_waiting_queue[0] = 0;
				changeState(finishedWorkshops);
			}
			else{
				changeState( duringPyrkon );
			}
		    // free(pkt);
		break;
		case finishedWorkshops:
			println("Wychodzę z Pyrkonu i idę do domu.")
			while(finished[rank] < number_of_participants-1){

			}
			
			reset_variables();
			println("KONIEC PYRKONU!!!")
			sleep(2);
			println("Odpalamy nowy Pyrkon!");
			sleep(1);
			changeState(beginPyrkon);
		break;
	    default: 
		break;
        }
        sleep(SEC_IN_STATE);
    }
}
