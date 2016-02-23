/* CAPD C++ file generated Automatically from HyLink */
 #include <iostream> 
 #include "capd/capdlib.h" 
 using namespace std; 
 using namespace capd; 
 
int getNextMode(int curMode, interval curModeTime);
main(){ 
 
  cout.precision(10);
  try{ 
 /* Differential equation for mode Mode0 Testing */ 
    IMap mode1("var:p,lam,pe,ivalue;fun:0.41328*(2*247*(-2.3421*p*p+2.7799*p-0.3273)-0.9*(-3.66+0.08979*104.71975511*p-0.0337*104.71975511*p*p+0.0001*104.71975511*104.71975511*p)),4*(13.893-35.2518*1*((1/14.7)*(-3.66+0.08979*104.71975511*pe-0.0337*104.71975511*pe*pe+0.0001*pe*104.71975511*104.71975511))+20.7364*1*1*((1/14.7)*(-3.66+0.08979*104.71975511*pe-0.0337*104.71975511*pe*pe+0.0001*pe*104.71975511*104.71975511))*((1/14.7)*(-3.66+0.08979*104.71975511*pe-0.0337*104.71975511*pe*pe+0.0001*pe*104.71975511*104.71975511))+2.6287*(0.9*(-3.66+0.08979*104.71975511*p-0.0337*104.71975511*p*p+0.0001*p*104.71975511*104.71975511))-1.592*(0.9*(-3.66+0.08979*104.71975511*p-0.0337*104.71975511*p*p+0.0001*p*104.71975511*104.71975511))*1*((1/14.7)*(-3.66+0.08979*104.71975511*pe-0.0337*104.71975511*pe*pe+0.0001*pe*104.71975511*104.71975511))-lam),0.41328*(2*1*(247)*(-2.3421*p*p+2.7799*p-0.3273)-(-3.66+0.08979*104.71975511*pe-0.0337*104.71975511*pe*pe+0.0001*pe*104.71975511*104.71975511)),0;");
 /* Differential equation for mode Mode1 Testing */ 
    IMap mode2("var:p,lam,pe,ivalue;fun:0.41328*(2*247.0*(-2.3421*p*p+2.7799*p-0.3273)-0.9*(-0.366+0.08979*104.71975511*p-0.0337*104.71975511*p*p+0.0001*104.71975511*104.71975511*p)),4*(13.893-35.2518*1*((1/14.7)*(1+ivalue+0.04*(1*lam-14.7))*(-0.366+0.08979*104.71975511*pe-0.0337*104.71975511*pe*pe+0.0001*pe*104.71975511*104.71975511))+20.7364*1*1*((1/14.7)*(1+ivalue+0.04*(1*lam-14.7))*(-0.366+0.08979*104.71975511*pe-0.0337*104.71975511*pe*pe+0.0001*pe*104.71975511*104.71975511))*((1/14.7)*(1+ivalue+0.04*(1*lam-14.7))*(-0.366+0.08979*104.71975511*pe-0.0337*104.71975511*pe*pe+0.0001*pe*104.71975511*104.71975511))+2.6287*(0.9*(-0.366+0.08979*104.71975511*p-0.0337*104.71975511*p*p+0.0001*p*104.71975511*104.71975511))-1.592*(0.9*(-0.366+0.08979*104.71975511*p-0.0337*104.71975511*p*p+0.0001*p*104.71975511*104.71975511))*1*((1/14.7)*(1+ivalue+0.04*(1*lam-14.7))*(-0.366+0.08979*104.71975511*pe-0.0337*104.71975511*pe*pe+0.0001*pe*104.71975511*104.71975511))-lam),0.41328*(2*1*(247.0)*(-2.3421*p*p+2.7799*p-0.3273)-(-0.366+0.08979*104.71975511*pe-0.0337*104.71975511*pe*pe+0.0001*pe*104.71975511*104.71975511)),0.14*(1*lam-14.7);");
    ITaylor* solvers[] = {
     new ITaylor(mode1,5),
     new ITaylor(mode2,5),
    };
    double initialState[4];
    IVector IState(4);
    int i;
    double absErr, relErr;
    double tstep,Gt;
    double curTime;
    int curMode, nextMode;
    std::cin >> curTime;
    for(i=0;i<4;i++){
      std::cin >> initialState[i]; IState[i] = initialState[i];
    }
    std::cin >> absErr >> relErr >> tstep >> Gt;
    std::cin >> curMode;
    nextMode = curMode;
    IVector SimState(4);
    IVector PrevSimState(4);
    PrevSimState = IState;
    interval tstepi(tstep);
    interval Gti(Gt);
    for(i=0;i<2;i++)
      solvers[i]->setStep(tstepi);
    interval currTime(curTime), currModeTime(0.0);
    C0HORect2Set SimSet(IState);
    while(currTime < Gti){
      std::cout << " " << currTime.leftBound();
      for(i=0;i<4;i++){
        std::cout << " " << PrevSimState[i].leftBound();
      }
      std::cout << endl;
      SimSet.move(*solvers[curMode-1]);
      SimState = (IVector)SimSet;
      currTime+=tstepi;currModeTime+=tstepi;
      std::cout << " " << currTime.leftBound();
      for(i=0;i<4;i++){
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
