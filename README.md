# Reader-and-Writers-Problem
Solution for Readers and Writers problem:

Zadanie polega na realizacji problemu czytelników i pisarzy (opis wersji klasycznej w Wikipedii), przy czym:      
-jest ustalona liczba procesów — N   
-każdy proces działa naprzemiennie w dwóch fazach: fazie relaksu i fazie korzystania z czytelni  
-w dowolnym momencie fazy relaksu proces może (choć nie musi) zmienić swoją rolę       
z pisarza na czytelnika lub z czytelnika na pisarza;  
-przechodząc do fazy korzystania z czytelni proces musi uzyskać dostęp we właściwym dla swojej aktualnej roli trybie;  
-pisarz umieszcza efekt swojej pracy — swoje dzieło — w formie komunikatu w kolejce komunikatów, gdzie komunikat ten pozostaje do momentu, aż wszystkie procesy, które w momencie wydania dzieła były w roli czytelnika, nie odczytają go (po odczytaniu przez wszystkie wymagane procesy komunikat jest usuwany);  
-pojemność kolejki komunikatów — reprezentującej półkę z książkami — jest ograniczona, tzn. nie może ona przechowywać więcej niż K dzieł;  
podczas pobytu w czytelni proces (również pisarz) czyta co najwyżej jedno dzieło, po czym czytelnik opuszcza czytelnię, a pisarz czeka na miejsce w kolejce, żeby opublikować kolejne dzieło.  

Uruchomienie: 
Aby ustalić liczbę procesów wykonywanych współbieżnie i liczbę książek należy zmienić wartości na początku programu przy #DEFINE
