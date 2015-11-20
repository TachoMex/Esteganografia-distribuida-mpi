#ifndef MATRIX_H
#define MATRIX_H 

#include <tbb/tbb.h>
#include <iostream>
using namespace std;
using namespace tbb;

const int MAX_V = 10;


typedef concurrent_vector<concurrent_vector<double>> Matriz;

void matrizAleatoria(Matriz& m){
	parallel_for(0,(int)m.size(),[&](int i){
		for(int j=0;j<m[i].size();j++){
			m[i][j] = random() % MAX_V;
		}
	});
}

void matrizIdentidad(Matriz& m){
	parallel_for(0,(int)m.size(), [&](int i){
		for(int j=0;j<m[i].size();j++){
			m[i][j] = i == j;
		}
	});
}

void multiplicaRenglon(Matriz& m, int r, double k){
	parallel_for(0,(int)m[r].size(),[&](int i){
		m[r][i]*=k;
	});
}

void sumaRenglon(Matriz& a, Matriz& b, int i, int j, double x){
	parallel_for(0,(int)a[i].size(),[&](int k){
		a[i][k]+=b[j][k]*x;
	});
}

void copia(Matriz& a, Matriz& b){
	parallel_for(0, (int) a.size(), [&](int i){
		for(int j=0;j<a[i].size();j++){
			a[i][j] = b[i][j];
		}
	});
}

void matrizInversa(Matriz& a, Matriz& r){
	matrizIdentidad(r);
	Matriz m(a.size(), concurrent_vector<double>(a.size()));
	copia(m,a);
	//muestra(m, r);
	for(int i=0;i<a.size();i++){
		if(m[i][i] == 0){

		}
		multiplicaRenglon(r,i, 1.0 / m[i][i]);
		multiplicaRenglon(m,i, 1.0 / m[i][i]);
	//	muestra(m, r);
		for(int j=i+1; j<a.size(); j++){
			sumaRenglon(r, r, j, i, -m[j][i]);
			sumaRenglon(m, m, j, i, -m[j][i]);
	//		muestra(m, r);
		}
	}
	for(int i=a.size()-1;i>=0;i--){
		for(int j=i-1; j>=0; j--){
			sumaRenglon(r, r, j, i, -m[j][i]);
			sumaRenglon(m, m, j, i, -m[j][i]);
	//		muestra(m, r);
		}
	}
}

void multiplica(Matriz& a, Matriz& b,Matriz& c){
	int n = a.size(), m = b[0].size(), o = b.size();
	parallel_for(0,n,[&](int i){
		for(int j=0;j<m;j++){
			c[i][j] = 0;
			for(int k=0;k<o;k++){
				c[i][j] += a[i][k] * b[k][j];
			}
		}
	});
}


void muestra(const Matriz& m){
	for(int i=0;i<m.size();i++){
		for(int j=0;j<m.size();j++){
			printf("%d ",(int)floor(m[i][j]+0.5));
		}
		cout<<endl;
	}
	cout<<endl;
}
void muestra(const Matriz& a,const Matriz& b){
	for(int i=0;i<a.size();i++){
		for(int j=0;j<a.size();j++){
			printf("%+1.03lf ",a[i][j]);
		}
		cout<<" | ";
		for(int j=0;j<a.size();j++){
			printf("%+1.03lf ",b[i][j]);
		}
		cout<<endl;
	}
	cout<<endl;
}
#endif