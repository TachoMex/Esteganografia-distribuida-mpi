#ifndef PAQUETE_H
#define PAQUETE_H

#include <cstring>

class PaqueteEntrada{
public:
	unsigned char * buffer;
	int imagen_x, imagen_y, mensaje_l, posicion;
	int buff_size;
	
	void guarda(int dato, int pos){
		memcpy(buffer+pos, &dato,sizeof (int));
	}

	PaqueteEntrada(int imagen_x, int imagen_y, int mensaje_l, int total){
		this->imagen_y = imagen_y;
		this->imagen_x = imagen_x;
		this->mensaje_l = mensaje_l;
		this->posicion = posicion;
		buff_size = imagen_y*imagen_x*3+sizeof(int)*5+mensaje_l;
		buffer = new unsigned char [buff_size];		
		guarda(imagen_x,0);
		guarda(imagen_y,sizeof(int));
		guarda(mensaje_l, 2*sizeof(int)+3*imagen_y*imagen_x);
	}

	PaqueteEntrada(){
		buffer = NULL;
	}

	/*
		| tamaño en y | tamaño en x | imagen | tamaño del mensaje | mensaje | posicion 
	*/

	void guardaImagen(unsigned char* buff){
		memcpy(buffer+2*sizeof(int),buff, imagen_x*imagen_y*3);
	}

	void guardaMensaje(unsigned char*buff){
		memcpy(buffer+3*sizeof(int)+imagen_y*imagen_x*3, buff, mensaje_l);
	}

	void guardaPosicion(int pos){
		posicion = pos;	
		guarda(posicion,3*sizeof(int)+3*imagen_x*imagen_y+mensaje_l);
	}

	void guardaTotal(int t){	
		guarda(t,4*sizeof(int)+3*imagen_x*imagen_y+mensaje_l);
	}

	unsigned char* punteroImagen(){
		return buffer+ 2*sizeof(int);
	}
	int tamImagenX(){
		return *((int*)(buffer+sizeof(int)));
	}
	int tamImagenY(){
		return *((int*)buffer);
	}
	int tamMensaje(){
		return *((int*)(buffer+2*sizeof(int)+3*tamImagenX()*tamImagenY()));
	}

	int totalTramas(){
		return *((int*)(buffer+4*sizeof(int)+3*tamImagenX()*tamImagenY()+tamMensaje()));
	}

	unsigned char* punteroMensaje(){
		return buffer+3*sizeof(int)+3*tamImagenX()*tamImagenY();
	}

	int numeroTrama(){
		return *((int*)(buffer+3*sizeof(int)+3*tamImagenX()*tamImagenY()+tamMensaje()));
	}

};

class PaqueteSalida{
public:
	unsigned char *buffer;
	int size, posicion;
	int buff_size;

	void guarda(int dato, int pos){
		memcpy(buffer+pos, &dato,sizeof (int));
	}

	PaqueteSalida(int s, int pos){
		size = s;
		posicion = pos;
		buff_size = size+2*sizeof(int);
		buffer = new unsigned char[buff_size];
		guarda(s,0);
		guarda(pos,sizeof(int));
	}

	PaqueteSalida(){
		buffer = NULL;
	}

	void guardaMensaje(unsigned char*buff){
		memcpy(buffer+2*sizeof(int), buff, size);
	}	

	int numeroTrama(){
		return *((int*)(buffer+sizeof(int)));
	}
	int tamMensaje(){
		return *((int*)buffer);
	}

	unsigned char *punteroMensaje(){
		return buffer+2*sizeof(int);
	}

};

#endif