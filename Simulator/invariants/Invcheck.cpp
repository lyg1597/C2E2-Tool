/* CAPD file which determines invariant satisfaction for the rectangles */
 #include <iostream> 
 #include "capd/capdlib.h" 
 using namespace std; 
 using namespace capd; 
 main(int argc, char* argv[]){ 
  int curMode;
  double bufferReader;
  double scanLB[5];
  double scanUB[5];
  IVector x(5);
  IVector Range(5);

  FILE* reader;
  reader = fopen("reachtube.dat","r");

  FILE* writer;
  writer = fopen("invariant.dat","w");

  fscanf(reader,"%d",&curMode);

  while( fscanf(reader,"%lf",&bufferReader) != EOF){

    scanLB[0] = bufferReader;
    for(int j=0; j<4; j++){
      fscanf(reader,"%lf",&bufferReader);
      scanLB[j+1] = bufferReader;
    }

    for(int j=0; j<5; j++){
      fscanf(reader,"%lf",&bufferReader);
      scanUB[j] = bufferReader;
    }

    for(int j=0; j<5; j++){
      Range[j].setLeftBound(scanLB[j]);
      Range[j].setRightBound(scanUB[j]);
    }

    x = Range;

    if(curMode == 1){
      interval RHSGE1;
      interval LHSGE1;
      interval evalE1;
      bool SATE1;
      bool SATEAND1;
      LHSGE1 = x[0];
      RHSGE1 = 9.5;
      evalE1 = LHSGE1 - RHSGE1;
      SATE1 = not (evalE1 > 0);
      SATEAND1 = SATE1 || false;
      if( SATEAND1 && true){
        for(int k=0; k<5;k++){
          fprintf(writer," %lf",Range[k].leftBound());
        }
        fprintf(writer," \n");
        for(int k=0; k<5;k++){
          fprintf(writer," %lf",Range[k].rightBound());
        }
        fprintf(writer," \n");
      }
    }
    if(curMode == 2){
      if( true){
        for(int k=0; k<5;k++){
          fprintf(writer," %lf",Range[k].leftBound());
        }
        fprintf(writer," \n");
        for(int k=0; k<5;k++){
          fprintf(writer," %lf",Range[k].rightBound());
        }
        fprintf(writer," \n");
      }
    }
  }
  fclose(reader);
  fclose(writer);
 }
