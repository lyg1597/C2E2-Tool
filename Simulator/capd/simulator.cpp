/* CAPD C++ file generated Automatically from HyLink */
 #include <iostream> 
 #include "capd/capdlib.h" 
 using namespace std; 
 using namespace capd; 
 
int getNextMode(int curMode, interval curModeTime);
main(){ 
 
  cout.precision(10);
  try{ 
 /* Differential equation for mode Brussellator Testing */ 
    IMap mode1("var:x,y;fun:1+x*x*y-2.5*x,1.5*x-x*x*y;");
    ITaylor* solvers[] = {
     new ITaylor(mode1,5),
    };
    double initialState[2];
    IVector IState(2);
    int i;
    double absErr, relErr;
    double tstep,Gt;
    double curTime;
    int curMode, nextMode;
    std::cin >> curTime;
    for(i=0;i<2;i++){
      std::cin >> initialState[i]; IState[i] = initialState[i];
    }
    std::cin >> absErr >> relErr >> tstep >> Gt;
    std::cin >> curMode;
    nextMode = curMode;
    IVector SimState(2);
    IVector PrevSimState(2);
    PrevSimState = IState;
    interval tstepi(tstep);
    interval Gti(Gt);
    for(i=0;i<1;i++)
      solvers[i]->setStep(tstepi);
    interval currTime(curTime), currModeTime(0.0);
    C0HORect2Set SimSet(IState);
    while(currTime < Gti){
      std::cout << " " << currTime.leftBound();
      for(i=0;i<2;i++){
        std::cout << " " << PrevSimState[i].leftBound();
      }
      std::cout << endl;
      SimSet.move(*solvers[curMode-1]);
      SimState = (IVector)SimSet;
      currTime+=tstepi;currModeTime+=tstepi;
      std::cout << " " << currTime.leftBound();
      for(i=0;i<2;i++){
        std::cout << " " << SimState[i].leftBound();
      }
      std::cout << endl;
      PrevSimState = SimState;
      nextMode = getNextMode(curMode,currModeTime);
      if(nextMode != curMode){
        curMode = nextMode; currModeTime = 0.0;
      }
    }
  }catch(exception& e){
    cout << "Exception caught!" << e.what() << endl << endl;
  }
}

int getNextMode(int curMode, interval curModeTime){
  return curMode;
}
