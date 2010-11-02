#ifndef COORD_H
#define COORD_H

using namespace std;

class Coordenada
{
    private:
        int x;
        int y;
        
    public:
        
        Coordenada( int x = 0 , int y = 0);
        Coordenada( const Coordenada& );
        ~Coordenada();
        
        bool coordenadaBandera(); 
        
        void set_coordenada( int x , int y );
        void set_x ( int x );
        void set_y ( int y );
        void get_coordenada( int &x , int &y );
        int&  get_x();
        int&  get_y();
        Coordenada operator = ( const Coordenada& );
};


bool Coordenada ::  coordenadaBandera() 
{
    return (this->x != -1 || this->y != -1 );
    
}

Coordenada :: Coordenada( int x , int y)
{
    this->x = x;
    this->y = y;
}

Coordenada ::  Coordenada( const Coordenada& obj)
{
    this->x = obj.x;
    this->y = obj.y;
}

Coordenada ::  ~Coordenada()
{ 
}
       
void Coordenada :: set_coordenada( int x , int y )
{
    this->x = x;
    this->y = y;
}

void Coordenada :: set_x ( int x )
{
    this->x = x;
}

void Coordenada :: set_y ( int y )
{
    this->y = y;
}

void Coordenada :: get_coordenada( int &x , int &y )
{
    x = this->x;
    y = this->y;
}

int& Coordenada ::  get_x()
{
    return this->x;
}

int& Coordenada ::  get_y()
{
    return this->y;
}

Coordenada Coordenada :: operator = ( const Coordenada& obj)
{
    this->x = obj.x;
    this->y = obj.y;
    
    return *this;
}




#endif
