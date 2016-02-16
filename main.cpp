//dane sa pobierane z pliku network.txt, zapisywane do wynik.txt

#include <iostream>
#include<fstream>
#include<string>
#include<stdlib.h>

using namespace std;

class kraw{
    public:
        int w1, w2, waga;
        kraw* next;
};

class wierzcholek{
    public:
        int indeks_zbioru;
        int moc_zbioru;
};

//-------------------------------funckje:-----------------------------------------------------------------
int wczytaj(ifstream *graf, int * wierzch, int ** tab);
void wyswietl_kol(kraw*);
void do_kolejki(kraw ** head, kraw ** tail,int i, int j, int **tab, int * total_weight);
kraw* z_kolejki(kraw **head,kraw  **tail);
void tworz_tab_w(wierzcholek * tab_wierzcholkow, int wierzch);
void spr_krawedz(wierzcholek * tab_wierzcholkow, kraw *biezaca, kraw * tree, int * galezie, int * waga_mdr, int wierzch);
void stworz_tab_mdr(int wierzch, int galezie, int** tab, kraw** tree);
void wypisz_mdr(int wierzch, int ** tab);
void zapisz(ofstream * wynik, int wierzch, int ** tab);
//--------------------------------------------------------------------------------------------------------
int main()
{
    ifstream graf("network.txt");
    int wierzch;
    int** tab = new int*[wierzch];                  //tworze tablice dynamiczna na dane
    wczytaj(&graf, &wierzch, tab);                  //wczytuje dane z pliku do tablicy
    graf.close();
    /*
    for (int i=0; i<wierzch;i++){                           //wypisz tablice pobrana z pliku
        cout << i+1<< ")" << "\t";
        for (int j=0;j<wierzch;j++)
            cout << tab[i][j] << " ";
        cout << endl << endl;
    }
*/

    int total_weight = 0;                           //waga wszystkich krawedzi, ale pomnozona przez 2, bo kazda krawedz 2 razy w tablicy
                                                    //tworze kolejke
    kraw* head = NULL;
    kraw* tail = NULL;
    //int m=0;
                                                //int a=0,b=0,c=0;
    for(int i=0; i<wierzch; i++)
        for (int j=0; j<wierzch; j++)
            if(tab[i][j]){
                //++m;            //licznik krawedzi
                do_kolejki(&head, &tail, i, j, tab, &total_weight);
            }
    //wyswietl_kol(head);
    //system("pause");                                                //wyswietla kolejke;
    //cout << "liczba krawedzi: " << m/2 << endl;

    wierzcholek *tab_wierzcholkow = new wierzcholek[wierzch];                   //tworze tablice wierzcholkow
    tworz_tab_w(tab_wierzcholkow, wierzch);

    kraw *tree = new kraw [wierzch];                                            //tablica krawedzi rozpinajacych
    int galezie = 0;                                                            //licznik galezi dodanych do drzewa rozpinajacego
    int waga_mdr = 0;                                                           //licznik wag MDR

    while(galezie<39){                                                                  //l.krawedzi min. drzewa rozp. = n-1
        kraw *biezaca = z_kolejki(&head, &tail);                                        //pobierz nastepna krawedz o najmniejszej wadze
        spr_krawedz(tab_wierzcholkow, biezaca, tree, &galezie, &waga_mdr, wierzch);      //jesli spelnia warunki, dodaj ja do drzewa i wykonaj czynnosci pomocnicze
    }

/*    for (int i=0;i<wierzch;i++)
        cout << i << "\t" << tab_wierzcholkow[i].indeks_zbioru << "\t" << tab_wierzcholkow[i].moc_zbioru << endl;
    system("pause");
*/
/*  //wypisuje krawedzie tworzace MDR
    for (int i=0;i<galezie;i++)
        cout << i+1 << "\t" << tree[i].w1 << "\t" << tree[i].w2 << "\t" << tree[i].waga << endl;
*/

    stworz_tab_mdr(wierzch, galezie, tab, &tree);               //tworze i wypelniam tablice MDR
    cout << "MDR:"<< endl;
    wypisz_mdr(wierzch, tab);                                        //wypisz MDR na ekran

    cout << "\nliczba wierzcholkow: " << wierzch << endl;
    cout << "Waga wszystkich krawedzi grafu: " << total_weight/2 << endl;
    cout << "Krawedzie MDR: " << galezie << endl;
    cout << "Waga minimalnego drzewa rozpinajacego: " << waga_mdr << endl << endl;
    cout << "Oszczednosc wag: " << total_weight/2-waga_mdr << endl;


    ofstream wynik("wynik.txt");
    zapisz(&wynik, wierzch, tab);                               //zapisz MDR do pliku
    wynik.close();

    for(int i=0; i<wierzch;i++)
        delete tab[i];
    delete tab;
    delete tab_wierzcholkow;
    delete tree;

    return total_weight/2-waga_mdr;
}




//------------------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------FUNKCJE-------------------------------------------------------------------------------------
//wczytuje informacje z pliku do tablicy dwuwymiarowej
int wczytaj(ifstream *graf, int * wierzch, int ** tab){
    if (*graf==0){
        cout << "nie udalo sie otworzyc pliku";
        return 1;
    }

    *graf >> *wierzch;
    for(int i=0; i<*wierzch;i++)
        tab[i] = new int[*wierzch];
    string tmp;                                     //pomocniczy string

    for (int i=0; i<*wierzch;i++)
        for (int j=0;j<*wierzch;j++){
            if(j == *wierzch-1){                     //po wczytaniu przedostatniego elementu w linii
                if(getline(*graf,tmp)){              // pobierz ostatni i skasuj enter
                    if (tmp[0] == '-') tab[i][j]=0;     //jesli myslnik, to wstaw 0
                    else tab[i][j] = atoi(tmp.c_str()); //jesli nie myslnik to interpretuj jako liczbe
                };
            }
            else if(getline(*graf, tmp, ',')){       //w pozostalych przypadkach jako separator traktuj przecinek
                if (tmp[0] == '-') tab[i][j]=0;
                else tab[i][j] = atoi(tmp.c_str());
            }
        }
        return 0;
}

//------------------------------------------------------------------
//funkcja dodaje do kolejki kolejne elementy z pliku
void do_kolejki(kraw ** head, kraw ** tail,int i, int j, int **tab, int * total_weight){      //, int *a, int *b, int *c
    kraw* nowa = new kraw;
    nowa -> w1 = i;
    nowa -> w2 = j;
    nowa -> waga = tab[i][j];

    if (!(*head)){                      //jesli kolejka pusta
        *head = nowa;
        *tail = nowa;
        nowa->next=NULL;
    }
    else if (nowa->waga <= (*head)->waga){        //jesli nowa krawedz mniejsza od najmniejszego el kolejki(czyli head)
            nowa->next = (*head);
            *head=nowa;
        }
        else{                               //jesli nowa krawedz wieksza od head
            kraw* wiekszy = *head;           //nastepna krawedz po wiekszy bedzie miala wieksza wage niz waga nowej
            while (wiekszy->next && nowa->waga > wiekszy->next->waga)
                wiekszy=wiekszy->next;      //znajduje pierwsza krawedz o wiekszej wadze
            nowa->next = wiekszy->next;
            wiekszy->next = nowa;
            if (nowa->next==NULL) *tail=nowa;
    }
    *total_weight += nowa -> waga;
    nowa=NULL;
    delete nowa;
}
//------------------------------------------------------------------
//wyswietla kolejke
void wyswietl_kol(kraw *head){
    int i=1;
    while (head){
        cout << i++ << ") w1:\t" << head->w1 << ", w2:\t" << head->w2 << ", waga:\t" << head->waga << endl;
        head = head->next;
    }
}
//------------------------------------------------------------------
//funkcja usuwa z kolejki pierwsza krawedz (o najmniejszej wadze) i przekazuje do niej wskaznik do main
kraw* z_kolejki(kraw **head,kraw  **tail){
    kraw* biezaca = *head;
    if (*head){
        *head = (*head)->next;
        return biezaca;
    } else return NULL;
    if (*head == *tail){
        *head = NULL;
        *tail = NULL;
        return biezaca;
    }
}
//------------------------------------------------------------------
//sprawdza biezaca krawedz i jesli nie laczy ona wierzcholkow w tym samym zbiorze, dodaje ja do MDR
void spr_krawedz(wierzcholek * tab_wierzcholkow, kraw *biezaca, kraw * tree, int * galezie, int * waga_mdr, int wierzch){
    wierzcholek* jeden = tab_wierzcholkow+(biezaca->w1);
    wierzcholek* dwa = tab_wierzcholkow+(biezaca->w2);

    if(jeden->indeks_zbioru != dwa->indeks_zbioru){                     //jesli zbiory, ktore laczy dana krawedz sa rozlaczne
        if(jeden->moc_zbioru >= dwa->moc_zbioru){                       //do wiekszego zbioru dolacz mniejszy
            jeden->moc_zbioru += dwa->moc_zbioru;                       //zwiekszam moc 1. zbioru o moc zbioru 2.
            int tmp = dwa->indeks_zbioru;                               //zm. pomocnicza przechowujaca numer indeksu mniejszego zbioru
            for (int i=0; i<wierzch; i++)
                if(tab_wierzcholkow[i].indeks_zbioru == tmp){           //wyszukuje elementy mniejszego zbioru i zmieniam ich indeksy i moc na takie, jakie ma wiekszy zbior
                    tab_wierzcholkow[i].indeks_zbioru = jeden->indeks_zbioru;
                    tab_wierzcholkow[i].moc_zbioru = jeden->moc_zbioru;
                }
        } else{                                                        //analogicznie, gdy wierzcholek jeden nalezy do zbioru o mniejszej mocy niz wierzcholek dwa
            dwa->moc_zbioru += jeden->moc_zbioru;
            int tmp = jeden->indeks_zbioru;
            for (int i=0; i<wierzch; i++)
                if(tab_wierzcholkow[i].indeks_zbioru == tmp){
                    tab_wierzcholkow[i].indeks_zbioru = dwa->indeks_zbioru;
                    tab_wierzcholkow[i].moc_zbioru = dwa->moc_zbioru;
                }
        }
        tree[(*galezie)++] = *biezaca;                                  //uzupelniam MDR
        *waga_mdr += biezaca->waga;                                     //licze wage MDR
    }
}

//------------------------------------------------------------------
//towrzy tablice wierzcholkow i nadaje wartosci poczatkowe
void tworz_tab_w(wierzcholek * tab_wierzcholkow, int wierzch){
    for(int i=0; i<wierzch; i++){                           //indeksy w tablicy oznaczaja nry wierzcholkow,
        tab_wierzcholkow[i].indeks_zbioru=i;                // indeks zbioru rozlacznego
        tab_wierzcholkow[i].moc_zbioru=1;                   //liczba elementow zbioru
    };
}

//------------------------------------------------------------------
//tworzy tablice Min drzewa rozpinajacego, zeruje i inicjalizuje
void stworz_tab_mdr(int wierzch, int galezie, int** tab, kraw** tree){
    for (int i=0; i<wierzch; i++)
        for (int j=0; j<wierzch;j++)
            tab[i][j]=0;
    for (int i=0; i<galezie; i++){
        tab[(*tree)[i].w1][(*tree)[i].w2] = (*tree)[i].waga;
        tab[(*tree)[i].w2][(*tree)[i].w1] = (*tree)[i].waga;
    }
}
//------------------------------------------------------------------
//wypisuje mdr na ekran
void wypisz_mdr(int wierzch, int ** tab){
    for (int i=0; i<wierzch; i++){
        for (int j=0; j<wierzch;j++){
            if (tab[i][j] == 0) cout << '-' << ",";
            else cout << tab[i][j] << ",";
        }
        cout << endl;
     }
}
//------------------------------------------------------------------
//zapisuje tablice MDR do pliku
void zapisz(ofstream * wynik, int wierzch, int ** tab){
    *wynik << wierzch << endl;
    for (int i=0; i<wierzch; i++){
        for (int j=0; j<wierzch;j++){
            if (tab[i][j] == 0) *wynik << '-' << ",";
            else *wynik << tab[i][j] << ",";
        }
        *wynik << endl;
    }
}
