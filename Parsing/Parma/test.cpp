#include <ppl.hh>
#include <iostream>
#include <fstream>

using namespace std;

void Parma_Polyhedra_Library::initialize();
//void Parma_Polyhedra_Library::restore_pre_PPL_rounding();

int main()
{
  filePoints.fopen("unsafeSetPoints.dat");

  Variable x(0);
  Variable y(1);
  Variable z(2);

  //Variables to remove
  Variables_Set vars;
  vars.insert(z);
  
  Constraint_System cs;
  cs.insert(x>=-3);
  cs.insert(x+-1*z<=1);
  cs.insert(x+y<=1);
  cs.insert(x-z<=1);
  cs.insert(x-y<=1);
  NNC_Polyhedron poly(cs);

  poly.remove_space_dimensions(vars);

  Generator_System gs=poly.minimized_generators();  
  Generator_System::const_iterator i;

  //This goes over every generator (/point) to get the convex hull and writes it to a file 
  for(i=gs.begin();i!=gs.end();++i)
  {
    if((*i).is_point())
    {
      double divisor=mpz_get_d((*i).divisor().get_mpz_t());
      cout<<"Divisor: "<<divisor<<endl;
      int dim=int((*i).space_dimension());
      for(int j=0;j<dim;j++)
      {
        cout<<(*i).coefficient(Variable(j))<<" ";
        //double dividend=mpz_get_d((*i).coefficient(Variable(j)).get_mpz_t());
        filePoints<<(*i).coefficient(Variable(j))<<"/"<<divisor<<" "; 
      }
      cout<<endl;
    }
  }

  return 0;
}
