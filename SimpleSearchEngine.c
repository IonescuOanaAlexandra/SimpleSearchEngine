
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//lista de cuvinte cheie
typedef struct cuvant{
    char *text;
    struct cuvant *next;
}Cuvant;

//lista de fisiere
typedef struct filelist{
    char id[50];
    int relevanta;
    Cuvant *listaCuvinte;
    struct filelist *next, *prev;
}CelFile, *LFile;

//lista de fisiere referinta
typedef struct refFile{
    CelFile *fisier;
    struct refFile *next;
}RefFile;

//arborele de regasire multicai
typedef struct tree{
    struct tree *children[26];
    int eow;
    RefFile *files;
}Tree;
//functie de initializare
Tree* initTree()
{
    //alocam memorie pentru radacina
    Tree *root = (Tree*)malloc(sizeof(Tree));
    if(root != NULL)
    {
        //initializam radacina,copiii si lista de fisiere de referinta
        root->eow=0;
        root->files=NULL;
        for(int i=0; i<26; i++)
            root->children[i]=NULL;
    }
    return root;
}

//functie de creare fisier
LFile createFile(char *id, int rel)
{
    LFile new = (LFile)malloc(sizeof(CelFile));
    if(new != NULL)
    {
        //setam id-ul, scorul de relevanta si initializam fisierul
        strcpy(new->id, id);
        new->relevanta=rel;
        new->listaCuvinte=NULL;
        new->next=new->prev=NULL;
    }
    return new;
}

//functie de creare si initializare lista de fisiere
RefFile* initRef(CelFile *fisier)
{
    RefFile *new=(RefFile*)malloc(sizeof(RefFile));
    if(new != NULL)
    {
        new->fisier=fisier;
        new->next=NULL;
    }
    return new;
}

//functie de adaugare fisier
LFile addFile(LFile *head, char* id, int rel)
{
    LFile new = (LFile)malloc(sizeof(CelFile));
    if(new == NULL) return NULL;
    strcpy(new->id, id);
    new->relevanta=rel;
    new->listaCuvinte=NULL;
    new->next = NULL;
    //verificam daca lista e goala
    if(*head == NULL)
    {
        new->prev = NULL;
        *head = new;
        return new;
    }
    //adaugam in lista
    LFile aux = *head;
    while(aux->next != NULL)
        aux = aux->next;
    aux->next=new;
    new->prev=aux;
    return new;
}

//functie de inserare in lista de referinte
void insertRef(Tree *t, LFile fisier)
{
    RefFile *new = initRef(fisier);
    if(new == NULL) return;
    //verificam daca lista e goala sau id-ul e mai mic decat primul
    if(t->files == NULL || strcmp(fisier->id, t->files->fisier->id) < 0)
    {
        //adaugam la inceputul listei
        new->next=t->files;
        t->files = new;
        return;
    }
    //parcurgem lista pana gasim locul lexicografic si adaugam in lista
    RefFile *aux = t->files;
    while(aux->next != NULL && strcmp(aux->next->fisier->id, fisier->id)<0)
        aux=aux->next;
    new->next=aux->next;
    aux->next=new;

}

//functie de adaugare in arbore
void addTree(Tree *root, char* word, LFile fisier)
{
    if(root == NULL || word == NULL || fisier == NULL) return;
    Tree *aux = root;
    int i=0;
    //parcurgem cuvantul litera cu litera
    while(word[i] != '\0')
    {
        //daca litera nu exista in arbore o adaugam
        int k = word[i] - 'a';
        if(aux->children[k]==NULL)
            aux->children[k]=initTree();
        aux=aux->children[k];
        i++;
    }
    //setam nodul ca final de cuvant si adaugam fisierul in lista de referinte
    aux->eow=1;
    insertRef(aux, fisier);
}

void ADD(LFile *l, Tree *root, char* id, int scor, int t, FILE *fin, FILE* fout)
{
    int ok=0;
    LFile aux=*l;
    //verificam daca fisierul exista
    while(aux != NULL)
    {
        if(strcmp(aux->id, id) == 0)
        {
            ok=1;
            fprintf(fout,"EXISTS\n");
            break;
        }
        aux=aux->next;
    }
    LFile fisier=NULL;
    //daca nu exista il adaugam
    if(!ok)
        fisier=addFile(l, id, scor);
    char cuv[120];
    //citim pe rand cuvintele cheie
    while(t)
    {
        fscanf(fin, "%s", cuv);
        if(fisier != NULL)
        {
            int dublura=0;
            //verificam daca a mai fost adaugat
            Cuvant *verificare=fisier->listaCuvinte;
            while(verificare != NULL)
            {
                if(strcmp(verificare->text,cuv)==0)
                {
                    dublura=1;
                    break;
                }
                verificare=verificare->next;
            }
            //daca nu e dublura il adaugam in arbore si in lista
            if(!dublura)
            {
                addTree(root,cuv,fisier);
                Cuvant* new = (Cuvant*)malloc(sizeof(Cuvant));
                if(new != NULL)
                {
                    new->text = strdup(cuv); 
                    new->next = fisier->listaCuvinte;
                    fisier->listaCuvinte = new;
                }
            }
            
        }    
        t--;
    }
    if(!ok)
        fprintf(fout,"OK\n");
}

//functie de gasire a fisierelor de referinta ale unui cuvant
void FIND(Tree *root, char* word, FILE *fout)
{
    if(root == NULL || word == NULL) return;
    Tree* aux=root;
    int i=0;
    //parcurgem arborele dupa literele cuvantului
    while(word[i] != '\0')
    {
        int k = word[i] - 'a';
        if(aux->children[k]!=NULL)
            aux=aux->children[k];
        else{
            fprintf(fout, "EMPTY\n");
            return;
        } 
        i++;
    }
    //cand ajungem la ultima litera verificam ca e final de cuvant
    if(aux->eow==1)
    {
        int k=0;
        RefFile *ref=aux->files;
        //verificam daca lista e goala
        if(ref == NULL)
            fprintf(fout, "EMPTY\n");
        //parcurgem lista si numaram cate fisiere sunt
        while(ref!=NULL)
        {
            k++;
            ref=ref->next;
        }
        fprintf(fout, "%d", k);
        ref=aux->files;
        //parcurgem lista si afisam fisierele
        while(ref != NULL)
        {
            fprintf(fout, " %s", ref->fisier->id);
            ref=ref->next;
        }
        fprintf(fout, "\n");
    }
    else fprintf(fout, "EMPTY\n");
}

//functie in care verificam daca un nod are copii
int nodParinte(Tree *nod)
{
    for(int i=0; i<26; i++)
    {
        if(nod->children[i]!=NULL)
            return 1;
    }
    return 0;
}

//functie de eliminare cuvant cheie din arbore
Tree* rmvKW(Tree* nod, char* word, int adancime)
{
    if(nod == NULL) return NULL;
    //verificam daca nodul e final de cuvant
    if(word[adancime] == '\0')
    {
        //eliminam marcajul de final de cuvant daca nu are fisiere
        if(nod->files == NULL)
            nod->eow=0;
        //daca e nod terminal il eliminam
        if(!nodParinte(nod) && nod->eow == 0 && adancime!=0)
        {
            free(nod);
            nod=NULL;
        }
        return nod;
    }
    //mergem recursiv in adancime
    int i=word[adancime] - 'a';
    nod->children[i]=rmvKW(nod->children[i], word, adancime+1);
    //verificam daca nodul poate fi sters
    if(!nodParinte(nod) && nod->eow==0 && nod->files == NULL && adancime!=0)
    {
        free(nod);
        nod=NULL;
    }
    return nod;
}

//functie de eliminare cuvant cheie
void DELKW(char *word, char *id, LFile l, Tree *root, FILE* fout)
{
    int ok=0;
    LFile aux=l;
    //parcurgem lista si verificam ca exista fisierul
    while(aux != NULL)
    {
        if(strcmp(aux->id, id) == 0)
        {
            ok=1;
            break;
        }
        aux=aux->next;
    }
    if(!ok)
    {
        fprintf(fout,"NOT FOUND\n");
        return;
    }
    //parcurgem lista de cuvinte cheie si cautam cuvantul
    Cuvant *key = aux->listaCuvinte;
    ok=0;
    while(key != NULL)
    {
        if(strcmp(key->text, word)==0)
        {
            ok=1;
            break;
        }
        key=key->next;
    }
    if(!ok)
    {
        fprintf(fout,"OK\n");
        return;
    } 
    Tree *t = root;
    int i=0;
    //parcurgem arborele sa ajungem la nodul terminal al cuvantului
    while(word[i] != '\0' && t != NULL)
    {
        int k = word[i] - 'a';
        t=t->children[k];
        i++;
    }
    //parcurgem lista de fisiere de referinta si eliminam fisierul 
    if(t!=NULL && t->files!=NULL)
    {
         RefFile* ref = t->files;
        //caz pentru inceput de lista
        if(t->files != NULL && strcmp(t->files->fisier->id, id)==0)
        {
            RefFile* temp = t->files;
            t->files=t->files->next;
            free(temp);
        }
        else
        {
            //parcurgem lista pana gasim fisierul si il eliminam
            while(ref->next != NULL)
            {
                if(strcmp(ref->next->fisier->id, id)==0)
                {
                    RefFile *temp=ref->next;
                    ref->next=temp->next;
                    free(temp);
                    break;
                }
                ref=ref->next;
            }
        }
        //daca lista de fisiere de referinta ramane goala eliminam cuvantul din arbore
        if(t->files == NULL)
        {
            t->eow=0;
            rmvKW(root,word,0);
        }
    }
   
    //eliminam cuvantul cheie din lista fisierului
    if(key == aux->listaCuvinte)
    {
        Cuvant *temp = key;
        aux->listaCuvinte=aux->listaCuvinte->next;
        free(temp->text);
        free(temp);
    }
    else
    {
        Cuvant *ant = aux->listaCuvinte;
        while(ant->next != key)
            ant=ant->next;
        ant->next=key->next;
        free(key->text);
        free(key);
    }
    fprintf(fout, "OK\n");
}

//functie de stergere fisier
void DEL(Tree *root, LFile *l, char* id, FILE* fout)
{
    int ok=0;
    LFile aux=*l;
    //parcurgem lista si cautam fisierul
    while(aux != NULL)
    {
        if(strcmp(aux->id, id) == 0)
        {
            ok=1;
            break;
        }
        aux=aux->next;
    }
    if(!ok)
    {
        fprintf(fout, "NOT FOUND\n");
        return;
    }
       
    Cuvant* words=aux->listaCuvinte;
    //luam pe rand fiecare cuvant cheie din fisier
    while(words != NULL)
    {
        Tree* t=root;
        int i=0;
        //parcurgem arborele sa ajungem la nodul terminal al cuvantului
        while(words->text[i] != '\0' && t != NULL)
        {
            int k = words->text[i] - 'a';
            if(t->children[k]!=NULL)
                t=t->children[k];
            else break;
            i++;
        }
        //cautam fisierul in lista de referinte si il eliminam
        if(t!=NULL && t->files != NULL && words->text[i] == '\0')
        {
            if(strcmp(t->files->fisier->id, id) == 0)
            {
                RefFile* temp=t->files;
                t->files=t->files->next;
                free(temp);
                if(t->files == NULL)
                    rmvKW(root, words->text, 0);
            }
            else
            {
                RefFile* ref=t->files;
                while(ref->next != NULL && strcmp(ref->next->fisier->id, id)<0)
                {
                    ref=ref->next;
                }
                if(ref->next != NULL && strcmp(ref->next->fisier->id, id)==0)
                {
                    RefFile* temp=ref->next;
                    ref->next=temp->next;
                    free(temp);
                    if(t->files == NULL)
                        rmvKW(root, words->text, 0);
                }
            }
        }
        words=words->next;
    }
    //parcurgem lista de cuvinte cheie a fisierului si le eliminam
    Cuvant* cuv =aux->listaCuvinte;
    while(cuv != NULL)
    {
        Cuvant* nextW =cuv->next;
        free(cuv->text);
        free(cuv);
        cuv=nextW;
    }
    //eliminam fisierul din lista de fisiere
    if(aux==*l)
    {
        LFile temp=*l;
        *l=(*l)->next;
        if(*l != NULL)
            (*l)->prev=NULL;
        free(temp);
    }
    else 
    {
        if(aux->next != NULL)
        {
            aux->prev->next=aux->next;
            aux->next->prev=aux->prev;
        }
        else
        {
            aux->prev->next=NULL;
        }
        free(aux);
    }
    fprintf(fout,"OK\n");
}

//functie de adaugare cuvant cheie
void ADDKW(LFile *head, char* id, Tree* root, char* cuv, FILE* fout)
{
    LFile l = *head;
    int ok=0;
    int gasit=0;
    //cautam fisierul in lista
    while(l!=NULL)
    {
        if(strcmp(l->id, id)==0)
        {
            ok=1;
            break;
        }
        l=l->next;
    }
    if(!ok)
    {
        fprintf(fout, "NOT FOUND\n");
        return;
    }
    //verificam daca cuvantul cheie exista deja in lista fisierului
    else{
        Cuvant* aux=l->listaCuvinte;
        while(aux != NULL)
        {
            if(strcmp(aux->text, cuv)==0)
            {
                gasit=1;
                break;
            }
            aux=aux->next;
        }
    }
    //daca nu exista il adaugam in lista fisierului si in arbore
    if(!gasit)
    {
        Cuvant* new = (Cuvant*)malloc(sizeof(Cuvant));
        if(new == NULL) return;
        addTree(root, cuv,l);
        new->next=NULL;
        new->text=strdup(cuv);
        if(l->listaCuvinte == NULL)
        {
            l->listaCuvinte=new;
        }
        else{
            Cuvant* aux=l->listaCuvinte;
            while(aux->next != NULL)
            {
                aux=aux->next;
            }
            aux->next=new;
        }
    }
    fprintf(fout, "OK\n");
}

//functie de comparare a relevantei
int cmpPriority(LFile a, LFile b)
{
    if(a->relevanta > b->relevanta) return 1;
    if(a->relevanta < b->relevanta) return 0;
    if(strcmp(a->id, b->id)<0) return 1;
    return 0;
}

void Heapify(LFile* heap, int i, int n)
{
    int max=i;
    int st=2*i+1;
    int dr=2*i+2;
    if(st < n && cmpPriority(heap[st],heap[max]))
        max=st;
    if(dr < n && cmpPriority(heap[dr],heap[max]))
        max=dr;
    if(i != max)
    {
        LFile aux=heap[i];
        heap[i]=heap[max];
        heap[max]=aux;
        Heapify(heap, max, n);
    }
}

//functie de extragere top k fisiere de referinta 
void TOPK(Tree* root, char* cuv, int k, FILE* fout)
{
    int n=0;
    Tree* aux=root;
    int i=0;
    //parcurgem arborele sa ajungem la nodul terminal al cuvantului
    while(cuv[i] != '\0')
    {
        int nr = cuv[i] - 'a';
        if(aux->children[nr]!=NULL)
            aux=aux->children[nr];
        else{
            fprintf(fout, "EMPTY\n");
            return;
        }
        i++;
    }
    //numaram fisierele de referinta
    if(aux->eow==1)
    {
        RefFile *ref=aux->files;
        while(ref != NULL)
        {
            n++;
            ref=ref->next;
        }
    }
    LFile* heap = (LFile*)malloc(n*sizeof(LFile*));
    if(aux->files == NULL)
    {
        fprintf(fout, "EMPTY\n");
        return;
    }
    //introducem fisierele in heap
    RefFile *ref = aux->files;
    for(int i=0; i<n; i++)
    {
        heap[i]=ref->fisier;
        ref=ref->next;
    }
    //le organizam
    for(int i=n/2-1; i>=0; i--)
        Heapify(heap, i, n);
    int counter;
    if(k<n)
        counter=k;
    else counter=n;
    fprintf(fout, "%d ", counter);
    //extragem pe rand cel mai relevant fisier si reorganizam heap-ul
    while(counter)
    {
        fprintf(fout, "%s ", heap[0]->id);
        LFile aux=heap[0];
        heap[0]=heap[n-1];
        heap[n-1]=aux;
        n--;
        counter--;
        Heapify(heap, 0, n);
    }
    fprintf(fout, "\n");
    free(heap);
}

//functie de reconstruire cuvant din arbore
void parcurgereTree(Tree* nod, int adancime, char* word, FILE *fout)
{
    if(nod == NULL) return;
    //verificam daca nodul este final de cuvant
    if(nod->eow == 1)
    {
        word[adancime]='\0';
        int k=0;
        RefFile* aux=nod->files;
        //numaram cate fisiere de referinta sunt
        while(aux != NULL)
        {
            k++;
            aux=aux->next;
        }
        fprintf(fout, "%s %d", word, k);
        aux=nod->files;
        //parcurgem lista de referinte si afisam fisierele
        while(aux != NULL)
        {
            fprintf(fout, " %s", aux->fisier->id);
            aux=aux->next;
        }
        fprintf(fout, "\n");
    }
    //reconstruim cuvantul si apelam recursiv functia pentru fiecare din copii
    for(int i=0; i<26; i++)
    {
        if(nod->children[i] != NULL)
        {
            word[adancime]=i + 'a';
            parcurgereTree(nod->children[i],adancime+1,word, fout);
        }
    }
}

//printeaza cuvintele cheie din arbore plus fisierele de referinta
void PRINT(Tree* root, FILE *fout)
{
    if (root == NULL || !nodParinte(root))
    {
        fprintf(fout, "EMPTY\n");
        return;
    }
    char word[120];
    parcurgereTree(root, 0, word,fout);
}

//functie de salvare fisiere care contin prefixul in cuvintele cheie
void parcurgerePrefix(Tree* nod, LFile** v, int* k, int* capacity)
{
    if(nod==NULL) return;
    //daca nodul este sfarsit de cuvant salvam fisierele de referinta intr-un vector
    if(nod->eow == 1)
    {
        RefFile* aux=nod->files;
        while(aux!=NULL)
        {
            //alocam mai multa memorie daca este nevoie
            if(*k >= *capacity)
            {
                (*capacity)*=2;
                *v=(LFile*)realloc(*v,(*capacity)*sizeof(LFile));
            }
            (*v)[*k]=aux->fisier;
            (*k)++;
            aux=aux->next;
        }
    }
    //apelam functia recursiv pentru copiii nodului
    for(int i=0; i<26; i++)
    {
        if(nod->children[i] != NULL)
            parcurgerePrefix(nod->children[i], v, k, capacity);
    }
}

//functie de afisare fisiere care contin un prefix
void PREFIX(Tree *root, char* word, FILE *fout)
{

    if(root == NULL)
        return;
    Tree* aux=root;
    int i=0;
    //parcurgem arborele pana la finalul prefixului
    while(word[i] != '\0')
    {
        int nr = word[i] - 'a';
        if(aux->children[nr]!=NULL)
            aux=aux->children[nr];
        else{
            fprintf(fout, "EMPTY\n");
            return;
        }
        i++;
    }
    int capacity=10;
    int k=0;
    //alocam memorie pentru vector
    LFile *v = (LFile*)malloc(capacity*sizeof(LFile));
    //apelam functia de la finalul prefixului
    parcurgerePrefix(aux, &v, &k, &capacity);
    //sortam vectorul
    for(int i=0;i<k-1;i++)
    {
        for(int j=i+1;j<k;j++)
        {
            if(strcmp(v[i]->id,v[j]->id)>0)
            {
                LFile aux=v[i];
                v[i]=v[j];
                v[j]=aux;
            }
        }
    }
    int print=1;
    //eliminam dublurile
    if(k==0)
    {
        fprintf(fout, "EMPTY\n");
        free(v);
        return;
    }
    else{
        for(int i=1; i<k; i++)
        {
            if(strcmp(v[i]->id,v[i-1]->id)!=0)
            {
                v[print]=v[i];
                print++;
            }
        }
    }
    //printam numarul de fisiere si fisierele
    fprintf(fout, "%d", print);
    for(int i=0; i<print; i++)
        fprintf(fout, " %s", v[i]->id);
    fprintf(fout, "\n");
    free(v);
}

//functie de eliberare arbore
void freeTree(Tree* nod)
{
    if(nod == NULL) return;
    for(int i=0; i<26; i++)
        freeTree(nod->children[i]);
    if(nod->eow==1)
    {
        while(nod->files != NULL)
        {
            RefFile* temp=nod->files;
            nod->files=nod->files->next;
            free(temp);
        }
    }
    free(nod);
}

//functie de eliberare program
void freeSystem(Tree* root, LFile *l)
{
    //eliberam arborele 
    freeTree(root);
    //eliberam lista de fisiere
    while((*l) != NULL)
    {
        LFile aux= *l;
        while((*l)->listaCuvinte != NULL)
        {
            Cuvant* temp=(*l)->listaCuvinte;
            (*l)->listaCuvinte=(*l)->listaCuvinte->next;
            free(temp->text);
            free(temp);
        }
        *l=(*l)->next;
        free(aux);
    }
}

int main()
{
    //deschidem fisierele si initializam structurile
    FILE *fin = fopen("indexare.in", "r");
    FILE *fout = fopen("indexare.out", "w");
    int nr_op;
    fscanf(fin, "%d", &nr_op);
    Tree* root=initTree();
    LFile head=NULL;
    char cmd[20];
    char id[50];
    char word[120];
    int rel, nr,k;
    //citim pe rand operatiile si apelam functiile corespunzatoare
    for(int i=0; i<nr_op; i++)
    {
        fscanf(fin, "%s", cmd);
        if(strcmp(cmd, "ADD")==0)
        {
            fscanf(fin, "%s %d %d", id, &rel, &nr);
            ADD(&head,root,id,rel,nr,fin,fout);
        }
        else if(strcmp(cmd,"FIND")==0)
        {
            fscanf(fin,"%s",word);
            FIND(root,word,fout);
        }
        else if(strcmp(cmd,"ADDKW")==0)
        {
            fscanf(fin, "%s %s", id, word);
            ADDKW(&head, id, root, word,fout);
        }
        else if(strcmp(cmd,"DEL")==0)
        {
            fscanf(fin, "%s", id);
            DEL(root,&head,id,fout);
        }
        else if(strcmp(cmd,"DELKW")==0)
        {
            fscanf(fin,"%s %s",id,word);
            DELKW(word,id,head,root,fout);
        }
        else if(strcmp(cmd,"TOPK")==0)
        {
            fscanf(fin, "%s %d", word, &k);
            TOPK(root, word,k,fout);
        }
        else if(strcmp(cmd,"PRINT")==0)
        {
            PRINT(root,fout);
        }
        else if(strcmp(cmd,"PREFIX")==0)
        {
            fscanf(fin, "%s", word);
            PREFIX(root, word, fout);
        }
    }
    //inchidem fisierele si eliberam memoria
    fclose(fin);
    fclose(fout);
    freeSystem(root,&head);
    return 0;
}