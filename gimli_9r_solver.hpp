#include <cryptominisat5/cryptominisat.h>
#include <assert.h>
#include <time.h>
#include <vector>

using namespace CMSat;
using std::vector;

void same_ab(SATSolver* solver, int32_t a_index, int32_t b_index, int32_t numofbits);
void read_variable(SATSolver* solver, uint32_t a, uint32_t a_index);
void compute_gimli_and(SATSolver* solver, int32_t ab_index, int32_t a_index, int32_t b_index, int32_t a_shift, int32_t b_shift, int32_t emptybits);
void compute_gimli_or(SATSolver* solver, int32_t ab_index, int32_t a_index, int32_t b_index, int32_t a_shfit, int32_t b_shift, int32_t emptybits);
void compute_ox(SATSolver* solver, int32_t ox_index, int32_t z_index, int32_t y_index, int32_t xy_index);
void compute_oy(SATSolver* solver, int32_t oy_index, int32_t x_index, int32_t y_index, int32_t xz_index);
void compute_oz(SATSolver* solver, int32_t oz_index, int32_t x_index, int32_t z_index, int32_t yz_index);
int Gimli_ref_eq8_sol(vector<uint32_t>* sols, uint32_t given_y, uint32_t given_z, uint32_t given_yp, uint32_t given_zp);
int Gimli_9r_sol_finder_1(vector<uint32_t>* sols, uint32_t given_y, uint32_t given_z, uint32_t given_toy, uint32_t given_toz);
int Gimli_9r_sol_finder_2(vector<uint32_t>* sols, uint32_t given_y, uint32_t given_z, uint32_t given_tox);
int Gimli_9r_sol_finder_3(vector<uint32_t>* sols, uint32_t given_x, uint32_t given_toy, uint32_t given_toz);
int Gimli_9r_sol_finder_4(vector<uint32_t>* sols, uint32_t given_h0, uint32_t given_oy, uint32_t given_oz);
int Gimli_9r_sol_finder_5(vector<uint32_t>* sols, uint32_t given_h0, uint32_t given_ox, uint32_t given_new_ox, uint32_t given_fox);
