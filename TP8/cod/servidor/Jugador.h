#ifndef JUGADOR_H
#define JUGADOR_H

#include "../include/Coordenada.h"
#include "../include/Connection.h"

#include "../include/t_protocolo.h"

class Jugador {

    public:

        Jugador( int Numero,
                 int Vida,
                 Coordenada Posicion,
                 int BombasMax,
                 Connection<char>& Socket);

        ~Jugador();

        bool ponerBomba();
        int restarVida();
        int sumarVida(int cant);

        Coordenada moverArriba();
        Coordenada moverAbajo();
        Coordenada moverIzquierda();
        Coordenada moverDerecha();

        void setPosicion( Coordenada Posicion );

        Coordenada getPosicion();

        void send( t_protocolo data );

        t_protocolo recv();

        void setEnvioEscenario( bool envio );
        bool getEnvioEscenario();

        void eliminar();
        bool eliminado();
        
    private:

        int Numero;
        int Vida;
        Coordenada Posicion;
        int BombasColocadas;
        int BombasMax;

        bool EnvioEscenario;
        
        Connection<char> Socket;

};

Jugador :: Jugador( int Numero,
                    int Vida,
                    Coordenada Posicion,
                    int BombasMax,
                    Connection<char>& Socket){

    this->Socket = Socket;

    this->Numero = Numero;
    this->Vida = Vida;
    this->Posicion = Posicion;
    this->BombasMax = BombasMax;

    this->BombasColocadas = 0;

    EnvioEscenario = false;

}

Jugador :: ~Jugador(){
}

bool Jugador :: ponerBomba(){

    return true;
}

int Jugador :: restarVida(){
    Vida--;
    return this->Vida;
}

int Jugador :: sumarVida(int cant){
    Vida++;
    return this->Vida;
}

Coordenada Jugador :: moverArriba(){
    Posicion.set_y( Posicion.get_y() - 1 );
    return this->Posicion;
}

Coordenada Jugador :: moverAbajo(){
    Posicion.set_y( Posicion.get_y() + 1 );
    return this->Posicion;
}

Coordenada Jugador :: moverIzquierda(){
    Posicion.set_x( Posicion.get_x() - 1 );
    return this->Posicion;
}

Coordenada Jugador :: moverDerecha(){
    Posicion.set_x( Posicion.get_x() + 1 );
    return this->Posicion;
}

void Jugador :: setPosicion( Coordenada Posicion ){
    this->Posicion = Posicion;
}

Coordenada Jugador :: getPosicion(){
    return this->Posicion;
}

void Jugador :: send( t_protocolo data ){
    /*
    cout << "Enviando a jugador " << this->Numero << endl;
    
    cout << "id = "<< data.id << endl
         << "posicion = "<< data.posicion << endl
         << "x = "<< data.x << endl
         << "y = "<< data.y << endl;
    */
    
    Socket.Send ( (char*) &data, sizeof(t_protocolo) );
}

t_protocolo Jugador :: recv(){
    t_protocolo recibido;
    
    Socket.Recv ( (char*) &recibido, sizeof(t_protocolo) );
    /*
    cout << "    Recibido de jugador " << this->Numero << endl;

    cout << "    id = "<< recibido.id << endl
         << "    posicion = "<< recibido.posicion << endl
         << "    x = "<< recibido.x << endl
         << "    y = "<< recibido.y << endl;
    */
    return recibido;
    
}

void Jugador :: setEnvioEscenario( bool envio ){
    this->EnvioEscenario = envio;
}

bool Jugador :: getEnvioEscenario(){
    return this->EnvioEscenario;
}

void Jugador :: eliminar(){
    this->Socket.Close();
    this->Numero = (this->Numero + 1) * -1;
}

bool Jugador :: eliminado(){
    return this->Numero < 0 ? true : false;
}

#endif