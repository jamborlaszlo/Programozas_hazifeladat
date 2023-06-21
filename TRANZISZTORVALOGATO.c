#include<stdio.h>
#include<stdlib.h>
#include<string.h>

/*tranzisztor struktúrája*/
typedef struct tranzisztor      
{
    char sorszam[6];
    char tipus[6];
    char gyarto[20];
    int beta;
}tranzisztor;

/*sorszám - béta pár struktúrája*/
typedef struct sorszam_es_beta  
{
    char sorszam[6];
    int beta;
}sorszam_es_beta;

/*raktarkeszlet.txt-t ilyen elemekből álló 
láncolt listába olvassuk be*/
typedef struct raktarkeszlet    
{
    tranzisztor t;
    struct raktarkeszlet *next;
}raktarkeszlet;

/*betak.txt-t ilyen elemekből álló 
láncolt listába olvassuk be*/
typedef struct betak            
{
    sorszam_es_beta b;
    struct betak *next;
}betak;

/*az összeválogatott párok listája ilyen elemekből áll*/
typedef struct parok          
{
    tranzisztor t1, t2;
    struct parok *next;
}parok;

int raktarkeszlet_beolvas(char *fajlnev, raktarkeszlet *head, raktarkeszlet *tail)
{
    char sorszam[6];
    char tipus[6];
    char gyarto[20];
    raktarkeszlet *p = head;
    FILE *raktar = fopen(fajlnev, "rt");    //a raktarkeszlet információt tartalmazó forrásfájl megnyitása
    if(fajlnev == NULL)     //sikertelen megnyitás esetén
        return 1;   //hibaüzenet
    /*adatok beolvasása a fájlból, és befűzése láncolt listába*/                        
    while(fscanf(raktar, "%s %s %s", sorszam, tipus, gyarto) == 3)  
    {
        p->next = malloc(sizeof(raktarkeszlet));
        strcpy(p->next->t.sorszam, sorszam);
        strcpy(p->next->t.tipus, tipus);
        strcpy(p->next->t.gyarto, gyarto);
        p->next->next = tail;
        p = p->next;
    }
    fclose(raktar);   //fájl bezárása
    return 0;            
}

int beta_beolvas(char *fajlnev, betak *head, betak *tail)  
{
    char sorszam[6]; char nev[50];
    int beta;
    betak *p = head;
    FILE *betakfajl = fopen(fajlnev, "rt");     //a bétákat tartalmazó forrásfájl megnyitása
    if(fajlnev == NULL)     //sikertelen fájlmegnyitás esetén
        return 1;   //hibaüzenet
    /*adatok beolvasása a fájlból, és befűzése láncolt listába*/
    while(fscanf(betakfajl, "%s %d %s", sorszam, &beta, nev) == 3)
    {
        p->next = malloc(sizeof(betak));
        strcpy(p->next->b.sorszam, sorszam);
        p->next->b.beta = beta;
        p->next->next = tail;
        p = p->next;
    }
    fclose(betakfajl);      //fájl bezárása
    return 0;
}

void beta_hozzarendel(raktarkeszlet *raktar_head, betak *betak_head)
{
    raktarkeszlet *p = raktar_head->next;
    betak *q = betak_head->next;
    for(p; p->next != NULL; p = p->next)    //raktárkészlet lista bejárása
    {
        for(q = betak_head->next; q->next != NULL; q = q->next)     //beták litájának bejárása
        {
            if(!strcmp(p->t.sorszam, q->b.sorszam))     //azonos sorszámú (ugyanazon) tranzisztor esetén
                p->t.beta = q->b.beta;
        }
    }
    
    q = betak_head;
    betak *r;
    /*béták listája által foglalt memória felszabadítása*/
    while(q != NULL)   
    {
        r = q->next;
        free(q);
        q = r;
    }
}

int szortiroz(int min_beta, int max_beta, raktarkeszlet *raktar_head, 
raktarkeszlet *osztaly_head, raktarkeszlet *osztaly_tail, char *tipus, char *gyarto)
{
    int n = 0;
    raktarkeszlet *p = raktar_head->next;
    raktarkeszlet *q = osztaly_head;
    for(p; p->next != NULL; p = p->next)        //raktárkészelet lista bejárása
    {
        if(!strcmp(p->t.tipus, tipus) && !strcmp(p->t.gyarto, gyarto))      /*amennyiben a tranzisztor típusa és gyártója is megegyezik 
                                                                            a függvényhíváskor megadottal*/
        {
            if(p->t.beta >= min_beta && p->t.beta <= max_beta)      //ha az adott tranzisztor bétája a megadott tartományba esik
            {
                /*a tranzisztor befűzése a függvényhíváskor megadoot listába*/
                q->next = malloc(sizeof(raktarkeszlet));
                q->next->t = p->t;
                q->next->next = osztaly_tail;
                q = q->next;
            }
            else
                /*azon tranzisztotok számának növelése, melyek megfelelő tipusúak és gyártmányúak, 
                azonban a bétájuk kívül esik a megadott tartományon*/
                n++;    
        }  
    }
    
    return n;
}

void swap(raktarkeszlet *head, raktarkeszlet *p,raktarkeszlet *min)
{
    if(p != min)        //ha a két pointer nem ugyanarra az elemre mutat 
    {
        raktarkeszlet *p_prev, *min_prev;

        if(p->next != min)  //általános eset
        {
            
            raktarkeszlet *p_next = p->next;
            for(p_prev = head; p_prev->next != p; p_prev = p_prev->next);   //p-t megelőző elem megkeresése
            for(min_prev = head->next; min_prev->next != min; min_prev = min_prev->next);   //min-t megelőző elem megkeresése
            /*az elemek cseréje*/
            p_prev->next = min; 
            min_prev->next = p;
            
            p->next = min->next;
            min->next = p_next;
        }
        else        //egymást követő elemek esete
        {
            for(p_prev = head; p_prev->next != p; p_prev = p_prev->next);   //p-t megelőző elem megkeresése
            /*az elemek cseréje*/
            p->next = min->next;
            min->next = p;
            p_prev->next = min;
        }
    }
}

void sorbarendez(raktarkeszlet *head)
{
    raktarkeszlet *p = head->next, *q, *min;
    raktarkeszlet *kovetkezo;
    /*rendezés közvetlen kiválasztással*/
    while(p->next != NULL && p->next->next != NULL)        
    {
        kovetkezo = p->next;       
        min = p;
        for(q = p->next; q->next != NULL; q = q->next)  //a legkisebb bétájú elem megkeresése az adott részhalmazban
            if(q->t.beta < min->t.beta)
                min = q;
        swap(head, p, min);     //az elemek cseréje
        p = kovetkezo;
    }
}

raktarkeszlet* kozepso_elem(raktarkeszlet *head)
{
    raktarkeszlet *p; int n, m;
    for(p = head, n = 0; p->next->next != NULL; p = p->next, n++);
    /*Páratlan elemszám esetén a középső eleme címét adjuk vissza.
    Páros elemszám esetén a két középső közül a lista végéhez közelebbit.*/
    for(p = head, m = 0; p != NULL && m <= n / 2; p = p->next, m++); 
    return p;
}

void listabol_torol(raktarkeszlet *head, raktarkeszlet *p)
{
    raktarkeszlet *p_prev; 
    for(p_prev = head; p_prev->next != p; p_prev = p_prev->next);   //elem előtti elem megkeresése
    /*elem törlése*/
    p_prev->next = p->next;
    free(p);
}

double elteres(raktarkeszlet *x, raktarkeszlet *y)
{
    int a = x->t.beta;
    int b = y->t.beta;
    if(a >= b)
        return (a - b) / (double)a;
    else if(b > a)
        return (b - a) / (double)b;
}

int parba_valogat(parok *parok_head, parok *parok_tail, raktarkeszlet *head, double max_elteres)
{
    int n = 0;
    raktarkeszlet *p;
    raktarkeszlet *q;
    parok *beszur = parok_head;
    while(head->next->next != NULL)
    {
        /*legtávolabbi, de a feltételnek még megfelelő elem keresése jobbra*/
        for(p = kozepso_elem(head); p ->next->next != NULL && elteres(p->next, kozepso_elem(head)) <= max_elteres; p = p->next);
        /*legtávolabbi, de a feltételnek még megfelelő elem keresése balra*/
        for(q = head->next; q != kozepso_elem(head) && elteres(q, kozepso_elem(head)) > max_elteres; q = q->next);   
        if(p == kozepso_elem(head) && q == kozepso_elem(head))  //az aktuális középső elemnek nem találtunk párt
        {   listabol_torol(head, kozepso_elem(head)); 
            n++;    //azon elemek száma, melyeknek nem találtunk párt
        }
        else if(p != kozepso_elem(head) && elteres(p, kozepso_elem(head)) >= elteres(q, kozepso_elem(head)))    //|p bétája - közápső elem bétája| > |q bétája - középső elem bétája| esetén
        {
            /*elempár elhelyezése a "párok" listában*/
            beszur->next = malloc(sizeof(parok));
            beszur->next->t1 = p->t; beszur->next->t2 = kozepso_elem(head)->t;
            beszur->next->next = parok_tail;
            beszur = beszur->next;
            /*elempár eltávolítása az eredeti listából*/
            listabol_torol(head, kozepso_elem(head));
            listabol_torol(head, p);
        }
        else if(q != kozepso_elem(head) && elteres(q, kozepso_elem(head)) >= elteres(p, kozepso_elem(head)))    //|q bétája - közápső elem bétája| > |p bétája - középső elem bétája| esetén
        {
            /*elempár elhelyezése a "párok" listában*/
            beszur->next = malloc(sizeof(parok));
            beszur->next->t1 = q->t; beszur->next->t2 = kozepso_elem(head)->t;
            beszur->next->next = parok_tail;
            beszur = beszur->next;
            /*elempár eltávolítása az eredeti listából*/
            listabol_torol(head, kozepso_elem(head));
            listabol_torol(head, q);
        }
    }
    /*eredeti - mostmár üres - lista strázsáinak felszámolása*/
    p = head;
    free(p->next);
    free(p);

    return n;
}

void listat_kiir(parok *head)
{
    printf("Sorszam   Tipus    Gyarto             Beta\n\n\n"); //fejléc kiírása
    head = head->next;
    /*stringek tartalmának modosítása a szebb megjelenítés érdekében*/
    while(head->next != NULL && head->next->next != NULL)
    {   for(int i = 0; i < 19; i++)
        {
            if(head->t1.gyarto[i] == '_')
                head->t1.gyarto[i] = ' ';
            if(head->t2.gyarto[i] == '_')
                head->t2.gyarto[i] = ' ';
            if(head->t1.gyarto[i] == '\0')
            {
                for(int j = i; j < 19; j++)
                    head->t1.gyarto[j] = ' ';
                head->t1.gyarto[19] = '\0';
            }
            if(head->t2.gyarto[i] == '\0')
            {
                for(int j = i; j < 19; j++)
                    head->t2.gyarto[j] = ' ';
                head->t2.gyarto[19] = '\0';
            }    
        }
        /*egy pár kiírása a standard outputra*/
        printf("%s       %s    %s%d\n", head->t1.sorszam, head->t1.tipus, head->t1.gyarto, head->t1.beta);
        printf("%s       %s    %s%d\n", head->t2.sorszam, head->t2.tipus, head->t2.gyarto, head->t2.beta);
        printf("------------------------------------------\n");
        head = head->next;
    }
    printf("\n\n");
}

void lista_felszamol(parok *head)
{
    /*a lista elemei által elfoglalt memória felszabadítása*/
    parok *q;
    while(head != NULL)
    {
        q = head->next;
        free(head);
        head = q;
    }
}

void vegrehajt(raktarkeszlet *raktar_head)
{
    /*az egyes tranzisztor-osztályok listáinak létrehozása*/
    parok *parok_BC182_ST_head = malloc(sizeof(parok));
    parok_BC182_ST_head->next = malloc(sizeof(parok));
    parok *parok_BC182_ST_tail =  parok_BC182_ST_head->next;
    parok_BC182_ST_tail->next = NULL;

    parok *parok_BC182_TI_head = malloc(sizeof(parok));
    parok_BC182_TI_head->next = malloc(sizeof(parok));
    parok *parok_BC182_TI_tail =  parok_BC182_TI_head->next;
    parok_BC182_TI_tail->next = NULL;

    parok *parok_BC546_ST_head = malloc(sizeof(parok));
    parok_BC546_ST_head->next = malloc(sizeof(parok));
    parok *parok_BC546_ST_tail =  parok_BC546_ST_head->next;
    parok_BC546_ST_tail->next = NULL;   

    parok *parok_BC546_TI_head = malloc(sizeof(parok));
    parok_BC546_TI_head->next = malloc(sizeof(parok));
    parok *parok_BC546_TI_tail =  parok_BC546_TI_head->next;
    parok_BC546_TI_tail->next = NULL; 

    parok *parok_BD139_ST_head = malloc(sizeof(parok));
    parok_BD139_ST_head->next = malloc(sizeof(parok));
    parok *parok_BD139_ST_tail =  parok_BD139_ST_head->next;
    parok_BD139_ST_tail->next = NULL;

    parok *parok_BD139_TI_head = malloc(sizeof(parok));
    parok_BD139_TI_head->next = malloc(sizeof(parok));
    parok *parok_BD139_TI_tail =  parok_BD139_TI_head->next;
    parok_BD139_TI_tail->next = NULL;

    parok *parok_BD249_ST_head = malloc(sizeof(parok));
    parok_BD249_ST_head->next = malloc(sizeof(parok));
    parok *parok_BD249_ST_tail =  parok_BD249_ST_head->next;
    parok_BD249_ST_tail->next = NULL;

    parok *parok_BD249_TI_head = malloc(sizeof(parok));
    parok_BD249_TI_head->next = malloc(sizeof(parok));
    parok *parok_BD249_TI_tail =  parok_BD249_TI_head->next;
    parok_BD249_TI_tail->next = NULL;

    /*kommunikáció a felhasználóval, adatok bekérése a standard input-ról*/
    printf("\n***TRANZISZTORVALOGATO***\n\n");
    printf("\nKompatibilis az alabbi tipusokkal: BC182, BC546, BD139, BD249\nKompatibilis az alabbi gyartokkal: ST, Texas Instruments\n\n");
    printf("Uj tranzisztor hozzaadasa: \"+\"\n");
    int be_nem_valogatott = 0;
    int BC182_ST=0, BC182_TI=0, BC546_ST=0, BC546_TI=0, BD139_ST=0, BD139_TI=0, BD249_ST=0, BD249_TI=0;
    char valasztas;
    scanf("%c", &valasztas);
    while(valasztas == '+')     //új tranzisztor hozzáadása esetén
    {
        char tipus[10]; char gyarto[20];
        int min_beta, max_beta; double max_elteres;
        printf("Tranzisztor tipusa: "); 
        scanf("%s", tipus); 
        while(getchar() != '\n');
        printf("Gyarto: "); fgets(gyarto, 20, stdin);
        printf("\nMegengedett legkisebb beta: "); scanf("%d", &min_beta);
        printf("Megengedett legnagyobb beta: "); scanf("%d", &max_beta);
        printf("\nMegengedett maximális elteres(%%): "); scanf("%lf", &max_elteres); max_elteres /= 100;

        /*feladatok végrehajtása adott típus - gyártó kombináció esetén*/
        if(!strcmp(tipus, "BC182"))
        {
            if(!strcmp(gyarto, "ST\n"))
            {   
                raktarkeszlet *head = malloc(sizeof(raktarkeszlet));           
                head->next = malloc(sizeof(raktarkeszlet));
                raktarkeszlet *tail = head->next;
                tail->next = NULL; 
                be_nem_valogatott += szortiroz(min_beta, max_beta, raktar_head, head, tail, "BC182", "ST");
                sorbarendez(head);
                be_nem_valogatott += parba_valogat(parok_BC182_ST_head, parok_BC182_ST_tail, head, max_elteres);
                BC182_ST = 1;
                
            }
            else if(!strcmp(gyarto, "Texas Instruments\n"))
            {
                raktarkeszlet *head = malloc(sizeof(raktarkeszlet));           
                head->next = malloc(sizeof(raktarkeszlet));
                raktarkeszlet *tail = head->next;
                tail->next = NULL; 
                be_nem_valogatott += szortiroz(min_beta, max_beta, raktar_head, head, tail, "BC182", "Texas_Instruments");
                sorbarendez(head);
                be_nem_valogatott += parba_valogat(parok_BC182_TI_head, parok_BC182_TI_tail, head, max_elteres);
                BC182_TI = 1;
            }
            else    //helytelen kitöltés esetén
            {
                printf("\n*HIBA!*\nToltse ki ujra!\n\n");
                continue;
            }
        }
        else if(!strcmp(tipus, "BC546"))
        {
            if(!strcmp(gyarto, "ST\n"))
            {
                raktarkeszlet *head = malloc(sizeof(raktarkeszlet));           
                head->next = malloc(sizeof(raktarkeszlet));
                raktarkeszlet *tail = head->next;
                tail->next = NULL; 
                be_nem_valogatott += szortiroz(min_beta, max_beta, raktar_head, head, tail, "BC546", "ST");
                sorbarendez(head);
                be_nem_valogatott += parba_valogat(parok_BC546_ST_head, parok_BC546_ST_tail, head, max_elteres);
                BC546_ST = 1;
            }
            else if(!strcmp(gyarto, "Texas Instruments\n"))
            {
                raktarkeszlet *head = malloc(sizeof(raktarkeszlet));           
                head->next = malloc(sizeof(raktarkeszlet));
                raktarkeszlet *tail = head->next;
                tail->next = NULL; 
                be_nem_valogatott += szortiroz(min_beta, max_beta, raktar_head, head, tail, "BC546", "Texas_Instruments");
                sorbarendez(head);
                be_nem_valogatott += parba_valogat(parok_BC546_TI_head, parok_BC546_TI_tail, head, max_elteres);
                BC546_TI = 1;
            }
            else    //helytelen kitöltés esetén
            {
                printf("\n*HIBA!*\nToltse ki ujra!\n\n");
                continue;
            }
        }
        else if(!strcmp(tipus, "BD139"))
        { 
            if(!strcmp(gyarto, "ST\n"))
            {
                raktarkeszlet *head = malloc(sizeof(raktarkeszlet));           
                head->next = malloc(sizeof(raktarkeszlet));
                raktarkeszlet *tail = head->next;
                tail->next = NULL; 
                be_nem_valogatott += szortiroz(min_beta, max_beta, raktar_head, head, tail, "BD139", "ST");
                sorbarendez(head);
                be_nem_valogatott += parba_valogat(parok_BD139_ST_head, parok_BD139_ST_tail, head, max_elteres);
                BD139_ST = 1;
            }
            else if(!strcmp(gyarto, "Texas Instruments\n"))
            {
                raktarkeszlet *head = malloc(sizeof(raktarkeszlet));           
                head->next = malloc(sizeof(raktarkeszlet));
                raktarkeszlet *tail = head->next;
                tail->next = NULL; 
                be_nem_valogatott += szortiroz(min_beta, max_beta, raktar_head, head, tail, "BD139", "Texas_Instruments");
                sorbarendez(head);
                be_nem_valogatott += parba_valogat(parok_BD139_TI_head, parok_BD139_TI_tail, head, max_elteres);
                BD139_TI = 1;
            }
            else    //helytelen kitöltés esetén
            {
                printf("\n*HIBA!*\nToltse ki ujra!\n\n");
                continue;
            }
        }
        else if(!strcmp(tipus, "BD249"))
        {    
            if(!strcmp(gyarto, "ST\n"))
            {
                raktarkeszlet *head = malloc(sizeof(raktarkeszlet));           
                head->next = malloc(sizeof(raktarkeszlet));
                raktarkeszlet *tail = head->next;
                tail->next = NULL; 
                be_nem_valogatott += szortiroz(min_beta, max_beta, raktar_head, head, tail, "BD249", "ST");
                sorbarendez(head);
                be_nem_valogatott += parba_valogat(parok_BD249_ST_head, parok_BD249_ST_tail, head, max_elteres);
                BD249_ST = 1;
            }
            else if(!strcmp(gyarto, "Texas Instruments\n"))
            {
                raktarkeszlet *head = malloc(sizeof(raktarkeszlet));           
                head->next = malloc(sizeof(raktarkeszlet));
                raktarkeszlet *tail = head->next;
                tail->next = NULL; 
                be_nem_valogatott += szortiroz(min_beta, max_beta, raktar_head, head, tail, "BD249", "Texas_Instruments");
                sorbarendez(head);
                be_nem_valogatott += parba_valogat(parok_BD249_TI_head, parok_BD249_TI_tail, head, max_elteres);
                BD249_TI = 1;
            }
            else    //helytelen kitöltés esetén
            {
                printf("\n*HIBA!*\nToltse ki ujra!\n\n");
                continue;
            }
        }
        else    //helytelen kitöltés esetén
        {
            printf("\n*HIBA!*\nToltse ki ujra!\n\n");
            continue;
        }

        printf("Uj tranzisztor hozzaadasa: \"+\"\n");
        printf("Eredmenyek kiirasa: \"*\"\n");
        scanf("\n%c", &valasztas);
    }
    
    if(valasztas == '*')
    {
        /*az beválogatott tranzisztor list(ák) kiírása*/
        if(BC182_ST) listat_kiir(parok_BC182_ST_head);
        if(BC182_TI) listat_kiir(parok_BC182_TI_head);
        if(BC546_ST) listat_kiir(parok_BC546_ST_head);
        if(BC546_TI) listat_kiir(parok_BC546_TI_head);
        if(BD139_ST) listat_kiir(parok_BD139_ST_head);
        if(BD139_TI) listat_kiir(parok_BD139_TI_head);
        if(BD249_ST) listat_kiir(parok_BD249_ST_head);
        if(BD249_TI) listat_kiir(parok_BD249_TI_head);
    }
    else
        printf("*HIBA!*\n");

    /*memóriafelszabadítás*/
    lista_felszamol(parok_BC182_ST_head);
    lista_felszamol(parok_BC182_TI_head);
    lista_felszamol(parok_BC546_ST_head);
    lista_felszamol(parok_BC546_TI_head);
    lista_felszamol(parok_BD139_ST_head);
    lista_felszamol(parok_BD139_TI_head);
    lista_felszamol(parok_BD249_ST_head);
    lista_felszamol(parok_BD249_TI_head);
    /*memóriafelszabadítás*/
    raktarkeszlet *p;
    while(raktar_head != NULL)
    {
        p = raktar_head->next;
        free(raktar_head);
        raktar_head = p;
    }
    /*annak kíírása, hogy hány olyan tranzisztor volt, melynek típusa és gyártója
    megfelel a felhasználó által kértnek, azonban nem került beválogatásra (pl. túl alacsony vagy túl magas béta miatt)*/
    printf("\nNem sikerult bevalogatni %d tranzisztort.\n\n", be_nem_valogatott); 
}

int main(void)
{
    /*raktarkészlet listájának létrehozása*/
    raktarkeszlet *raktar_head = malloc(sizeof(raktarkeszlet));            
    raktar_head->next = malloc(sizeof(raktarkeszlet));
    raktarkeszlet *raktar_tail = raktar_head->next;
    raktar_tail->next = NULL;

    /*béták listájának létrehozása*/
    betak *betak_head = malloc(sizeof(betak));                              
    betak_head->next = malloc(sizeof(betak));
    betak *betak_tail = betak_head->next;
    betak_tail->next = NULL;

    /*függvények meghívása, esetleges hibaüzenetek fogadása, és kíírása a felhasználónak*/
    if(raktarkeszlet_beolvas("raktarkeszlet.txt", raktar_head, raktar_tail))
        {printf("*HIBA!*\nForrasfajl nem talalhato!\n"); return 0;}
    
    if(beta_beolvas("betak.txt", betak_head, betak_tail))
        {printf("*HIBA!*\nForrasfajl nem talalhato!\n"); return 0;}
    
    beta_hozzarendel(raktar_head, betak_head);

    vegrehajt(raktar_head);

    return 0;  
}
