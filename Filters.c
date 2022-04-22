#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
int w,h; //fonksiyonlardan return ve parametre olarak kullanýlmamasý için global tanýmlanan deðerler resmin header bilgileri.
int maxGray;
char mNumber[3];

int readPGMFile (char filePath[]);
void writePGMfile(unsigned char **input,char filePath[]);
int averageFiltre(unsigned char **input);
int medianFiltre(unsigned char **input);

int main(){
	unsigned char **resim;
	unsigned char **sonuc_average;
	unsigned char **sonuc_median;
	int i,j;
	char file[20];

	resim=readPGMFile("galaxy.pgm");//okunacak resmin dosya ismi buraya yazýlýr.
	sonuc_average=averageFiltre(resim);
	sonuc_median=medianFiltre(resim);
	writePGMfile(sonuc_average,"galaxy_average.pgm");//okunan resmin averaj filtresi sonucundaki çýktýsýnýn yazýlacaðý dosya ismi buraya yazýlýr.
	writePGMfile(sonuc_median,"galaxy_median.pgm");//okunan resmin medyan filtresi sonucundaki çýktýsýnýn yazýlacaðý dosya ismi buraya yazýlýr.
	
	free(resim);
	free(sonuc_average);
	free(sonuc_median);
	
	return 0;
}

int readPGMFile (char filePath[]){ //resmi okuyan fonksiyon
		
	char ch;
	FILE *fp;
	int i,j,val;
	unsigned char **picture;
	unsigned char **ptr;
	
	fp = fopen(filePath, "rb");
	fgets(mNumber, 3, fp); // bu satýrda resmin ilk header bilgisi olan mod okunur
	while(getc(fp)!='\n'); // bu kod parçasý comment satýrý varsa atlamak için
		while(getc(fp)=='#'){
			while(getc(fp)!='\n');
		}
		fseek(fp, -1,SEEK_CUR);
	
	fscanf(fp, "%d %d",&w, &h); //diðer header bilgileri okunur
	fscanf(fp, "%d",&maxGray);
	
	picture = (unsigned char **)malloc (sizeof(unsigned char*)*h); //height ve width bilgileri ile uygun matris allocasyonu yapýlýr.
	for(i=0;i<h;i++){
		picture[i]=(unsigned char*)malloc(sizeof(unsigned char)*w);
	}
	
	if(picture == NULL){
		printf("Malloc error");
		exit(1);
	}
	
	if(!strcmp(mNumber, "P2")){ //P2 ve P5 modlarýna ayrý ayrý iþlem yapýlarak resim matrise aktarýlýr.
		for(i=0;i<h;i++){
			for(j=0;j<w;j++){
				fscanf(fp, "%i", &val);
				picture[i][j]=val;
			}
		}
	}else{
		for(i=0;i<h;i++){
			for(j=0;j<w;j++){
				fscanf(fp, "%c", &ch);
				picture[i][j]=ch;
			}
		}
	}
	
	fclose(fp);
	return picture;
}

void writePGMfile(unsigned char **input,char filePath[]){//resmi dosyaya yazan fonksiyon.
	FILE *fp;
	int width,height,grayScale,i,j;
	
	if(!strcmp(mNumber,"P2")){ //okumada olduðu gibi burada da P2 ve P5 modlarý için ayrý iþlemler yapýlýr.
		fp = fopen(filePath,"w");//P2 olduðunda w modunda açýlýr ve decimal deðerler dosyaya headerarý ile birlikte yazýlýr.
		fputs(mNumber,fp);
		fprintf(fp,"\n%d %d\n",w,h);
		fprintf(fp,"%d\n",maxGray);
		for(i=0;i<h;i++){
			for(j=0;j<w;j++){
			fprintf(fp,"%d ",input[i][j]);
				}	
			}
	}else{
		fp = fopen(filePath,"wb");//P5 olduðunda ise wb modunda açýlýr ve character olarak yazýlýr.
		fputs(mNumber,fp);
		fprintf(fp,"\n%d %d\n",w,h);
		fprintf(fp,"%d\n",maxGray);
		for(i=0;i<h;i++){
			for(j=0;j<w;j++){
				fprintf(fp,"%c",input[i][j]);
			}
		}
	}
}

int averageFiltre(unsigned char **input){//3x3 averaj filtresi uygulayan fonksiyon
	
	unsigned char **output;
	int i,j,a,b,toplam;
	
	output = (unsigned char **)malloc (sizeof(unsigned char*)*h); //filtre uygulanan resim baþka bir matrise kaydedilmesi gerektiði için yeni bir matris açýlýr.
	for(i=0;i<h;i++){
		output[i]=(unsigned char*)malloc(sizeof(unsigned char)*w);
	}
	
	for(i=1;i<h-1;i++){//dýþtaki 2 for döngüsü filtrenin büyük resim üzerinde kaydýrýlmasýný saðlar.
		for(j=1;j<w-1;j++){
			toplam=0;
			for(a=0;a<3;a++){//içteki 2 for 3x3 lük filtrenin elemanlarýný toplar.
				for(b=0;b<3;b++){
					toplam += input[(i-1)+a][(j-1)+b];
				}
			}
			output[i-1][j-1]=toplam/9;//ortadaki elemana ortalama deðeri atýlýr.
		}
	}
	
	return output;
}

int medianFiltre(unsigned char **input){//3x3 medyan filtresi uygulayan fonskyion
	unsigned char **output;
	unsigned char medianArray[9];
	int i,j,a,b,m,n,tmp,deneme,index;
	output =(unsigned char **)malloc (sizeof(unsigned char*)*h); //filtre çýktýsýný aktarabilmek için yeni bir matris tanýmlanýr.
	for(i=0;i<h;i++){
		output[i]=(unsigned char*)malloc(sizeof(unsigned char)*w);
	}
	for(i=1;i<h-1;i++){//dýþtaki 2 for resim üzerinde kaydýrma iþlemi yapar.
		for(j=1;j<w;j++){
			index = 0;
			
			for(a=0;a<3;a++){//içteki 2 for filtrenin elemanlarýný bir diziye atar.
				for(b=0;b<3;b++){
					medianArray[index]=input[(i-1)+a][(j-1)+b];
					index++;
				}
			}
			for(m=0;m<8;m++){//dizi bubble sort ile küçükten büyüye sýralanýr 
				for(n=0;n<8-m;n++){
					if(medianArray[n]>medianArray[n+1]){
						tmp = medianArray[n];
						medianArray[n]=medianArray[n+1];
						medianArray[n+1] = tmp;
					}
				}
			}
			output[i-1][j-1]=medianArray[4];//dizideki ortadaki eleman filtrenin ortasýndaki deðer yerine yazýlýr.
		}
	}
	return output;
}




