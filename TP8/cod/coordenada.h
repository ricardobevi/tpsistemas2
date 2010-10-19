#ifndef COORD_H
#define COORD_H

using namespace std;

class Coordenada
{
    private:
        unsigned int x;
        unsigned int y;
        
    public:
        
        Coordenada( unsigned int x , unsigned int y);
        ~Coordenada();
        
        void set_coordenada( unsigned int x , unsigned int y );
        void set_x ( unsigned int x );
        void set_y ( unsigned int y );
        void get_coordenada( unsigned int &x , unsigned int &y );
        unsigned int  get_x();
        unsigned int  get_y();
};


Coordenada :: Coordenada( unsigned int x , unsigned int y)
{
    this->x = x;
    this->y = y;
}

Coordenada ::  ~Coordenada()
{ 
}
       
void Coordenada :: set_coordenada( unsigned int x , unsigned int y )
{
    this->x = x;
    this->y = y;
}

void Coordenada :: set_x ( unsigned int x )
{
    this->x = x;
}

void Coordenada :: set_y ( unsigned int y )
{
    this->y = y;
}

void Coordenada :: get_coordenada( unsigned int &x , unsigned int &y )
{
    x = this->x;
    y = this->y;
}

unsigned int Coordenada ::  get_x()
{
    return this->x;
}

unsigned int Coordenada ::  get_y()
{
    return this->y;
}


#endif
