/*
 * Coordenada.cpp
 *
 *  Created on: 04/11/2010
 *      Author: ric
 */

#include "Coordenada.h"

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
