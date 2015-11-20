#include "imagen.h"
#include <iostream>
#include <vector>
#include <string>
using namespace std;

void incrustar(vector<color>& v, unsigned char c){
	v[0].r=bool(c&128)+(v[0].r&254);
	v[0].g=bool(c&64)+ (v[0].g&254);
	v[0].b=bool(c&32)+ (v[0].b&254);
	v[1].r=bool(c&16)+ (v[1].r&254);
	v[1].g=bool(c&8)+  (v[1].g&254);
	v[1].b=bool(c&4)+  (v[1].b&254);
	v[2].r=bool(c&2)+  (v[2].r&254);
	v[2].g=bool(c&1)+  (v[2].g&254);
} 

unsigned char extraer(vector<color> v){
	unsigned char c=0;
	c+=(v[0].r&1)*128;
	c+=(v[0].g&1)*64;
	c+=(v[0].b&1)*32;
	c+=(v[1].r&1)*16;
	c+=(v[1].g&1)*8;
	c+=(v[1].b&1)*4;
	c+=(v[2].r&1)*2;
	c+=(v[2].g&1)*1;
	return c;
}

void incrustar(imagen& I,const unsigned char *str,const unsigned char *end){
	int size = (int)(end-str);
	int contTam = 0;
	for(int i=0;i<I.filas() and str!=end or contTam<16;i++){
		for(int j=2;j<I.columnas() and str!=end or contTam<16;j+=3){
			if(str!=end){
				vector<color> v(3);
				v[0]=I.en(i,j-2);
				v[1]=I.en(i,j-1);
				v[2]=I.en(i,j);
				incrustar(v,*str++);
				I.en(i,j-2)=v[0];
				I.en(i,j-1)=v[1];
				I.en(i,j)=v[2];
			}
			if(contTam<16){
				bool tamDig = size&(1<<contTam++);
				I.en(i,j).b= tamDig + (I.en(i,j).b&254);;
			}
		}
	}
}

string extraer(imagen& I){
	string s="";
	int size=0;
	int contTam = 0;
	for(int i=0;i<I.filas();i++){
		for(int j=2;j<I.columnas();j+=3){
			vector<color> v(3);
			v[0]=I.en(i,j-2);
			v[1]=I.en(i,j-1);
			v[2]=I.en(i,j);
			s+=extraer(v);
			if(contTam<16){
				size+=bool(I.en(i,j).b&1)<<contTam++;
			}
		}
	}
	return s.substr(0,size);
}

int main(int argc, char **argv){
	imagen I;
	if(argc>4){
		if(string("-i")==argv[1]){
			I.leerBMP(argv[2]);
			ifstream ifs(argv[3]);
			string msj((istreambuf_iterator<char>(ifs)),(istreambuf_iterator<char>()));	
			incrustar(I,(const unsigned char*)msj.c_str(),(const unsigned char*)msj.c_str()+msj.size());
			I.guardaBMP(argv[4]);
		}
	}else if(argc>3){
		if(string("-e")==argv[1]){
			I.leerBMP(argv[2]);
			ofstream salida(argv[3]);
			salida<<extraer(I);
			salida.close();
		}		
	}
	return 0;
}