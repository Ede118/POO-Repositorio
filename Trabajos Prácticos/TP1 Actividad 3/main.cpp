#include <iostream>
using namespace std;

// Clase que representa un móvil en el plano

class Movil{
    private:
        int x;
        int y;
        string nombre;

    public:
        Movil(string n, int _x, int _y){
            this->x = _x;
            this->y = _y;
            this->nombre = n;
        }

        // Según tengo entendido sería mejor hacer:
        // Movil(string n, int _x, int _y) : x(_x), y(_y), nombre(n) {}

        void mover(int dx, int dy){
            x = dx;
            y = dy;
        }

        void reportar_estado(){
            cout << "\nEl movil tiene coordenadas (x,y) = (" << x << ", " << y <<") \n\n" << endl;
        }

};

int main(){
    Movil m1("Dron1", 0, 0);
    m1.mover(3, 2);
    m1.reportar_estado();
    m1.mover(-8, 1);
    m1.reportar_estado();
    return 0;
}