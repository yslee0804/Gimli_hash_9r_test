#include <cryptominisat5/cryptominisat.h>
#include <assert.h>
#include <time.h>
#include <vector>
#include "gimli_9r_solver.hpp"

using namespace CMSat;
using std::vector;

/*
	Here is functions for solving problems of Gimli
	Make cnf clauses and add to SAT solver
*/

/*
	Add cnf clauses such that two variable 'a' and 'b' have same bits to SAT solver
	a_index, b_index	: assigned number of variable 'a' and 'b'
	numofbits	: the number of same bits of 'a' and 'b'(lsb, 0<=numofbits<=32)
*/
void same_ab(SATSolver* solver, int32_t a_index, int32_t b_index, int32_t numofbits)
{
	vector<Lit> clause;
	if (numofbits == 0) { return; }

	for (int i = 0; i < numofbits; i++)
	{
		// (-1 2 0) (1 -2 0)
		clause.push_back(Lit(a_index + i, true));
		clause.push_back(Lit(b_index + i, false));
		solver->add_clause(clause);
		clause.clear();
		clause.push_back(Lit(a_index + i, false));
		clause.push_back(Lit(b_index + i, true));
		solver->add_clause(clause);
		clause.clear();
	}
}

/*
	read 32-bit variable and transform to cnf clauses
	a	: 32-bit variable
	a_index	: assigned number of variable 'a' (lsb)
*/
void read_variable(SATSolver* solver, uint32_t a, uint32_t a_index)
{
	vector<Lit> clause;

	for (int i = 0; i < 32; i++)
	{
		if ((a >> i) & 1)
			clause.push_back(Lit(a_index + i, false)); // (1 0)
		else
			clause.push_back(Lit(a_index + i, true)); // (-1 0)
		solver->add_clause(clause);
		clause.clear();
	}
}

/*
	Add cnf clauses which 'a' & 'b' = 'ab'
    in Gimli SP operation, rotate 'a' and 'b' then excute 'and' operation
    So, we need additional variable how many rotate that variables
    Also in Gimli SP operation, some bits of 'a&b' does not use
    So, that bit variables set to 0
    
	a_index     : assigned number of variable 'a'  
	b_index     : assigned number of variable 'b'  
    ab_index    : assigned number of variable 'a&b' 
    a_shift     : rotate bit number of variable 'a'
    b_shift     : rotate bit number of variable 'b'
    emptybits   : number of not using bits of 'a&b'    
*/
void compute_gimli_and(SATSolver* solver, int32_t ab_index, int32_t a_index, int32_t b_index, int32_t a_shift, int32_t b_shift, int32_t emptybits)
{
	vector<Lit> clause;
	int i;

	for (i = 0; i < emptybits; i++) // set the not using index to 0
	{
		clause.push_back(Lit(ab_index + i, true));
		solver->add_clause(clause);
		clause.clear();
	}

	for (; i < 32; i++)
	{
		// (-1 -2 3) (2 -3) (1 -3)
		clause.push_back(Lit(a_index + ((i + a_shift) & 0x1f), true));
		clause.push_back(Lit(b_index + ((i + b_shift) & 0x1f), true));
		clause.push_back(Lit(ab_index + i, false));
		solver->add_clause(clause);
		clause.clear();
		clause.push_back(Lit(b_index + ((i + b_shift) & 0x1f), false));
		clause.push_back(Lit(ab_index + i, true));
		solver->add_clause(clause);
		clause.clear();
		clause.push_back(Lit(a_index + ((i + a_shift) & 0x1f), false));
		clause.push_back(Lit(ab_index + i, true));
		solver->add_clause(clause);
		clause.clear();
	}
}

/*
	similar to "compute_gimli_and" function. see above
*/
void compute_gimli_or(SATSolver* solver, int32_t ab_index, int32_t a_index, int32_t b_index, int32_t a_shift, int32_t b_shift, int32_t emptybits)
{
	vector<Lit> clause;
	int i;

	for (i = 0; i < emptybits; i++)
	{
		clause.push_back(Lit(ab_index + i, true));
		solver->add_clause(clause);
		clause.clear();
	}

	for (; i < 32; i++)
	{
		// (1 2 -3) (-2 3) (-1 3)
		clause.push_back(Lit(a_index + ((i + a_shift) & 0x1f), false));
		clause.push_back(Lit(b_index + ((i + b_shift) & 0x1f), false));
		clause.push_back(Lit(ab_index + i, true));
		solver->add_clause(clause);
		clause.clear();
		clause.push_back(Lit(b_index + ((i + b_shift) & 0x1f), true));
		clause.push_back(Lit(ab_index + i, false));
		solver->add_clause(clause);
		clause.clear();
		clause.push_back(Lit(a_index + ((i + a_shift) & 0x1f), true));
		clause.push_back(Lit(ab_index + i, false));
		solver->add_clause(clause);
		clause.clear();
	}
}

/*
	compute 'ox' variable of Gimli SP
    ox_i = z_i ^ y_(i+23) ^ (x_(i+5) & y_(i+20))

    ox_index    : assigned number of variable output 'ox'  
    z_index     : assigned number of variable input 'z'
    y_index     : assigned number of variable input 'y'
    xy_index    : assigned number of variable input 'x_i+5 & y_i+20'
                  rotate and shift already applied in the function 'compute_gimli_and'
*/
/*
	Question. Using "solver.add_xor_clauses" is better?
*/
void compute_ox(SATSolver* solver, int32_t ox_index, int32_t z_index, int32_t y_index, int32_t xy_index)
{
	vector<Lit> clause;
	for (int i = 0; i < 32; i++)
	{
		clause.push_back(Lit(z_index + i, false));
		clause.push_back(Lit(y_index + ((i + 23) & 0x1f), false));
		clause.push_back(Lit(xy_index + i, false));
		clause.push_back(Lit(ox_index + i, true));
		solver->add_clause(clause);
		clause[3] = Lit(ox_index + i, false);
		clause[2] = Lit(xy_index + i, true);
		solver->add_clause(clause);
		clause[2] = Lit(xy_index + i, false);
		clause[1] = Lit(y_index + ((i + 23) & 0x1f), true);
		solver->add_clause(clause);
		clause[1] = Lit(y_index + ((i + 23) & 0x1f), false);
		clause[0] = Lit(z_index + i, true);
		solver->add_clause(clause);

		clause[1] = Lit(y_index + ((i + 23) & 0x1f), true);
		clause[2] = Lit(xy_index + i, true);
		solver->add_clause(clause);
		clause[3] = Lit(ox_index + i, true);
		clause[2] = Lit(xy_index + i, false);
		solver->add_clause(clause);
		clause[2] = Lit(xy_index + i, true);
		clause[1] = Lit(y_index + ((i + 23) & 0x1f), false);
		solver->add_clause(clause);
		clause[1] = Lit(y_index + ((i + 23) & 0x1f), true);
		clause[0] = Lit(z_index + i, false);
		solver->add_clause(clause);
		clause.clear();
	}
}

/*
	simliar to 'compute_ox' function
	oy_i = x_(i+8) ^ y_(i+23) ^ (x_(i+7) | z_(i+31))
*/
void compute_oy(SATSolver* solver, int32_t oy_index, int32_t x_index, int32_t y_index, int32_t xz_index)
{
	vector<Lit> clause;
	for (int i = 0; i < 32; i++)
	{
		clause.push_back(Lit(x_index + ((i + 8) & 0x1f), false));
		clause.push_back(Lit(y_index + ((i + 23) & 0x1f), false));
		clause.push_back(Lit(xz_index + i, false));
		clause.push_back(Lit(oy_index + i, true));
		solver->add_clause(clause);
		clause[3] = Lit(oy_index + i, false);
		clause[2] = Lit(xz_index + i, true);
		solver->add_clause(clause);
		clause[2] = Lit(xz_index + i, false);
		clause[1] = Lit(y_index + ((i + 23) & 0x1f), true);
		solver->add_clause(clause);
		clause[1] = Lit(y_index + ((i + 23) & 0x1f), false);
		clause[0] = Lit(x_index + ((i + 8) & 0x1f), true);
		solver->add_clause(clause);

		clause[1] = Lit(y_index + ((i + 23) & 0x1f), true);
		clause[2] = Lit(xz_index + i, true);
		solver->add_clause(clause);
		clause[3] = Lit(oy_index + i, true);
		clause[2] = Lit(xz_index + i, false);
		solver->add_clause(clause);
		clause[2] = Lit(xz_index + i, true);
		clause[1] = Lit(y_index + ((i + 23) & 0x1f), false);
		solver->add_clause(clause);
		clause[1] = Lit(y_index + ((i + 23) & 0x1f), true);
		clause[0] = Lit(x_index + ((i + 8) & 0x1f), false);
		solver->add_clause(clause);
		clause.clear();
	}
}


/*
	simliar to 'compute_ox' function
	oz_i = x_(i+8) ^ z_(i+31) ^ (y_(i+21) & z_(i+30)) except oz_0 = x_8
*/
void compute_oz(SATSolver* solver, int32_t oz_index, int32_t x_index, int32_t z_index, int32_t yz_index)
{
	vector<Lit> clause;

	clause.push_back(Lit(x_index + 8, false));
	clause.push_back(Lit(oz_index, true));
	solver->add_clause(clause);
	clause[0] = Lit(x_index + 8, true);
	clause[1] = Lit(oz_index, false);
	solver->add_clause(clause);

	clause.clear();

	for (int i = 1; i < 32; i++)
	{
		clause.push_back(Lit(x_index + ((i + 8) & 0x1f), false));
		clause.push_back(Lit(z_index + ((i + 31) & 0x1f), false));
		clause.push_back(Lit(yz_index + i, false));
		clause.push_back(Lit(oz_index + i, true));
		solver->add_clause(clause);
		clause[3] = Lit(oz_index + i, false);
		clause[2] = Lit(yz_index + i, true);
		solver->add_clause(clause);
		clause[2] = Lit(yz_index + i, false);
		clause[1] = Lit(z_index + ((i + 31) & 0x1f), true);
		solver->add_clause(clause);
		clause[1] = Lit(z_index + ((i + 31) & 0x1f), false);
		clause[0] = Lit(x_index + ((i + 8) & 0x1f), true);
		solver->add_clause(clause);

		clause[1] = Lit(z_index + ((i + 31) & 0x1f), true);
		clause[2] = Lit(yz_index + i, true);
		solver->add_clause(clause);
		clause[3] = Lit(oz_index + i, true);
		clause[2] = Lit(yz_index + i, false);
		solver->add_clause(clause);
		clause[2] = Lit(yz_index + i, true);
		clause[1] = Lit(z_index + ((i + 31) & 0x1f), false);
		solver->add_clause(clause);
		clause[1] = Lit(z_index + ((i + 31) & 0x1f), true);
		clause[0] = Lit(x_index + ((i + 8) & 0x1f), false);
		solver->add_clause(clause);
		clause.clear();
	}
}

/*
	Given y,z,y',z', find x such that SP^2(x,y,z)_x = SP^2(x,y',z')_x
	equations from the paper "New results on Gimli: full-permutation distinguishers and improved collisions"
		(equation (8))

	solutions are save in "sols" (save only variable 'x')
	return the number of solutions
*/
int Gimli_ref_eq8_sol(vector<uint32_t>* sols, uint32_t given_y, uint32_t given_z, uint32_t given_yp, uint32_t given_zp)
{
	int x_index			= 0;
	int y_index			= x_index		+ 32;
	int z_index			= y_index		+ 32;
	int yp_index		= z_index		+ 32;
	int zp_index		= yp_index		+ 32;
	int xy_index		= zp_index		+ 32;
	int xz_index		= xy_index		+ 32;
	int yz_index		= xz_index		+ 32;
	int ox_index		= yz_index		+ 32;
	int oy_index		= ox_index		+ 32;
	int oz_index		= oy_index		+ 32;
	int oxoy_index		= oz_index		+ 32;
	int tox_index		= oxoy_index	+ 32;
	int xyp_index		= tox_index		+ 32;
	int xzp_index		= xyp_index		+ 32;
	int ypzp_index		= xzp_index		+ 32;
	int oxp_index		= ypzp_index	+ 32;
	int oyp_index		= oxp_index		+ 32;
	int ozp_index		= oyp_index		+ 32;
	int oxpoyp_index	= ozp_index		+ 32;
	int toxp_index		= oxpoyp_index	+ 32;

	SATSolver solver;
	int num_of_sol = 0;

	solver.new_vars(toxp_index + 32);

	read_variable(&solver, given_y, y_index);
	read_variable(&solver, given_z, z_index);
	read_variable(&solver, given_yp, yp_index);
	read_variable(&solver, given_zp, zp_index);

	compute_gimli_and(&solver, xy_index, x_index, y_index, 5, 20, 3);
	compute_gimli_or(&solver, xz_index, x_index, z_index, 7, 31, 1);
	compute_gimli_and(&solver, yz_index, y_index, z_index, 21, 30, 2);

	compute_ox(&solver, ox_index, z_index, y_index, xy_index);
	compute_oy(&solver, oy_index, x_index, y_index, xz_index);
	compute_oz(&solver, oz_index, x_index, z_index, yz_index);

	compute_gimli_and(&solver, oxoy_index, ox_index, oy_index, 5, 20, 3);
	compute_ox(&solver, tox_index, oz_index, oy_index, oxoy_index);

	compute_gimli_and(&solver, xyp_index, x_index, yp_index, 5, 20, 3);
	compute_gimli_or(&solver, xzp_index, x_index, zp_index, 7, 31, 1);
	compute_gimli_and(&solver, ypzp_index, yp_index, zp_index, 21, 30, 2);

	compute_ox(&solver, oxp_index, zp_index, yp_index, xyp_index);
	compute_oy(&solver, oyp_index, x_index, yp_index, xzp_index);
	compute_oz(&solver, ozp_index, x_index, zp_index, ypzp_index);

	compute_gimli_and(&solver, oxpoyp_index, oxp_index, oyp_index, 5, 20, 3);
	compute_ox(&solver, toxp_index, ozp_index, oyp_index, oxpoyp_index);

	same_ab(&solver, tox_index, toxp_index, 32);

	/* run solver */
	while (solver.solve() == l_True)
	{
		uint32_t a_sol = 0;
		vector<Lit> ban_solution;
		num_of_sol++;

		for (int i = 0; i < 32; i++)
		{
			if (solver.get_model()[x_index + i] == l_True)
			{
				a_sol |= (1 << i);
				ban_solution.push_back(Lit(x_index + i, true));
			}
			else
			{
				ban_solution.push_back(Lit(x_index + i, false));
			}
		}
		solver.add_clause(ban_solution);
		sols->push_back(a_sol);
		a_sol = 0;
	}
	return num_of_sol;
}

/*
	Given y, z, toy, toz, find solution x, tox such that SP^2(x,y,z) = (tox, toy, toz)

	solutions are save in "sols" (save variables 'x' and 'tox')
	return the number of solutions
*/
int Gimli_9r_sol_finder_1(vector<uint32_t>* sols, uint32_t given_y, uint32_t given_z, uint32_t given_toy, uint32_t given_toz)
{
	int x_index = 0;
	int tox_index	= x_index	 + 32;
	int y_index		= tox_index	 + 32;
	int z_index		= y_index	 + 32;
	int toy_index	= z_index	 + 32;
	int toz_index	= toy_index	 + 32;
	int xy_index	= toz_index	 + 32;
	int yz_index	= xy_index	 + 32;
	int xz_index	= yz_index	 + 32;
	int ox_index	= xz_index	 + 32;
	int oy_index	= ox_index	 + 32;
	int oz_index	= oy_index	 + 32;
	int oxoy_index	= oz_index	 + 32;
	int oyoz_index	= oxoy_index + 32;
	int oxoz_index	= oyoz_index + 32;

	SATSolver solver;
	int num_of_sol = 0;

	solver.new_vars(oxoz_index + 32);

	read_variable(&solver, given_y, y_index);
	read_variable(&solver, given_z, z_index);
	read_variable(&solver, given_toy, toy_index);
	read_variable(&solver, given_toz, toz_index);

	compute_oy(&solver, toy_index, ox_index, oy_index, oxoz_index);
	compute_oz(&solver, toz_index, ox_index, oz_index, oyoz_index);
	compute_gimli_and(&solver, xy_index, x_index, y_index, 5, 20, 3);
	compute_gimli_or(&solver, xz_index, x_index, z_index, 7, 31, 1);
	compute_gimli_and(&solver, yz_index, y_index, z_index, 21, 30, 2);
	compute_ox(&solver, ox_index, z_index, y_index, xy_index);
	compute_oy(&solver, oy_index, x_index, y_index, xz_index);
	compute_oz(&solver, oz_index, x_index, z_index, yz_index);
	compute_gimli_and(&solver, oxoy_index, ox_index, oy_index, 5, 20, 3);
	compute_gimli_or(&solver, oxoz_index, ox_index, oz_index, 7, 31, 1);
	compute_gimli_and(&solver, oyoz_index, oy_index, oz_index, 21, 30, 2);
	compute_ox(&solver, tox_index, oz_index, oy_index, oxoy_index);

	//run solver
	while (solver.solve() == l_True)
	{
		uint32_t a_sol = 0, b_sol = 0;
		vector<Lit> ban_solution;
		num_of_sol++;

		for (int i = 0; i < 32; i++)
		{
			if (solver.get_model()[x_index + i] == l_True)
			{
				a_sol |= (1 << i);
				ban_solution.push_back(Lit(x_index + i, true));
			}
			else
			{
				ban_solution.push_back(Lit(x_index + i, false));
			}
			if (solver.get_model()[tox_index + i] == l_True)
			{
				b_sol |= (1 << i);
				ban_solution.push_back(Lit(tox_index + i, true));
			}
			else
			{
				ban_solution.push_back(Lit(tox_index + i, false));
			}
		}
		solver.add_clause(ban_solution);
		sols->push_back(a_sol);
		sols->push_back(b_sol);
		a_sol = 0;
		b_sol = 0;
	}
	return num_of_sol;
}

/* 
	Given y,z,tox, find solution x, toy, toz such that SP^2(x,y,z) = (tox, toy, toz) 
	
	solutions are save in "sols" (save variables 'x', 'toy', and 'toz')
	return the number of solutions
*/
int Gimli_9r_sol_finder_2(vector<uint32_t>* sols, uint32_t given_y, uint32_t given_z, uint32_t given_tox)
{
	int x_index = 0;
	int toy_index = x_index + 32;
	int toz_index = toy_index + 32;
	int y_index = toz_index + 32;
	int z_index = y_index + 32;
	int tox_index = z_index + 32;
	int xy_index = tox_index + 32;
	int yz_index = xy_index + 32;
	int xz_index = yz_index + 32;
	int ox_index = xz_index + 32;
	int oy_index = ox_index + 32;
	int oz_index = oy_index + 32;
	int oxoy_index = oz_index + 32;
	int oyoz_index = oxoy_index + 32;
	int oxoz_index = oyoz_index + 32;

	SATSolver solver;
	int num_of_sol = 0;

	solver.new_vars(oxoz_index + 32);

	read_variable(&solver, given_y, y_index);
	read_variable(&solver, given_z, z_index);
	read_variable(&solver, given_tox, tox_index);
	compute_gimli_and(&solver, xy_index, x_index, y_index, 5, 20, 3);
	compute_gimli_or(&solver, xz_index, x_index, z_index, 7, 31, 1);
	compute_gimli_and(&solver, yz_index, y_index, z_index, 21, 30, 2);

	compute_ox(&solver, tox_index, oz_index, oy_index, oxoy_index);

	compute_ox(&solver, ox_index, z_index, y_index, xy_index);
	compute_oy(&solver, oy_index, x_index, y_index, xz_index);
	compute_oz(&solver, oz_index, x_index, z_index, yz_index);
	compute_gimli_and(&solver, oxoy_index, ox_index, oy_index, 5, 20, 3);
	compute_gimli_or(&solver, oxoz_index, ox_index, oz_index, 7, 31, 1);
	compute_gimli_and(&solver, oyoz_index, oy_index, oz_index, 21, 30, 2);

	compute_oy(&solver, toy_index, ox_index, oy_index, oxoz_index);
	compute_oz(&solver, toz_index, ox_index, oz_index, oyoz_index);


	//run solver
	while (solver.solve() == l_True)
	{
		uint32_t a_sol = 0, b_sol = 0, c_sol = 0;
		//uint32_t ox_sol = 0, oz_sol = 0, oyoz_sol = 0;
		vector<Lit> ban_solution;
		num_of_sol++;

		for (int i = 0; i < 32; i++)
		{
			if (solver.get_model()[x_index + i] == l_True)
			{
				a_sol |= (1 << i);
				ban_solution.push_back(Lit(x_index + i, true));
			}
			else
			{
				ban_solution.push_back(Lit(x_index + i, false));
			}
			if (solver.get_model()[toy_index + i] == l_True)
			{
				b_sol |= (1 << i);
				ban_solution.push_back(Lit(toy_index + i, true));
			}
			else
			{
				ban_solution.push_back(Lit(toy_index + i, false));
			}
			if (solver.get_model()[toz_index + i] == l_True)
			{
				c_sol |= (1 << i);
				ban_solution.push_back(Lit(toz_index + i, true));
			}
			else
			{
				ban_solution.push_back(Lit(toz_index + i, false));
			}
			//if (solver.get_model()[ox_index + i] == l_True) ox_sol |= (1 << i);
			//if (solver.get_model()[oz_index + i] == l_True) oz_sol |= (1 << i);
			//if (solver.get_model()[oyoz_index + i] == l_True) oyoz_sol |= (1 << i);

		}
		solver.add_clause(ban_solution);
		sols->push_back(a_sol);
		sols->push_back(b_sol);
		sols->push_back(c_sol);
		a_sol = 0;
		b_sol = 0;
		c_sol = 0;

		//sols->push_back(ox_sol);
		//sols->push_back(oz_sol);
		//sols->push_back(oyoz_sol);
		//
		//ox_sol = 0;
		//oz_sol = 0;
		//oyoz_sol = 0;
	}
	return num_of_sol;
}

/* 
	Given x, toy, toz, find solutions y, z, tox such that SP^2(x,y,z) = (tox, toy, toz) 

	solutions are save in "sols" (save variables 'y', 'z', and 'tox')
	return the number of solutions
*/
int Gimli_9r_sol_finder_3(vector<uint32_t>* sols, uint32_t given_x, uint32_t given_toy, uint32_t given_toz)
{
	int y_index = 0;
	int z_index = y_index + 32;
	int tox_index = z_index + 32;
	int x_index = tox_index + 32;
	int toy_index = x_index + 32;
	int toz_index = toy_index + 32;
	int xy_index = toz_index + 32;
	int yz_index = xy_index + 32;
	int xz_index = yz_index + 32;
	int ox_index = xz_index + 32;
	int oy_index = ox_index + 32;
	int oz_index = oy_index + 32;
	int oxoy_index = oz_index + 32;
	int oyoz_index = oxoy_index + 32;
	int oxoz_index = oyoz_index + 32;

	SATSolver solver;
	int num_of_sol = 0;

	solver.new_vars(oxoz_index + 32);

	read_variable(&solver, given_x, x_index);
	read_variable(&solver, given_toy, toy_index);
	read_variable(&solver, given_toz, toz_index);

	compute_oy(&solver, toy_index, ox_index, oy_index, oxoz_index);
	compute_oz(&solver, toz_index, ox_index, oz_index, oyoz_index);
	compute_ox(&solver, tox_index, oz_index, oy_index, oxoy_index);

	compute_gimli_and(&solver, oxoy_index, ox_index, oy_index, 5, 20, 3);
	compute_gimli_or(&solver, oxoz_index, ox_index, oz_index, 7, 31, 1);
	compute_gimli_and(&solver, oyoz_index, oy_index, oz_index, 21, 30, 2);

	compute_ox(&solver, ox_index, z_index, y_index, xy_index);
	compute_oy(&solver, oy_index, x_index, y_index, xz_index);
	compute_oz(&solver, oz_index, x_index, z_index, yz_index);

	compute_gimli_and(&solver, xy_index, x_index, y_index, 5, 20, 3);
	compute_gimli_or(&solver, xz_index, x_index, z_index, 7, 31, 1);
	compute_gimli_and(&solver, yz_index, y_index, z_index, 21, 30, 2);
	
	//run solver
	while (solver.solve() == l_True)
	{
		uint32_t a_sol = 0, b_sol = 0, c_sol =0;
		vector<Lit> ban_solution;
		num_of_sol++;

		for (int i = 0; i < 32; i++)
		{
			if (solver.get_model()[y_index + i] == l_True)
			{
				a_sol |= (1 << i);
				ban_solution.push_back(Lit(y_index + i, true));
			}
			else
			{
				ban_solution.push_back(Lit(y_index + i, false));
			}
			if (solver.get_model()[z_index + i] == l_True)
			{
				b_sol |= (1 << i);
				ban_solution.push_back(Lit(z_index + i, true));
			}
			else
			{
				ban_solution.push_back(Lit(z_index + i, false));
			}
			if (solver.get_model()[tox_index + i] == l_True)
			{
				c_sol |= (1 << i);
				ban_solution.push_back(Lit(tox_index + i, true));
			}
			else
			{
				ban_solution.push_back(Lit(tox_index + i, false));
			}
		}
		solver.add_clause(ban_solution);
		sols->push_back(a_sol);
		sols->push_back(b_sol);
		sols->push_back(c_sol);
		a_sol = 0;
		b_sol = 0;
		c_sol = 0;
	}
	return num_of_sol;
}

/* Given h0, oy, oz, find solutions y, z, ox such that SP(h0, y, z) = (ox, oy, oz) */
int Gimli_9r_sol_finder_4(vector<uint32_t>* sols, uint32_t given_h0, uint32_t given_oy, uint32_t given_oz)
{
	int y_index = 0;
	int z_index = y_index + 32;
	int ox_index = z_index + 32;
	int x_index = ox_index + 32;
	int oy_index = x_index + 32;
	int oz_index = oy_index + 32;
	int xy_index = oz_index + 32;
	int yz_index = xy_index + 32;
	int xz_index = yz_index + 32;

	SATSolver solver;
	int num_of_sol = 0;

	solver.new_vars(xz_index + 32);

	read_variable(&solver, given_h0, x_index);
	read_variable(&solver, given_oy, oy_index);
	read_variable(&solver, given_oz, oz_index);

	compute_oy(&solver, oy_index, x_index, y_index, xz_index);
	compute_oz(&solver, oz_index, x_index, z_index, yz_index);

	compute_gimli_and(&solver, xy_index, x_index, y_index, 5, 20, 3);
	compute_gimli_or(&solver, xz_index, x_index, z_index, 7, 31, 1);
	compute_gimli_and(&solver, yz_index, y_index, z_index, 21, 30, 2);

	compute_ox(&solver, ox_index, z_index, y_index, xy_index);
	//run solver
	while (solver.solve() == l_True)
	{
		uint32_t a_sol = 0, b_sol = 0, c_sol = 0;
		vector<Lit> ban_solution;
		num_of_sol++;

		for (int i = 0; i < 32; i++)
		{
			if (solver.get_model()[y_index + i] == l_True)
			{
				a_sol |= (1 << i);
				ban_solution.push_back(Lit(y_index + i, true));
			}
			else
			{
				ban_solution.push_back(Lit(y_index + i, false));
			}
			if (solver.get_model()[z_index + i] == l_True)
			{
				b_sol |= (1 << i);
				ban_solution.push_back(Lit(z_index + i, true));
			}
			else
			{
				ban_solution.push_back(Lit(z_index + i, false));
			}
			if (solver.get_model()[ox_index + i] == l_True)
			{
				c_sol |= (1 << i);
				ban_solution.push_back(Lit(ox_index + i, true));
			}
			else
			{
				ban_solution.push_back(Lit(ox_index + i, false));
			}
		}
		solver.add_clause(ban_solution);
		sols->push_back(a_sol);
		sols->push_back(b_sol);
		sols->push_back(c_sol);
		a_sol = 0;
		b_sol = 0;
		c_sol = 0;
	}
	return num_of_sol;

}

/* given h0, ox, new ox, fox, find solutions such that SP(h0,y,z) = (ox, oy, oz) and SP^2(new_ox,oy,oz) = (fox, foy, foz) */
int Gimli_9r_sol_finder_5(vector<uint32_t>* sols, uint32_t given_h0, uint32_t given_ox, uint32_t given_new_ox, uint32_t given_fox)
{
	int y_index = 0;
	int z_index = y_index + 32;
	int foy_index = z_index + 32;
	int foz_index = foy_index + 32;
	int x_index = foz_index + 32;
	int ox_index = x_index + 32;
	int ox_c_index = ox_index + 32;
	int fox_index = ox_c_index + 32;
	int oy_index = fox_index + 32;
	int oz_index = oy_index + 32;
	int xy_index = oz_index + 32;
	int yz_index = xy_index + 32;
	int xz_index = yz_index + 32;
	int oxoy_index = xz_index + 32;
	int oyoz_index = oxoy_index + 32;
	int oxoz_index = oyoz_index + 32;
	int tox_index = oxoz_index + 32;
	int toy_index = tox_index + 32;
	int toz_index = toy_index + 32;
	int toxtoy_index = toz_index + 32;
	int toytoz_index = toxtoy_index + 32;
	int toxtoz_index = toytoz_index + 32;

	SATSolver solver;
	int num_of_sol = 0;

	solver.new_vars(toxtoz_index + 32);

	read_variable(&solver, given_h0, x_index);
	read_variable(&solver, given_ox, ox_index);
	read_variable(&solver, given_new_ox, ox_c_index);
	read_variable(&solver, given_fox, fox_index);

	compute_gimli_and(&solver, xy_index, x_index, y_index, 5, 20, 3);
	compute_gimli_or(&solver, xz_index, x_index, z_index, 7, 31, 1);

	compute_ox(&solver, ox_index, z_index, y_index, xy_index);

	compute_gimli_and(&solver, oxoy_index, ox_c_index, oy_index, 5, 20, 3);
	compute_gimli_or(&solver, oxoz_index, ox_c_index, oz_index, 7, 31, 1);

	compute_oy(&solver, toy_index, ox_c_index, oy_index, oxoz_index);
	compute_oz(&solver, toz_index, ox_c_index, oz_index, oyoz_index);
	compute_ox(&solver, fox_index, toz_index, toy_index, toxtoy_index);

	compute_gimli_and(&solver, yz_index, y_index, z_index, 21, 30, 2);

	compute_oy(&solver, oy_index, x_index, y_index, xz_index);
	compute_oz(&solver, oz_index, x_index, z_index, yz_index);

	compute_gimli_and(&solver, oyoz_index, oy_index, oz_index, 21, 30, 2);

	compute_ox(&solver, tox_index, oz_index, oy_index, oxoy_index);

	compute_gimli_and(&solver, toxtoy_index, tox_index, toy_index, 5, 20, 3);
	compute_gimli_or(&solver, toxtoz_index, tox_index, toz_index, 7, 31, 1);
	compute_gimli_and(&solver, toytoz_index, toy_index, toz_index, 21, 30, 2);

	compute_oy(&solver, foy_index, tox_index, toy_index, toxtoz_index);
	compute_oz(&solver, foz_index, tox_index, toz_index, toytoz_index);

	//run solver
	while (solver.solve() == l_True)
	{
		uint32_t a_sol = 0, b_sol = 0, c_sol = 0, d_sol = 0;
		vector<Lit> ban_solution;
		num_of_sol++;

		for (int i = 0; i < 32; i++)
		{
			if (solver.get_model()[y_index + i] == l_True)
			{
				a_sol |= (1 << i);
				ban_solution.push_back(Lit(y_index + i, true));
			}
			else
			{
				ban_solution.push_back(Lit(y_index + i, false));
			}
			if (solver.get_model()[z_index + i] == l_True)
			{
				b_sol |= (1 << i);
				ban_solution.push_back(Lit(z_index + i, true));
			}
			else
			{
				ban_solution.push_back(Lit(z_index + i, false));
			}
			if (solver.get_model()[foy_index + i] == l_True)
			{
				c_sol |= (1 << i);
				ban_solution.push_back(Lit(foy_index + i, true));
			}
			else
			{
				ban_solution.push_back(Lit(foy_index + i, false));
			}
			if (solver.get_model()[foz_index + i] == l_True)
			{
				d_sol |= (1 << i);
				ban_solution.push_back(Lit(foz_index + i, true));
			}
			else
			{
				ban_solution.push_back(Lit(foz_index + i, false));
			}
		}
		solver.add_clause(ban_solution);
		sols->push_back(a_sol);
		sols->push_back(b_sol);
		sols->push_back(c_sol);
		sols->push_back(d_sol);
		a_sol = 0;
		b_sol = 0;
		c_sol = 0;
		d_sol = 0;
	}
	return num_of_sol;

}