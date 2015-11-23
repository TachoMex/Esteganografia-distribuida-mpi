#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <mpi.h>
#include "imagen.h"
#include <iostream>


#include "paquete.h"


using namespace std;

const int N = 100;
const char NOM_MAT[] = "matriz.txt";
const int MAX_SIZE = 24000000;


typedef double* Matriz;

void cargaLLave(Matriz& x, const string& s){
	ifstream f(s); 
	for(int i=0;i<N;i++){
		for(int j=0;j<N;j++){
			f>>x[i*N+j];
		}
	}
	f.close();
}


void crearMatrizTexto(Matriz& m, const char* mensaje, int size, int& R, int& C){
	R = N;
	C = size / N;
	int relleno = R * C - size;

	if(relleno){
		C++;
	}

	m = new double[R*C];
	memset(m,0,sizeof m);

	int k = 0;


	for(int i=0;i<R;i++){
		for(int j=0;j<C;j++){
			if(k>=size)
				return;
			m[i*C+j] = mensaje[k++];
		}
	}

}

void incrustar(color* v, unsigned char c){
	v[0].r=bool(c&128)+(v[0].r&254);
	v[0].g=bool(c&64)+ (v[0].g&254);
	v[0].b=bool(c&32)+ (v[0].b&254);
	v[1].r=bool(c&16)+ (v[1].r&254);
	v[1].g=bool(c&8)+  (v[1].g&254);
	v[1].b=bool(c&4)+  (v[1].b&254);
	v[2].r=bool(c&2)+  (v[2].r&254);
	v[2].g=bool(c&1)+  (v[2].g&254);
} 


int main(int argc, char *argv[]){
	const int MASTER = 0;
	const int TAG_GENERAL = 1;

	int numTasks;
	int rank;
	int source;
	int dest;
	int rc;
	int count;
	int dataWaitingFlag;


	MPI_Status Stat;

	// Initialize the MPI stack and pass 'argc' and 'argv' to each slave node
	MPI_Init(&argc,&argv);

	// Gets number of tasks/processes that this program is running on
	MPI_Comm_size(MPI_COMM_WORLD, &numTasks);

	// Gets the rank (process/task number) that this program is running on
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	// If the master node
	if (rank == MASTER) {
		// Abrir el mensaje:
		char *mensaje;
		ifstream arch("mensaje.txt");
		arch.seekg(0,arch.end);
		int size = arch.tellg();
		mensaje = new char[size];
		arch.seekg(0,arch.beg);
		arch.read(mensaje,size);
		// Ponerlo en la matriz
		// poner en el paquete
		// abrir imagen portadora
		imagen I;
		I.leerBMP("portador.bmp");
		PaqueteEntrada paq(I.columnas(), I.filas(), size, 0);
		paq.guardaImagen((unsigned char*)I.pixels);
		paq.guardaMensaje((unsigned char*)mensaje);
		paq.guardaTotal(numTasks-1);

		for (dest = 1; dest < numTasks; dest++) {
			//Poner bloque de la imagen en el paquete
			//Mandar paquetes	
			paq.guardaPosicion(dest);
			rc = MPI_Send(paq.buffer, paq.buff_size, MPI_CHAR, dest, TAG_GENERAL, MPI_COMM_WORLD);
			printf("Se envio paquete a %d\n",dest);
		}

		unsigned char *buffer = new unsigned char[I.filas()*I.columnas()];
		memset(I.pixels,0,sizeof(I.pixels));
		for(dest = 1; dest < numTasks; dest++){
			rc = MPI_Recv(buffer, I.filas()*I.columnas(), MPI_CHAR, dest, MASTER, MPI_COMM_WORLD, &Stat);
			cout<<"Recibida trama"<<endl;
			PaqueteSalida ps;
			ps.buffer = buffer;
			int posBuffer = ps.numeroTrama();
			int tam = ps.tamMensaje();
			memcpy(I.pixels+posBuffer, ps.punteroMensaje(), tam);
		}

		incrustar(I.pixels, size&255);
		incrustar(I.pixels+3,size>>8);

		int R = N;
		int C = size / N;
		int relleno = R * C - size;

		if(relleno){
			C++;
		}

		unsigned char* ptr = (unsigned char*)&R;
		for(int i=0;i<sizeof(int);i++){
			incrustar(I.pixels+6+3*i, ptr[i]);
		}

		ptr = (unsigned char*)&C;
		for(int i=0;i<sizeof(int);i++){
			incrustar(I.pixels+6+3*sizeof(int)+3*i, ptr[i]);
		}


		I.guardaBMP("salida.bmp");

		//Recibir paquetes de regreso
		//Concatenarlos
		//Guardar archivo 
	}
	else{ 	// Else a slave node
		// Wait until a message is there to be received

		char *bufferEntrada = new char[MAX_SIZE];

		
		//Abrir matriz llave
		//pasar el mensaje a la matriz
		//multiplicar los segmentos de la imagen
		//Pasar el resultado a la cadena
		//incrustar el mensaje
		//enviar paquete
		// Get the message and put it in 'inMsg'
		rc = MPI_Recv(bufferEntrada, MAX_SIZE, MPI_CHAR, 0, TAG_GENERAL, MPI_COMM_WORLD, &Stat);

		PaqueteEntrada paq;
		paq.buffer = (unsigned char*)bufferEntrada;
		int tamMensaje = paq.tamMensaje();
		imagen I;
		I.pixels = (color*)paq.punteroImagen();
		I.y = paq.tamImagenX();
		I.x = paq.tamImagenY();

		int numTrama = paq.numeroTrama();
		printf("Se recibio el paquete en %d\n",numTrama);
		// Get how big the message is and put it in 'count'

		Matriz llave = new double[N*N];
		Matriz mensaje;
		int R, C;
		cargaLLave(llave, NOM_MAT);
		crearMatrizTexto(mensaje,(char*)paq.punteroMensaje(), tamMensaje, R, C);
		rc = MPI_Get_count(&Stat, MPI_CHAR, &count);
		Matriz cifrado = new double[R*C];

		int totalTramas = paq.totalTramas();

		int Ri = R/totalTramas*(numTrama-1);
		int Rf = Ri+R/totalTramas;
		if(numTrama == totalTramas){
			Rf = R;
		}
		for(int i=Ri;i<Rf;i++){
			for(int j=0;j<C;j++){
				cifrado[i*C+j]=0;
				for(int k=0;k<N;k++){
					cifrado[i*C+j]+=llave[i*C+k]+mensaje[k*C+j];
				}
				cout<<cifrado[i*C+j]<<" ";
			}
			cout<<endl;
		}

		unsigned char *ptrMsjI = (unsigned char*)(cifrado+Ri*C);
		unsigned char *ptrMsjF = (unsigned char*)(cifrado+Ri*C);

		color* memoria = I.pixels + (2+sizeof(double)*Ri*C+sizeof(int)*2)*3;
		color* ptrPaquete = memoria;
		for(unsigned char * i = ptrMsjI; i < ptrMsjF; i++){
			incrustar(memoria, *i);
			memoria+=3;
		}

		PaqueteSalida ps(memoria-ptrPaquete, ptrPaquete-I.pixels);
		ps.guardaMensaje((unsigned char*)ptrMsjI);

		cout<<"El proceso "<<numTrama<<" ha terminado y enviado resultado"<<endl;

		rc = MPI_Send(ps.buffer, sizeof(ps.buffer), MPI_CHAR, MASTER, MASTER, MPI_COMM_WORLD);


		I.pixels = NULL;
	}

	MPI_Finalize();
	return 0;
}