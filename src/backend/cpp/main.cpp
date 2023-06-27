#include "model.hpp"
#define SIMU 1
#define VERI 0

int main()
{
    Model cpp_Model;

    double initial_matrix[]={0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 10.0, 0.0, 0.0, 0.0, 0.0, 0.0, -10.0, 10.0, 0.0, 0.0, 0.0, 0.0, 0.0, -10.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 10.0, 0.0, 0.0, 0.0, 0.0, 0.0, -10.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0};
    double initial_b[]={0.0, 0.0, 10.0, -10.0, -8750.0, 9250.0, -3750.0, 4250.0, 0.0, 0.0, 0.0, 0.0};
    double unsafe_marix[]={0.0, 0.0, 0.0, 0.0, 0.0, 10.0, 0.0, 0.0, 0.0, 0.0, 0.0, -10.0, 10.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    double unsafe_b[]={30.0, -30.0, -1000.0};    
    
    cpp_Model.dimensions=6;
    cpp_Model.modes=3;
    cpp_Model.initial_mode=1;
    cpp_Model.initial_eqns=12;
    cpp_Model.unsafe_eqns=3;
    cpp_Model.annot_type=3;
    
    cpp_Model.refine=0;
    cpp_Model.simu_flag=VERI;
    
    vector<int> mode_linear;
    for(int i=0;i<3;i++)
    {
        mode_linear.push_back(1);
    }
    cpp_Model.mode_linear=mode_linear;
    
    cpp_Model.abs_err=0.0000000001;
    cpp_Model.rel_err=0.000000001;
    cpp_Model.delta_time=0.1;
    cpp_Model.end_time=200.0;
    
    vector<double> gammas;
    gammas.push_back(-0.020000260873814596);
    gammas.push_back(-0.030000031228404594);
    gammas.push_back(0.0);
    cpp_Model.gammas=gammas;
    vector<double> k_consts;
    k_consts.push_back(4.341086820425038);
    k_consts.push_back(27.24422545275809);
    k_consts.push_back(1.4142678583783848);
    cpp_Model.k_consts=k_consts;
    vector<double> initial_matrix_vec;
    for(int i=0;i<72;i++)
    {
        initial_matrix_vec.push_back(initial_matrix[i]);
    }
    cpp_Model.initial_matrix=initial_matrix_vec;
    vector<double> initial_b_vec;
    for(int i=0;i<12;i++)
    {
        initial_b_vec.push_back(initial_b[i]);
    }
    cpp_Model.initial_b=initial_b_vec;
    vector<double> unsafe_matrix_vec;
    for(int i=0;i<18;i++)
    {
        unsafe_matrix_vec.push_back(unsafe_marix[i]);
    }
    cpp_Model.unsafe_matrix=unsafe_matrix_vec;
    vector<double> unsafe_b_vec;
    for(int i=0;i<3;i++)
    {
        unsafe_b_vec.push_back(unsafe_b[i]);
    }
    cpp_Model.unsafe_b=unsafe_b_vec;

    cpp_Model.visualize_filename="../work-dir/LOS1";
    cpp_Model.simulate_verify();

    return 0;
}
