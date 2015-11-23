#include "matrix.h"

#include <fstream>

const int N = 100;
const char NOM_MAT[] = "matriz.txt";

void crearMatrizTexto(Matriz& m, const string& nom_arch){
	ifstream f(nom_arch, ios::binary|ios::ate);
	int size = f.tellg();
	f.seekg(0);
	for(auto& v:m){
		v.clear();
	}
	m.clear();
	int R = N;
	int C = size / N;
	int relleno = R * C - size;

	for(int i=0;i<R;i++){
		m.push_back(concurrent_vector<double>());
		for(int j=0;j<C;j++){
			//cout<<i<<" "<<j<<endl;
			int num = f.get();
			cout<<(char)num;
			if(f.eof()){
				goto finDeArchivo;
			}
			m[i].push_back(num);
		}
	}
	cout<<m.size()<<endl;
	finDeArchivo:
	for(int i=0;i<relleno;i++){
		m[R-1].push_back(' ');
	}
	f.close();
}

void guardaMensajeCifrado(Matriz& m, const string& nom_arch){
	ofstream f(nom_arch);
	int r = m.size();
	int c = m[0].size();
	f.write((char*)&r, sizeof(int));
	f.write((char*)&c, sizeof(int));
	for(int i=0;i<r;i++){
		for(int j=0;j<c;j++){
			f.write((char*)&m[i][j], sizeof(double));
		}
	}
	f.close();
}

void guardaMensajeTexto(Matriz& m, const string& nom_arch){
	ofstream f(nom_arch);
	for(int i=0;i<m.size();i++){
		for(int j=0;j<m[0].size();j++){
			f.put((char)max(0,(int)floor(m[i][j]+0.5)));
			cout<<m[i][j]<<" ";
		}
		cout<<endl;
	}
	f.close();
}

void cargaMensajeCifrado(Matriz& m, const string& nom_arch){
	int R;
	int C;
	ifstream f(nom_arch);
	f.read((char*)&R,sizeof(int));
	f.read((char*)&C,sizeof(int));
	cout<<R<<" "<<C<<endl;
	for(int i=0;i<R;i++){
		m.push_back(concurrent_vector<double>());
		for(int j=0;j<C;j++){
			double t;
			f.read((char*)&t, sizeof(double));
			m[i].push_back(t);
			cout<<t<<" ";
		}
		cout<<endl;
	}
	f.close();
}

int main(int argc, char**argv){
	ifstream f(NOM_MAT);
	Matriz llave = concurrent_vector<concurrent_vector<double>>(N, concurrent_vector<double>(N));
	Matriz llavei = concurrent_vector<concurrent_vector<double>>(N, concurrent_vector<double>(N));
	cout<<"cargando llave..."<<endl;
	for(int i=0;i<N;i++){
		for(int j=0;j<N;j++){
			f>>llave[i][j];
		}
	}
	matrizInversa(llave, llavei);
	Matriz mensajec;
	cargaMensajeCifrado(mensajec, "cifrado2.txt");
	Matriz mensaje = concurrent_vector<concurrent_vector<double>>(N, concurrent_vector<double>(mensajec[0].size()));
	cout<<mensajec.size()<<":"<<mensajec[0].size()<<endl;
	cout<<"Recuperando mensaje..."<<endl;
	multiplica(llavei,mensajec, mensaje);
	cout<<"Guardando mensaje recuperado..."<<endl;
	guardaMensajeTexto(mensaje, "texto2.txt");
	return 0;
}