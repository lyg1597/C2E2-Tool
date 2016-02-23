/* CAPD C++ file generated Automatically from HyLink */
 #include <iostream> 
 #include "capd/capdlib.h" 
 using namespace std; 
 using namespace capd; 
 
int getNextMode(int curMode, interval curModeTime);
main(){ 
 
  cout.precision(10);
  try{ 
 /* Differential equation for mode VO Testing */ 
    IMap mode1("var:x1,x2,x3,x4,x5,x6,x7;fun:1.4*x3-0.9*x1,2.5*x5-1.5*x2,0.6*x7-0.8*x3*x2,2.0-1.3*x4*x3,0.7*x1-1.0*x4*x5,0.3*x1-3.1*x6,1.8*x6-1.5*x7*x2;");
    ITaylor* solvers[] = {
     new ITaylor(mode1,5),
    };
    double initialState[7];
    IVector IState(7);
    int i;
    double absErr, relErr;
    double tstep,Gt;
    double curTime;
    int curMode, nextMode;
    std::cin >> curTime;
    for(i=0;i<7;i++){
      std::cin >> initialState[i]; IState[i] = initialState[i];
    }
    std::cin >> absErr >> relErr >> tstep >> Gt;
    std::cin >> curMode;
    nextMode = curMode;
    IVector SimState(7);
    IVector PrevSimState(7);
    PrevSimState = IState;
    interval tstepi(tstep);
    interval Gti(Gt);
    for(i=0;i<1;i++)
      solvers[i]->setStep(tstepi);
    interval currTime(curTime), currModeTime(0.0);
    C0HORect2Set SimSet(IState);
    while(currTime < Gti){
      std::cout << " " << currTime.leftBound();
      for(i=0;i<7;i++){
        std::cout << " " << PrevSimState[i].leftBound();
      }
      std::cout << endl;
      SimSet.move(*solvers[curMode-1]);
      SimState = (IVector)SimSet;
      currTime+=tstepi;currModeTime+=tstepi;
      std::cout << " " << currTime.leftBound();
      for(i=0;i<7;i++){
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
