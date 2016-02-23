/* CAPD C++ file generated Automatically from HyLink */
 #include <iostream> 
 #include "capd/capdlib.h" 
 using namespace std; 
 using namespace capd; 
 
int getNextMode(int curMode, interval curModeTime);
main(){ 
 
  cout.precision(10);
  try{ 
 /* Differential equation for mode SlowDown Testing */ 
    IMap mode1("var:sx,vx,ax,vy,omega,sy;fun:vx-2.5,0.1*ax,-0.01*sx-0.01*10.3+0.3*2.8-0.3*vx-0.5*ax,-2*vy,-2*omega,0.1*vy;");
 /* Differential equation for mode StartTurn1 Testing */ 
    IMap mode2("var:sx,vx,ax,vy,omega,sy;fun:vx-2.5,0.1*ax,-0.5*vx+1.4-0.5*ax,2.5*3-0.15*3*omega+0.5-0.025*sy-0.05*vy,3-3*0.05*omega+0.2-0.01*sy,0.1*vy;");
 /* Differential equation for mode EndTurn1 Testing */ 
    IMap mode3("var:sx,vx,ax,vy,omega,sy;fun:vx-2.5,0.1*ax,-0.5*vx+1.4-0.5*ax,-0.1*2.5*omega+0.5-0.025*sy-0.05*vy,-0.1*omega+0.2-0.01*sy,0.1*vy;");
 /* Differential equation for mode EndTurn2 Testing */ 
    IMap mode4("var:sx,vx,ax,vy,omega,sy;fun:vx-2.5,0.1*ax,-0.5*vx+1.4-0.5*ax,-0.1*2.5*omega+0.5-0.025*sy-0.05*vy,-0.1*omega+0.2-0.01*sy,0.1*vy;");
 /* Differential equation for mode StartTurn2 Testing */ 
    IMap mode5("var:sx,vx,ax,vy,omega,sy;fun:vx-2.5,0.1*ax,-0.5*vx+1.4-0.5*ax,-2.5*3-0.15*3*omega+0.5-0.025*sy-0.05*vy,-3-3*0.05*omega+0.2-0.01*sy,0.1*vy;");
 /* Differential equation for mode SpeedUp Testing */ 
    IMap mode6("var:sx,vx,ax,vy,omega,sy;fun:vx-2.5,0.1*ax,-0.01*sx+0.01*10.3+0.3*2.8-0.3*vx-0.5*ax,-2*vy,-2*omega,0.1*vy;");
 /* Differential equation for mode Continue Testing */ 
    IMap mode7("var:sx,vx,ax,vy,omega,sy;fun:vx-2.5,0.1*ax,-0.5*vx+1.4-0.5*ax,-2*vy,-2*omega,0.1*vy;");
    ITaylor* solvers[] = {
     new ITaylor(mode1,5),
     new ITaylor(mode2,5),
     new ITaylor(mode3,5),
     new ITaylor(mode4,5),
     new ITaylor(mode5,5),
     new ITaylor(mode6,5),
     new ITaylor(mode7,5),
    };
    double initialState[6];
    IVector IState(6);
    int i;
    double absErr, relErr;
    double tstep,Gt;
    double curTime;
    int curMode, nextMode;
    std::cin >> curTime;
    for(i=0;i<6;i++){
      std::cin >> initialState[i]; IState[i] = initialState[i];
    }
    std::cin >> absErr >> relErr >> tstep >> Gt;
    std::cin >> curMode;
    nextMode = curMode;
    IVector SimState(6);
    IVector PrevSimState(6);
    PrevSimState = IState;
    interval tstepi(tstep);
    interval Gti(Gt);
    for(i=0;i<7;i++)
      solvers[i]->setStep(tstepi);
    interval currTime(curTime), currModeTime(0.0);
    C0HORect2Set SimSet(IState);
    while(currTime < Gti){
      std::cout << " " << currTime.leftBound();
      for(i=0;i<6;i++){
        std::cout << " " << PrevSimState[i].leftBound();
      }
      std::cout << endl;
      SimSet.move(*solvers[curMode-1]);
      SimState = (IVector)SimSet;
      currTime+=tstepi;currModeTime+=tstepi;
      std::cout << " " << currTime.leftBound();
      for(i=0;i<6;i++){
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
