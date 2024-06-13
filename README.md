Michał Domasik 151905, Miłosz Koźlicki 151873

#OPIS ALGORYTMU:
Michał Domasik 151905, Miłosz Koźlicki 151873

#OPIS ALGORYTMU:

W kodzie zostały zastosowane podstawowe zasady algorytmu Ricarta-Agrawali do zarządzania wejściem na Pyrkon oraz warsztaty:

Wysyłanie żądania:

Proces (uczestnik) wysyła żądanie wejścia na Pyrkon lub warsztat do wszystkich innych procesów. Żądanie to zawiera znacznik czasu z logicznego zegara (clock_l).
Oczekiwanie na odpowiedzi:

Proces czeka na odpowiedzi od wszystkich innych procesów (zaakceptowani[rank]). Jeśli liczba otrzymanych ACK jest wystarczająca, uzyskuje dostęp do Pyrkonu lub warsztatu.
Odbiór żądania:

Kiedy proces otrzymuje żądanie (REQUEST_TICKET), sprawdza, czy może wysłać odpowiedź natychmiast (jeśli jest to dozwolone przez warunki algorytmu), czy musi dodać żądanie do kolejki oczekujących (waiting_queue).
Zwolnienie zasobu:

Po zakończeniu warsztatu proces wysyła wiadomość zwolnienia (RELEASE_TICKET) do wszystkich, którzy czekają na jego odpowiedź.


Gdy uczestnik zakończy wszystkie swoje warsztaty (workshop_count[rank] > number_of_workshops_per_participant), zmienia swój stan na finishedWorkshops.
Uczestnik wysyła wiadomość FINISH do wszystkich innych uczestników, informując ich o zakończeniu swojego uczestnictwa.
Odbiór powiadomień o zakończeniu:

Gdy uczestnik otrzymuje wiadomość FINISH od innych uczestników, zwiększa licznik finished[rank].
Uczestnik czeka, aż licznik finished[rank] osiągnie wartość równą liczbie uczestników minus jeden (number_of_participants - 1), co oznacza, że wszyscy pozostali uczestnicy zakończyli swoje warsztaty.

Po zresetowaniu zmiennych, uczestnik zmienia swój stan na beginPyrkon i rozpoczyna nową rundę Pyrkonu od początku, wykonując te same kroki co na początku (mainLoop).
Proces losowo wybiera nowe warsztaty, ustawia nowe żądania i zaczyna cały cykl od nowa.
