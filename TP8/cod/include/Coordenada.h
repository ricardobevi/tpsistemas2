/*
 * Coordenada.h
 *
 *  Created on: 04/11/2010
 *      Author: ric
 */

#ifndef COORDENADA_H_
#define COORDENADA_H_

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

        bool operator == (const Coordenada& obj);
};

#endif /* COORDENADA_H_ */
