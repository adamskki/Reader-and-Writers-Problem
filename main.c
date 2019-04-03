#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <time.h>
#include <elf.h>
#include <sys/msg.h>
#include <semaphore.h>
#include <semaphore.h>


//Definiujemy ile procesow ma dzialac wspolbieznie oraz jaka jest maksymalna liczba ksiazek jaka moze byc w czytelni
#define PROCESSES 10
#define BOOKS 5


static struct sembuf buf;


//Definicja ksiazki ktora bedzie wysylana do kolejki komunikatow
struct book_structure
{
    long book_id; // identyfikator ksiazki, bedzie to liczba wieksza od 0, tak zeby kazdy proces mogl wybrac konkretna ksiazka jaka chce pobrac przez message receive
    int number_of_readers; //liczba czytelnikow jacy maja przeczytac dana ksiazke, jesli jest 0, to ksiazka jest usuwana z kolejki komunikatow
};

//Podnoszenie i opuszczanie semaforow, pierwszy argument to id tablicy semaforow, drugi to indeks w tablicy semaforow, a trzeci to wartosc o jaka chcemy podniesc albo opuscic dany semafor
void up(int semid, int semnum,int value_to_up) {
    buf.sem_num = semnum;
    buf.sem_op = value_to_up;
    buf.sem_flg = 0;
    if (semop(semid, &buf, 1) == -1) {
        perror("Blad podnoszenia semafora");
        exit(1);
    }
}

void down(int semid, int semnum, int value_to_down) {
    buf.sem_num = semnum;
    buf.sem_op = -value_to_down;
    buf.sem_flg = 0;
    if (semop(semid, &buf, 1) == -1) {
        perror("Blad opuszczania semafora");
        exit(1);
    }
}


void writer(int id_person,int semid_writers_readers, int semid_writers, int role_array[], int book_array[][BOOKS+1], int msgid_book, struct book_structure book, int id_book_array[], int semid_books_to_read, int semid_library_size)
{

    struct book_structure book2; //Struktura ksiazki, ktora potrzebujemy aby odebrac jako pisarz ksiazke jesli mamy cos do przeczytania
    int how_many_readers = 0; //jako pisarz musimy wiedziec ilu jest czytelnikow w danej chwili, po to aby wybrac zaznaczyc im ze maja ksiazke do przeczytania (czyli ustawic w komunikacie liczba czytelnikow)
    int id_book_to_read;
    int temp_condition = 0; //zmienna pomocnicza, ktora ma zadanie sprawdzic czy dany pisarz ma cos do przeczytania, jesli nie, to probuje cos napisac
    down(semid_library_size,0,1); //opuszczamy semafor, ktory kontroluje ile miejsca na ksiazki mamy w bibliotece, jesli nie ma miejsca, to czekamy, az czytelnicy cos przeczytaja
    down(semid_writers,0,1); //opuszczamy semafor dla pisarzy, czyli blokujemy aby tylko jeden pisarz naraz byl w czytelni
    down(semid_writers_readers,0,1); //opuszczamy semafor do wykluczenia jednoczesnego dostepu do czytelni przez pisarzy i czytelnikow
    printf("Jestem pisarzem nr:%d i jestem w czytelni! Aktualny stan ksiazek: \n", id_person);

    //Sprawdzamy czy jako pisarz mamy co przeczytac, jesli ma to zmienna temp_condition = 1
    for(int i=1;i<BOOKS+1;i++)
    {
        if(book_array[id_person][i]==1)
        {
            id_book_to_read=i;
            temp_condition = 1;
            break;
        }
    }
    //
    if(temp_condition == 1) // pisarz czyta ksiazke
    {
        msgrcv(msgid_book, &book2, sizeof(book2.number_of_readers),id_book_to_read,IPC_NOWAIT);
        book_array[id_person][id_book_to_read] = 0;
        book2.number_of_readers--;
        if(book2.number_of_readers == 0)
        {
            printf("Pisarz nr %d jako ostatni przeczytal ksiazke nr %ld zastem jest ona usuwana z biblioteki \n",id_person,book2.book_id);
            id_book_array[id_book_to_read] = 0; //zaznaczamy ze dany numer ksiazki jest wolny
            up(semid_library_size,0,1); //zaznaczamy ze zwolnilo sie jedno miejsce na ksiazke
        } else
        {
            printf("Pisarz nr %d przeczytal ksiazke nr %lu i odklada ja w czytelni \n", id_person, book2.book_id);
            msgsnd(msgid_book, &book2, sizeof(book2.number_of_readers), IPC_NOWAIT);
        }

        down(semid_books_to_read,id_person,1); //dana osoba ma o jedna ksiazke mniej do przeczytania
    }
    //

    //Sprawdzamy ktore id ksiazki jest zajete ktore wolne, wybieramy pierwsze wolne jakie jest dostepne, przypisujemy to jako id do naszego structa, a w tablicy dajemy 1 czyli zaznaczamy ze dane id jest zajete
    for(int i=1;i<BOOKS+1;i++)
    {
        if(id_book_array[i]==0)
        {
            book.book_id = i;
            id_book_array[i] = 1;
            break;
        }
    }
    for(int i=0;i<PROCESSES;i++) {
            if (role_array[i] == 0) {
                how_many_readers++;
                book_array[i][book.book_id] = 1;
                up(semid_books_to_read,i,1);
            }
        }


    book.number_of_readers = how_many_readers; //tyle osob musi przeczytac dana ksiazke aby byla ona usunieta z kolejki komunikatow

    msgsnd(msgid_book, &book, sizeof(book.number_of_readers), IPC_NOWAIT); //Dodajemy ksiazke do czytelni czyli wysylamy ją do kolejki komunikatow

    //Wypisywanie informacji
    printf("K:");
    for(int i=1;i<BOOKS+1;i++)
    {
        printf(" %d", i);
    }
    printf("\n");
    for(int i=0;i<PROCESSES;i++)
    {
        printf("P%d",i);
        for(int j=1;j<BOOKS+1;j++)
        {
            printf(" %d",*(*(book_array+i)+j) );
        }
        printf("\n");
    }
    up(semid_writers_readers,0,1);
    up(semid_writers,0,1);
}

void reader(int id_person,int semid_readers_count, int readers_count[],int semid_writers_readers, int book_array[][BOOKS+1], int msgid_book, int id_book_array[], int semid_books_to_read, int semid_book_reservation, int semid_library_size)
{
    int id_book_to_read;
    struct book_structure book;
    down(semid_books_to_read,id_person,1); //sprawdzamy czy dany czytelnik ma co czytac, jesli tak to wchodzi do czytelni, jesli nie to czeka az jakis pisarz wyda ksiazke i podniesie mu ta wartosc na semaforze
    down(semid_readers_count,0,1); //poniewaz liczba czytelnikow jest w segmencie pamieci wspoldzielonej musimy opuscic semafor dla innych czytelnikow jesli chcemy zmienic jej wartosc
//    printf("Liczba czytelnikow: %d", readers_count[0]);
    if(readers_count[0]==0)
    {
        readers_count[0]++;
        down(semid_writers_readers,0,1); //pierwszy pisarz opuszcza semafor dla pisarzy, dzieki czemu teraz czytelnicy maja jako jedyni dostep czy czytelni
    } else
    {
        readers_count[0]++;
    }
    up(semid_readers_count,0,1);
    for(int i=1;i<BOOKS+1;i++) //wybieramy ksiazke jaka mamy przeczytac
    {
        if(book_array[id_person][i]==1)
        {
            down(semid_book_reservation,i,1); //semafor ktory zabezpiecza zeby w tej samej czytelnicy nie chcieli wyjac tej samej ksiazki przypadkiem z kolejki komunikatow
            id_book_to_read=i;
            break;
        }
    }
    //Odebranie komunikatu czyli czytanie ksiazki, zmieniamy wartosc liczby czytelnikow w komunikacie (ksiazce) o jeden, co oznacza ze liczba czytelnikow ktorzy maja jeszcze ja przeczytac zostala zmniejszona o jeden
    msgrcv(msgid_book, &book, sizeof(book.number_of_readers),id_book_to_read,IPC_NOWAIT);
    book_array[id_person][id_book_to_read] = 0; //zaznaczamy ze ksiazka jest juz przez nas przeczytana
    book.number_of_readers--; //zmniejszamy liczbe czytelnikow ktorzy musza jeszcze przeczytac ta ksiazke
    if(book.number_of_readers == 0)
    {
        printf("Czytelnik nr %d jako ostatni przeczytal ksiazke nr %ld zatem jest ona usuwana z biblioteki \n",id_person,book.book_id);
        id_book_array[id_book_to_read] = 0;
        up(semid_library_size,0,1);
    } else
    {
        printf("Czytelnik nr %d przeczytal ksiazke nr %lu i odklada ja w czytelni \n", id_person, book.book_id);
        msgsnd(msgid_book, &book, sizeof(book.number_of_readers), IPC_NOWAIT); //jesli jacys czytelnicy musza przeczytac jeszcze ta ksiazke, to wysylamy ja z powrotem do kolejki komunikatow, jej id (typ) pozostaje taki sam, zmienia sie natomiast number_of_readers
    }
    up(semid_book_reservation,id_book_to_read,1);

    down(semid_readers_count, 0 ,1);
    readers_count[0]--;
    if(readers_count[0]==0) //jesli nie ma juz czytelnikow to pozwalamy wejsc pisarzom
    {
        up(semid_writers_readers,0,1);
    }
    up(semid_readers_count,0,1);
}

void Person(int shmid_role, int shmid_readers_count, int id_person, int semid_role, int semid_readers_count, int semid_writers_readers, int semid_writers, int shmid_book, int shmid_id_book, int msgid_book, int semid_books_to_read, int semid_book_reservation, int semid_library_size)
{
    int choose_role;
    int ile_czeka;
    struct book_structure book;

    //Przylaczenie pamieci wspoldzielonej trzymajacej informacje o roli danych procesow
    int *role_array;
    role_array = (int*)shmat(shmid_role,NULL,0);
    if(role_array == 0)
    {
        perror("Blad przylaczenia pamieci wspodzielonej przechowujacej inforamcje o rolach");
        exit(1);
    }
    //

    //Przylaczenie pamieci wspodzielonej trzymajacej informacje o liczbie czytelnikow
    int *readers_count;
    readers_count = (int*)shmat(shmid_readers_count,NULL,0);
    if(readers_count == 0)
    {
        perror("Blad przylaczenia pamieci wspoldzielonej mowiacej o liczbie czytelnikow");
    }
    //

    //Dodanie pamieci wspoldzielonej odpowiadajacej za to jaki proces musi ktora ksiazke przeczytac
    int (*book_array)[BOOKS+1];
    book_array = shmat(shmid_book, NULL, 0);
    if(book_array == 0)
    {
        perror("Blad przylaczenia pamieci wspodzielonej 2D przechowujacej informacje jaki czytelnik ma jaka ksiazke do przeczytania");
    }
    //

    //Dodanie pamieci wspoldzielonej trzymajacej informacje o tym, jakie id ksiazki jest zajete a jakie wolne
    int *id_book_array;
    id_book_array= (int*)shmat(shmid_id_book,NULL,0);
    if(id_book_array == 0)
    {
        perror("Blad przylaczenia pamieci wspoldzielonej mowiacej o tym jakie id ksiazki jest wolne a jakie zajete");
    }
    //

    //Stan poczatkowy tablicy mowiacej jaki proces ma ile ksiazek do przeczytania
    if(id_person == 0)
    {
        printf("K:");
        for(int i=1;i<BOOKS+1;i++)
        {
            printf(" %d", i);
        }
        printf("\n");
        for(int i=0;i<PROCESSES;i++)
        {
            printf("P%d",i);
            for(int j=1;j<BOOKS+1;j++)
            {
                printf(" %d",*(*(book_array+i)+j) );
            }
            printf("\n");
        }
    }
    //



    //Zabezpieczenie, aby srandtime losowal rozne wartosci nawet po uruchomieniu wielu watkow przez fork()
    srand(time(NULL)^ (getpid() << 16)); // << sluzy do przesuniecia bitow o 16 miejsc, czyli pomnozeniu wartosci, uzywane jest w celu uniknieciu wylosowania tej samej wartosci (np samych 0) przez wszystkie procesy w przypadku uzycia srand(time)

    while(1)
    {
        ile_czeka = semctl(semid_library_size, 0,GETVAL);
        if(ile_czeka == 0) //jesli cala biblioteka jest zapelniona to nowe procesy beda stawaly sie czytelnikami w celu unikniecia zakleszczenia
        {
            choose_role = 0;
        } else
            choose_role = rand()%2;

        //Opuszczemy semafor jesli chcemy zmienic swoje role, czyli sprawdzamy w danej chwili przypadkiem nie dziala manager
        down(semid_role,0,1);
        role_array[id_person] = choose_role;
        up(semid_role,0,1);
        sleep(5);

        if(choose_role == 0) //Praces staje sie czytelnikiem
        {
            reader(id_person, semid_readers_count,readers_count, semid_writers_readers,book_array, msgid_book ,id_book_array,semid_books_to_read, semid_book_reservation, semid_library_size);
        }

        else if(choose_role == 1) //Proces staje sie pisarzem
        {
            writer(id_person, semid_writers_readers, semid_writers, role_array,book_array, msgid_book, book ,id_book_array,semid_books_to_read, semid_library_size);
        }

    }
}


int main() {
    printf("Glowny proces \n");
    
    printf("Wybrana liczba procesow to:%d \n", PROCESSES);
    printf("Maksymalna liczba ksiazek to:%d \n", BOOKS);


    //UWAGA NA TO NIE WIADOMO CZY POTRZEBNE
    // Tworzenie pamieci wspodzielonej, przechowujacej informacje o liczbie czytelnikow i pisarzy
    int shmid_role;
    shmid_role = shmget(IPC_PRIVATE, PROCESSES*sizeof(int), IPC_CREAT|0600);
    if(shmid_role == -1)
    {
        perror("Blad utworzenia pamieci wspodzielonej przechowujacej informacje o roli danego procesu");
        exit(1);
    }
    //

    //Pamiec wspodzielona przechowujaca informacje o liczbie czytelnikow, ktorzy sa w czytelni
    int shmid_readers_count;
    shmid_readers_count = shmget(IPC_PRIVATE,1*sizeof(int),IPC_CREAT|0600);
    if(shmid_readers_count == -1)
    {
        perror("Blad tworzenia pamieci wspoldzielonej przechowujacej informacje o liczbie czytelnikow");
    }
    //

    //Pamieci wspodzielona mowiaca, ktory czytelnik ma jaka ksiazke do przeczytania, pisarz uzywa jej aby zaznaczyc ze dany czytelnik musi ja przeczytac
    int shmid_book;
    shmid_book = shmget(IPC_PRIVATE, sizeof(int[PROCESSES][BOOKS+1]), IPC_CREAT|0600);
    if(shmid_book == -1)
    {
        perror("Blad tworzenia pamieci wspoldzielonej przechowujacej informacje jaki czytelnik ma przeczytac jaka ksiazke");
    }
    //

    //Pamiec wspodzielona przechowujaca informacje o tym, jakie id ksiazki jest wolne (jesli w tablicy bedzie 0 to znaczy ze id jest wolne, a jesli 1 to znaczy ze zajete)
    int shmid_id_book;
    shmid_id_book= shmget(IPC_PRIVATE,(BOOKS+1)* sizeof(int), IPC_CREAT|0600);
    if(shmid_id_book == -1)
    {
        perror("Blad tworzenia pamieci wspoldzielonej mowiacej o tym jakie id ksiazki jest wolne a jakie zajete");
    }
    //



    //Semafor ktory blokuje procesy w momencie ktorym manager chce sprawdzic liczbe czytelnikow i pisarzy
    int semid_role;
    semid_role = semget(IPC_PRIVATE, 1, IPC_CREAT | 0600);
    if(semid_role == -1)
    {
        perror("Blad semafora do kontrolowania procesu managera");
    }
    if(semctl(semid_role,0,SETVAL,PROCESSES) == -1)
    {
        perror("Blad nadania wartosci semaforowi do kontroli procesow");
    }
    //

    //Semafor ktory kontroluje zwiekszanie liczby czytelnikow
    int semid_readers_count;
    semid_readers_count = semget(IPC_PRIVATE, 1, IPC_CREAT | 0600);
    if(semid_readers_count == -1)
    {
        perror("Blad semafora do kontrolowania liczby czytelnikow");
    }
    if(semctl(semid_readers_count,0,SETVAL,1) == -1)
    {
        perror("Blad nadania wartosci semaforowi do kontrolowania liczby czytelnikow");
    }
    //

    //Semafor ktory kontroluje dostep do sekcji krytycznej pomiedzy czytelnikami a pisarzem
    int semid_writers_readers;
    semid_writers_readers= semget(IPC_PRIVATE, 1, IPC_CREAT | 0600);
    if(semid_writers_readers == -1)
    {
        perror("Blad tworzenia semafora do wykluczania jednoczesnego dostepu przez czytelnikow i pisarzy do czytelni");
    }
    if(semctl(semid_writers_readers,0,SETVAL,1) == -1)
    {
        perror("Blad nadania wartosci semaforowi do wykluczania jednoczesnego dostepu przez czytelnikow i pisarzy do czytelni");
    }
    //

    //Semafor ktory wyklucza dostep do czytelni pomiedzy pisarzami (tylko jeden pisarz moze byc w czytelni)
    int semid_writers;
    semid_writers= semget(IPC_PRIVATE, 1, IPC_CREAT | 0600);
    if(semid_writers== -1)
    {
        perror("Blad tworzenia semafora do wykluczania jednoczesnego dostepu przez pisarzy do czytelni");
    }
    if(semctl(semid_writers,0,SETVAL,1) == -1)
    {
        perror("Blad nadania wartosci semaforowi do wykluczania jednoczesnego dostepu przez pisarzy do czytelni");
    }
    //

    //Tablica semaforow ktora odpowiada za sprawdzenie czy dany czytelnik ma jakakolwiek ksiazke do przeczytania, uzyte jest w celu unikniecia aktywnego czekania
    int semid_books_to_read;
    semid_books_to_read= semget(IPC_PRIVATE, PROCESSES, IPC_CREAT | 0600);
    if(semid_books_to_read== -1)
    {
        perror("Blad tworzenia semafora do przechowywania liczby ksiazek do przeczytania przez poszczegolnego czytelnika");
    }
    for(int i=0;i<PROCESSES;i++)
    {
        if(semctl(semid_books_to_read,i,SETVAL,0) == -1)
        {
            perror("Blad nadania wartosci semaforowi mowiacemu ile ksiazek ma dany pisarz do przeczytania");
        }
    }
    //


    //Tablica semaforow o wielkosci liczby ksiazek ktory sluzy zapobieganiu sytuacji w ktorej dwa procesy probuja przeczytac w tej samej chwili ta sama ksiazke
    int semid_book_reservation;
    semid_book_reservation= semget(IPC_PRIVATE, BOOKS+1, IPC_CREAT | 0600);
    if(semid_book_reservation== -1)
    {
        perror("Blad tworzenia semafora ktory ma zapobiec czytaniu tej samej ksiazki przez dwa procesy naraz");
    }
    for(int i=0;i<BOOKS+1;i++)
    {
        if(semctl(semid_book_reservation,i,SETVAL,1) == -1)
        {
            perror("Blad nadania wartosci tablicy semaforow ktora ma zapobiec czytaniu tej samej ksiazki przez dwa procesy");
        }
    }
    //

    //Semafor ktory ma za zadanie kontrole czy pisarz ma miejsce w bibliotece aby wypuscic swoja nowa ksiazke
    int semid_library_size;
    semid_library_size= semget(IPC_PRIVATE,1, IPC_CREAT | 0600);
    if(semid_library_size== -1)
    {
        perror("Blad tworzenia semafora ktory ma zadanie sprawdzanie czy jest miejsce w bibliotece na ksiazke");
    }
    if(semctl(semid_library_size,0,SETVAL,(int)BOOKS) == -1)
    {
        perror("Blad nadania wartosci semaforowi ktory ma zadanie sprawdzic czy jest miejsce w bibliotece na ksiazke");
    }
    //



    //Tworzenie kolejki komunikatow trzymajacej ksiazki wydawane przez pisarzy
    int msgid_book;
    msgid_book = msgget(IPC_PRIVATE, IPC_CREAT|IPC_EXCL|0600);
    if(msgid_book==-1)
    {
        printf("Kolejka komunikatow z ksiazkami juz istnieje\n");
        msgid_book = msgget(IPC_PRIVATE, IPC_CREAT|0600);
        if(msgid_book == -1)
        {
            perror("Blad utowrzenia kolejki komunikatow przechowujacej ksiazki");
            exit(1);
        }
    }
    //



    //Dodajemy pamiec wspodzielona mowiaca tym jaki proces ma ktora ksiazke do przeczytania, nastepnie zerujemy ja, tak aby zaznaczyc ze na poczatku nikt nie ma co przeczytac
    int (*book_array)[BOOKS+1];
    book_array = shmat(shmid_book, NULL, 0);
    if(book_array == 0)
    {
        perror("Blad przylaczenia pamieci wspodzielonej 2D przechowujacej informacje jaki czytelnik ma jaka ksiazke do przeczytania");
    }

    for(int i=0;i<PROCESSES;i++)
    {
        for(int j=0;j<BOOKS+1;j++)
        {
            *(*(book_array+i)+j) = 0;
        }
    }
    //

    //Przylaczamy a nastepnie zerujemy tablice w segmencie pamieci wspoldzielonej przechowujacej informacje o tym jakie id ksiazki jest uzyte a jakie jest wolne
    int *id_book_array;
    id_book_array= (int*)shmat(shmid_id_book,NULL,0);
    if(id_book_array == 0)
    {
        perror("Blad przylaczenia pamieci wspoldzielonej mowiacej o tym jakie id ksiazki jest wolne a jakie zajete");
    }

    for(int i=0;i<BOOKS+1;i++)
        id_book_array[i] =0;
    //

    for(int i=0;i<PROCESSES;i++)
    {
        if(fork() == 0)
        {
            Person(shmid_role, shmid_readers_count, i, semid_role, semid_readers_count, semid_writers_readers, semid_writers, shmid_book, shmid_id_book, msgid_book, semid_books_to_read, semid_book_reservation, semid_library_size);
            //break;
        }
    }

    wait(0);
}