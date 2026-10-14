#include <iostream>
#include <string>
#include <cstdio>
#include "../struct.hpp"

using std::string;
using std::cout;
using std::cin;
using std::endl;
using std::getline;
using std::to_string;

int lenght(string s)
{
    int i=0;
    while(s[i]!='\0')
    {
        i++;
    }
    return i;
}
//-----------------------[Creamos a los mozos]--------------------------
Mozo crearMozo(ComandaHistorica ch,int num, int comision)
{
    Mozo moz;  
                
    moz.idMozo=num+1;
    moz.totalComicion= comision ;

    cout<<"ingrese la clave del mozo: "<<ch.nombreMozo<<"(max=50 caracteres)"<<endl;
    cin>>moz.password;
    int i=0;
    while(moz.password[i] !='\0')
    {
        moz.password[i]+=5;
        i++;
    }
    for(int j=0;j<50;j++)
    {
        moz.nombre[j]=ch.nombreMozo[j];
    }
    return moz;
}
int cmpNombre(char nombre[],char c[])
{
    for(int i=0; i<50;i++)
    {
        if(nombre[i]!=c[i])return 0;
    }
    return 1;
}
int buscarMozo(Mozo mozos[],char c[],int lenm)
{
    for(int i=0; i<lenm;i++)
    {
        if(cmpNombre(mozos[i].nombre,c)==1)
        {
            return i;
        }
    }
    return -1;
}
void archivoMozos(Mozo mozo[],int lenm)
{
    FILE* mozos=fopen("Mozos.dat","wb");
    for(int i=0;i<lenm;i++)
    {
        Mozo m=mozo[i];
        fwrite(&m,sizeof(Mozo),1,mozos);
    }
    
    fclose(mozos);
}
void mostrarMozo(Mozo mozos[],int lenm)
{
    cout<<"=========LISTAS DE MOZO=========="<<endl;
    for(int i=0; i<lenm;i++)
    {
        cout<<"id del Mozo: "<<mozos[i].idMozo<<endl;
        cout<<"Nombre del mozo: "<<mozos[i].nombre<<endl;
        cout<<"password: "<<mozos[i].password<<endl;
        cout<<"Total de comicion: "<<mozos[i].totalComicion<<endl;
        cout<<"-------------------------------------------"<<endl;
    }
}
void crearMozo (string s,Mozo mozos[],ComandaHistorica comandah[],int & lenm,int &lenmch)
{
    FILE* CH = fopen(s.c_str(),"rb");
    if(CH==NULL)
    {
        cout<<"no hay nada"<<endl;
        return;
    }
    //hacemos un array de mozo para poder crear el archivo 
    

    ComandaHistorica ch;
    Mozo moz;
    
    
    int leido=fread(&ch,sizeof(ComandaHistorica),1,CH);
    while(leido==1)
    {
        
        if(lenm==0)
        {
            
            moz=crearMozo(ch,0,ch.comision);
            
            mozos[0]=moz;
            lenm++;
        }
        else
        {
            int pos=buscarMozo(mozos,ch.nombreMozo,lenm);
            if(pos==-1)
            {
                moz=crearMozo(ch,lenm,ch.comision);
                mozos[lenm]=moz;
                lenm++;

            }
            else
            {
                mozos[pos].totalComicion+=ch.comision;
            }
        }
        comandah[lenmch]=ch;
        lenmch++;
        leido=fread(&ch,sizeof(ComandaHistorica),1,CH);
    }
    archivoMozos(mozos,lenm);
    mostrarMozo(mozos,lenm);
    fclose(CH);
    
    
}
//------------------------[Crear las Ventas]------------------------------
void crearNombre(char fecha[], string& nombre)
{
    nombre="comanda_";
    string dat=".dat";

    for(int i=0;i<11;i++)
    {
        nombre+=fecha[i];
    }
    int d=0;
    while(dat[d]!='\0')
    {
        nombre+=dat[d];
        d++;
    }

}
int cmpFecha(char fecha1[],char fecha2[])
{
    int i=0;
    while(fecha1[i]!='\0' && fecha2[i]!='\0')
    {
        if(fecha1[i]>fecha2[i])return 1;

        if(fecha1[i]<fecha2[i])return -1;

        i++;
    }
    return 0;
}
void ordenarVentas(ComandaHistorica ch[],int lench)
{
    for(int i=0;i<lench-1;i++)
    {
        for(int j=0;j<lench-i-1;j++)
        {
            if(cmpFecha(ch[j].fecha,ch[j+1].fecha)>0)
            {
                ComandaHistorica temp=ch[j];
                ch[j]=ch[j+1];
                ch[j+1]=temp;
            }
        }
    }
}
void ComandaVenta(ComandaHistorica ch[],int lench,Mozo mozos[], int len)
{

    ordenarVentas(ch,lench);
    int i=0;
    while(i<lench)
    {
        ComandaHistorica fecha=ch[i];
        string s="";
        crearNombre(fecha.fecha,s);

        FILE* comanda=fopen(s.c_str(),"wb");
        while(i<lench && cmpFecha(fecha.fecha,ch[i].fecha)==0)
        {
            int pos=buscarMozo(mozos,ch[i].nombreMozo,len);
            fwrite(&mozos[pos],sizeof(Mozo),1,comanda);   
            i++;
        }
        fclose(comanda);

    }
}


//-----------------------[Ordenar el inventario]-------------------------------
int stockNuevo(ComandaHistorica ch[],int lench,int cod)
{
    int stock=0;
    for(int i=0;i<lench;i++)
    {
        if(ch[i].codigoProducto==cod)
        {
            stock+=ch[i].cantidad;
        }
    }
    return stock;
}
void ordenarInventario(string inventario,ComandaHistorica ch[],int lench)
{
    FILE* invet=fopen(inventario.c_str(),"rb+");

    if(invet==NULL)return;

    Producto p;


   
    while(fread(&p,sizeof(Producto),1,invet)==1)
    {
        
        int nuevoStock=stockNuevo(ch,lench,p.codigo);

        p.stuckActual-=nuevoStock;
        
        fseek(invet,-(long)sizeof(Producto),SEEK_CUR);
        
        fwrite(&p,sizeof(Producto),1,invet);
        
        fseek(invet,0,SEEK_CUR);
        
    }
    fclose(invet);
     

    
}
void mostrarInventario(string s)
{
    FILE* f=fopen(s.c_str(),"rb");
    Producto p;
    int leido=fread(&p,sizeof(Producto),1,f);
    cout<<"codigo | descripcion | precio | stock actual"<<endl;
    while(leido==1)
    {
        cout<<p.codigo<<" | "<<p.descripcion<<"  "<<p.precio <<"  "<<p.stuckActual<<endl;
        cout<<"---------------------------"<<endl;
        leido=fread(&p,sizeof(Producto),1,f);
    }
    
    fclose(f);

}
int main()
{
 
    Mozo mozos[100];
    int lenm=0;
    ComandaHistorica ch[100];
    int lench=0;
    crearMozo("comandas_historicas.dat",mozos,ch,lenm,lench);
    ComandaVenta(ch,lench,mozos,lenm);
    


    ordenarInventario("inventario.dat",ch,lench);
    mostrarInventario("inventario.dat");
    return 0;
}