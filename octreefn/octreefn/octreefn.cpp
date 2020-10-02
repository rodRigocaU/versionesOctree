#include <iostream>
#include <stdio.h>
#include <fstream>
#include <direct.h>
#include <vector>
#include <sstream>
#include <cstdlib>
#include<stdlib.h>
#include<time.h>
#include<stack>
using namespace std;


//PARA ESTE CASO INTENTE HACERLO CON LA RESPECTIVA COMPROBACION ES DECIR CON 60% Y ASI PERO ME CREA HIJOS VACIOS QUE ME DAN ERROR

template <typename T>
class point
{
public:
    T x;
    T y;
    T z;

    //ES UN NODO VACIO, ES UNA COORDENADA QUE LOS PUNTOS NO TOCARAN PORQUE NO SON MUY GRANDES
    point() : x(-6560), y(-6560), z(-6560) {}

    point(int x_, int y_, int z_) : x(x_), y(y_), z(z_) {}
};


template <typename T>
class octree
{

public:

    //SI CUMPLE CON LA CONDICION DEL 60% LLENO
    bool isTrue = false;
    point<T>* limin;
    point<T>* limax;
    point<T>* points;
    vector<octree<T>*> children;

    octree()
    {
        //PUNTO VACIO DENTRO DEL ARBOL
        points = new point<T>();
    }
    octree(T x_, T y_, T z_)
    {
        //SUBARBOL PERO QUE TENGA UN PUNTO(PARA EL INSERT)
        points = new point<T>(x_, y_, z_);
    }
    octree(T xmin, T ymin, T zmin, T xmax, T ymax, T zmax) {

        //SE ENTIENDE QUE LOS LIMETES YA ESTAN DEFINIDOS, ES DECIR EL LIMITE SUPERIOR NO SERA MENOR AL LIMITE INFERIOR
        /*if (xmax < xmin || y2 < y1 || z2 < z1)
            return;*/

        int areaMin;
        bool noMoreDivision = false;
        points = nullptr;

        limin = new point<T>(xmin, ymin, zmin);
        limax = new point<T>(xmax, ymax, zmax);

        //PARA QUE LOS HIJOS COMIENCEN NULOS
        children.assign(8, nullptr);

        for (int i = 0; i < 8; i++)
        {
            children[i] = new octree<T>();
        }

    }

    int searchminArea(point<T>* pMin, point<T>* pMax)
    {
        int minArea = 0;
        //minArea = 2 * (abs(pMax->x - pMin->x) * abs(pMax->y - pMin->y)) + 2 * (abs(pMax->y - pMin->y) * abs(pMax->z - pMin->z)) + 2 * (abs(pMax->z - pMin->z) * abs(pMax->x - pMin->x));
        minArea = abs(pMax->x - pMin->x) * abs(pMax->y - pMin->y) * abs(pMax->z - pMin->z);
        /*minArea = abs(pMax->z - pMin->z) * abs(pMax->y - pMin->y) * abs(pMax->x - pMin->x);
        minArea /= 1000;*/
       //aqui verificamos el area minima como criterio de parada es decir si llegamos al 60% de la caja entonces podemos para
        minArea *= 0.5;
        return minArea;
    }

   
    //TRABAJO CON PUNTEROS PARA NO EXIGIR EL FIND A CADA MOMENTO
    //GUARDAMOS EL PAPA, GUARDAMOS LA POS DEL HIJO ES DECIR CHILD[POS], SU MIDX Y SU MID Y, EL RECORRIDO DEL STACK PARA DEVOLVER S, Y UN BOOLEANO PARA VER SI ESA RAMA SE SIGUE DIVIENDO
    bool find(T x, T y, T z, octree<T>*& dad, int& poss, T& midx_, T& midy_, T& midz_, stack<octree<T>*> S, bool NoMore)
    {
        //LAS CONDICIONES PARA REVISAR SI ESTA AFUERA O NO DEL CUBO ESTAN DE MAS SE SUPONE QUE LOS PUNTOS DEL OBJ ESTARAN DENTRO DEL LIMITE

         //HACEMOS UNA BUSQUEDA BINARIA
        T midx = (limin->x + limax->x) / 2;
        T midy = (limin->y + limax->y) / 2;
        T midz = (limin->z + limax->z) / 2;

        midx_ = midx;
        midy_ = midy;
        midz_ = midz;


        int pos;

        // DONDE ESTA, EN QUE OCTANTE

        if (x <= midx) {
            if (y <= midy) {
                if (z <= midz)
                    pos = 0;
                else
                    pos = 4;
            }
            else {
                if (z <= midz)
                    pos = 3;
                else
                    pos = 7;
            }
        }
        else {
            if (y <= midy) {
                if (z <= midz)
                    pos = 1;
                else
                    pos = 5;
            }
            else {
                if (z <= midz)
                    pos = 2;
                else
                    pos = 5;
            }
        }
        poss = pos;
        if (children[pos]->points == nullptr) {
            S.push(this);
            return children[pos]->find(x, y, z, dad, poss, midx_, midy_, midz_, S, NoMore);
        }
        else if (children[pos]->points->x == -6560) {
            //poss = pos;
            dad = this;
            S.push(dad);
            return 0;
        }
        else {
            dad = this;
            S.push(dad);
            //CHEKEAMOS AL PUNTO
                if (x == children[pos]->points->x && y == children[pos]->points->y && z == children[pos]->points->z)
                {
                    //poss = pos;
                    return true;
                }
            
        }
        //RETORNAMOS THIS QUE ES EL PADRE PARA PODER TRABAJAR CON EL POR ESO GUARDO LOS MIDX... Y LA POS DEL HIJO
        return false;
    }

    //FUNCION RECURSIVA DE INSERTADO
    bool reinsert(T x, T y, T z, octree<T>* dad, int poss, T midx_, T midy_, T midz_)
    {
        T midx = midx_;
        T midy = midy_;
        T midz = midz_;

        int pos = poss;
     
        if (dad->children[pos]->points->x == -6560) {

            delete (dad->children[pos]);
            dad->children[pos] = new octree<T>(x, y, z);
            return true;
        }
        else
        {
            T x_ = dad->children[pos]->points->x, y_ = dad->children[pos]->points->y, z_ = dad->children[pos]->points->z;

            delete (dad->children[pos]);
            dad->children[pos] = nullptr;

            if (pos == 0) {
                dad->children[pos] = new octree(dad->limin->x, dad->limin->y, dad->limin->z, midx, midy, midz);
            }
            else if (pos == 1) {
                dad->children[pos] = new octree(midx + 1, dad->limin->y, dad->limin->z, dad->limax->x, midy, midz);
            }
            else if (pos == 2) {
                dad->children[pos] = new octree(midx + 1, midy + 1, dad->limin->z, dad->limax->x, dad->limax->y, midz);
            }
            else if (pos == 3) {
                dad->children[pos] = new octree(dad->limin->x, midy + 1, dad->limin->z, midx, dad->limax->y, midz);
            }
            else if (pos == 4) {
                dad->children[pos] = new octree(dad->limin->x, dad->limin->y, midz + 1, midx, midy, dad->limax->z);
            }
            else if (pos == 5) {
                dad->children[pos] = new octree(midx + 1, dad->limin->y, midz + 1, dad->limax->x, midy, dad->limax->z);
            }
            else if (pos == 6) {
                dad->children[pos] = new octree(midx + 1, midy + 1, midz + 1, dad->limax->x, dad->limax->y, dad->limax->z);
            }
            else if (pos == 7) {
                dad->children[pos] = new octree(dad->limin->x, midy + 1, midz + 1, midx, dad->limax->y, dad->limax->z);
            }
            dad->children[pos]->insert(x_, y_, z_);
            dad->children[pos]->insert(x, y, z);
            return true;
        }
    }
    bool insert(T x, T y, T z)
    {
        octree<T>* aux;
        int pos;
        bool NoMore = false;
        stack<octree<T>*> S;
        T midx_;
        T midy_;
        T midz_;

        //BUSCAMOS SI YA ESTA EN EL ARRAY , TENIENDO EN CUENTA EL AUX EL CUAL ES EL PADRE
        if (find(x, y, z, aux, pos, midx_, midy_, midz_,S,NoMore))
        {
            return false;
        }
        else
        {
            reinsert(x, y, z, aux, pos, midx_, midy_, midz_);
            return true;
        }

    }

    bool deleted(T x_, T y_, T z_)
    {
        octree<T>* aux;
        int pos = 0;
        T midx_;
        T midy_;
        T midz_;

        if (find(x_, y_, z_, aux, pos, midx_, midy_, midz_))
        {
            
            //BORRAMOS DESDE CUALQUIER HIJO
            delete(aux->children[pos]);
            aux->children[pos] = new octree<T>();
            
            return true;
        }
        else
        {
            return false;
        }
    }
};

//BUSQUEDA DE LOS LIMITES ENTEROS
void searchMinMax(float* ar)
{
    ifstream lec;
    //PARA LEER LOS OBJ CONVIERTO EL ARCHIVO EN UN TXT Y BUSCO DENTRO DE LOS PUNTOS LOS LIMITES PARA CREAR LOS CUBOS CORRESPONDIENTES
    lec.open("cat.txt");

    float minz = 100.0;
    float maxz = -100.0;
    float minx = 100.0;
    float maxx = -100.0;
    float miny = 100.0;
    float maxy = -100.0;


    while (!lec.eof())
    {

        string aux;
        getline(lec, aux);
        if (aux.substr(0, 1) == "#")
        {
            break;
        }
        else
        {
            aux = aux.substr(3, aux.length() - 3);
            int i = 0;
            size_t found = aux.find(" ");
            //x max y x min
            float arr[3];

            stringstream aa(aux.substr(i, found));
            aa >> arr[0];


            if (arr[0] > maxx)
            {
                maxx = arr[0];
            }
            else if (arr[0] < minx)
            {
                minx = arr[0];
            }

            //PARA Y
            i = found + 1;
            found = aux.find(" ", found + 1);

            stringstream bb(aux.substr(i, found));
            bb >> arr[1];

        
            if (arr[1] > maxy)
            {
                maxy = arr[1];
            }
            else if (arr[1] < miny)
            {
                miny = arr[1];
            }

            //PARA Z

            i = found + 1;

            stringstream cc(aux.substr(i, aux.length() - i));
            cc >> arr[2];
           
            if (arr[2] > maxz)
            {
                maxz = arr[2];
            }
            else if (arr[2] < minz)
            {
                minz = arr[2];
            }
        }
    }

    ar[0] = minx;
    ar[1] = maxx;
    ar[2] = miny;
    ar[3] = maxy;
    ar[4] = minz;
    ar[5] = maxz;


    lec.close();

}


int main()
{
    //BUSCAR LIMITES DEL OBJ Y TRABAJAR CON SUS PUNTOS
    /*int arr[6];

    searchMinMax(arr);
    octree tree(arr[0], arr[2], arr[4], arr[1], arr[3], arr[5]);

    ifstream lec;
    lec.open("cat.txt");

    while (!lec.eof())
    {
        string aux;
        getline(lec, aux);
        if (aux.substr(0, 1) == "#")
        {
            break;
        }
        else
        {
            aux = aux.substr(3, aux.length() - 3);
            int i = 0;
            size_t found = aux.find(" ");
            //x max y x min
            int arr[3];

            stringstream aa(aux.substr(i, found));
            aa >> arr[0];

            //PARA Y
            i = found + 1;
            found = aux.find(" ", found + 1);

            stringstream bb(aux.substr(i, found));
            bb >> arr[1];

            //PARA Z

            i = found + 1;

            stringstream cc(aux.substr(i, aux.length() - i));
            cc >> arr[2];

            tree.insert(arr[0], arr[1], arr[2]);
        }
    }
    lec.close();
    
    */


    octree<int> tree(1, 1, 1, 40, 40, 40);
    srand(time(NULL));

    int xx;
    int yy;
    int zz;

    for (int i = 1; i < 40; i++)
    {
        xx = 2 + rand() % (39 - 1);
        yy = 2 + rand() % (39 - 1);
        zz = 2 + rand() % (39 - 1);
        tree.insert(xx, yy, zz);
        cout << xx << " " << yy << " " << zz << '\n';
    }


    octree<int> *pa;
    int a; int b; int c;
    int pos;
    stack<octree<int>*> s;
    bool nm = false;
    cout << tree.find(xx, yy,zz, pa, pos, a, b, c,s,nm) << '\n';

    /// <summary>
    /// NOS QUEDAMOS EN LO DEL STACK
    /// </summary>
    /// <returns></returns>

    point<int>* p1;
    point<int>* p2;
    p1 = new point<int>(1, 1, 1);
    p2 = new point<int>(40, 40, 40);

    
    cout << tree.searchminArea(p1, p2) << '\n';

}
