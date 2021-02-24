#include <cryptominisat5/cryptominisat.h>
#include <assert.h>
#include <time.h>
#include <vector>
#include <random>
#include "gimli.hpp"
#include "gimli_9r_solver.hpp"

void match_test()
{
	vector<uint32_t> solutions;
	uint32_t column[3],column2[3];
	uint32_t x, y, z, yp, zp, ox, oy, oz, tox, toy, toz, fox, nox;
	int ns;
	int ns1=0, ns2=0, ns3=0, ns4=0, ns5=0, nsref=0;
	int i = 0, j = 0;

	std::random_device rand_seed;
	std::mt19937 gen_rand_num(rand_seed());
	std::uniform_int_distribution<uint32_t> dis;

	while (i < 10000)
	{
		x = dis(gen_rand_num);
		y = dis(gen_rand_num);
		z = dis(gen_rand_num);
		yp = dis(gen_rand_num);
		zp = dis(gen_rand_num);
		nox = dis(gen_rand_num);
		ox = dis(gen_rand_num);
		oy = dis(gen_rand_num);
		oz = dis(gen_rand_num);
		tox = dis(gen_rand_num);
		toy = dis(gen_rand_num);
		toz = dis(gen_rand_num);
		fox = dis(gen_rand_num);

		//variants for right inputs
		//column[0] = x;
		//column[1] = y;
		//column[2] = z;
		//gimli_SP_column(column);
		//
		//ox = column[0];
		//oy = column[1];
		//oz = column[2];
		//
		//gimli_SP_column(column);
		//tox = column[0];
		//toy = column[1];
		//toz = column[2];
		//
		//column[0] = nox;
		//column[1] = oy;
		//column[2] = oz;
		//gimli_SPSP_column(column);
		//fox = column[0];
		

		if (0 < (ns = Gimli_ref_eq8_sol(&solutions, y, z, yp, zp)))
		{
			for (j = 0; j < ns; j++)
			{
				column[0] = solutions[j];
				column[1] = y;
				column[2] = z;
				column2[0] = solutions[j];
				column2[1] = yp;
				column2[2] = zp;
				gimli_SPSP_column(column);
				gimli_SPSP_column(column2);
				if (column[0] != column2[0])
				{
					std::cout << "ref_eq_solv_error" << std::endl;
					std::cout << "y : " << y << std::endl;
					std::cout << "z : " << z << std::endl;
					std::cout << "yp : " << yp << std::endl;
					std::cout << "zp : " << zp << std::endl;
				}
				solutions.clear();
			}
			nsref += ns;
		}

		if (0 < (ns = Gimli_9r_sol_finder_1(&solutions, y, z, toy, toz)))
		{
			for (j = 0; j < ns; j++)
			{
				column[0] = solutions[2*j + 0];
				column[1] = y;
				column[2] = z;
				gimli_SPSP_column(column);
				if ((column[0] != solutions[2*j+1]) | (column[1] != toy) | (column[2] != toz))
				{
					std::cout << "sol_finder1_error" << std::endl;
					std::cout << "y : " << y << std::endl;
					std::cout << "z : " << z << std::endl;
					std::cout << "toy : " << toy << std::endl;
					std::cout << "toz : " << toz << std::endl;
					//break;
				}
				solutions.clear();
			}
			ns1 += ns;
		}
		//else
		//{
		//	std::cout << "not found solution for finder 1" << std::endl;
		//	std::cout << "y : " << y << std::endl;
		//	std::cout << "z : " << z << std::endl;
		//	std::cout << "toy : " << toy << std::endl;
		//	std::cout << "toz : " << toz << std::endl;
		//}

		if (0 < (ns = Gimli_9r_sol_finder_2(&solutions, y, z, tox)))
		{
			for (j = 0; j < ns; j++)
			{
				column[0] = solutions[3*j + 0];
				column[1] = y;
				column[2] = z;
				gimli_SPSP_column(column);
				if ((column[0] != tox) | (column[1] != solutions[3*j + 1]) | (column[2] != solutions[3*j + 2]))
				{
					std::cout << "sol_finder2_error" << std::endl;
					std::cout << "y : " << y << std::endl;
					std::cout << "z : " << z << std::endl;
					std::cout << "tox : " << tox << std::endl;
					//break;
				}
				solutions.clear();
			}
			ns2 += ns;
		}
		//else
		//{
		//	std::cout << "not found solution for finder 2" << std::endl;
		//	std::cout << "y : " << y << std::endl;
		//	std::cout << "z : " << z << std::endl;
		//	std::cout << "tox : " << tox << std::endl;
		//}

		if (0 < (ns = Gimli_9r_sol_finder_3(&solutions, x, toy, toz)))
		{
			for (j = 0; j < ns; j++)
			{
				column[0] = x;
				column[1] = solutions[3*j + 0];
				column[2] = solutions[3*j + 1];
				gimli_SPSP_column(column);
				if ((column[0] != solutions[3*j + 2]) | (column[1] != toy) | (column[2] != toz))
				{
					std::cout << "sol_finder3_error" << std::endl;
					std::cout << "x : " << x << std::endl;
					std::cout << "toy : " << toy << std::endl;
					std::cout << "toz : " << toz << std::endl;
					//break;
				}
				solutions.clear();
			}
			ns3 += ns;
		}
		//else
		//{
		//	std::cout << "not found solution for finder 3" << std::endl;
		//	std::cout << "x : " << x << std::endl;
		//	std::cout << "toy : " << toy << std::endl;
		//	std::cout << "toz : " << toz << std::endl;
		//}

		if (0 < (ns = Gimli_9r_sol_finder_4(&solutions, x, oy, oz)))
		{
			for (j = 0; j < ns; j++)
			{
				column[0] = x;
				column[1] = solutions[3*j + 0];
				column[2] = solutions[3*j + 1];
				gimli_SP_column(column);
				if ((column[0] != solutions[3*j + 2]) | (column[1] != oy) | (column[2] != oz))
				{
					std::cout << "sol_finder4_error" << std::endl;
					std::cout << "x : " << x << std::endl;
					std::cout << "oy : " << oy << std::endl;
					std::cout << "oz : " << oz << std::endl;
					//break;
				}
				solutions.clear();
			}
			ns4 += ns;
		}
		//else
		//{
		//	std::cout << "not found solution for finder 4" << std::endl;
		//	std::cout << "x : " << x << std::endl;
		//	std::cout << "oy : " << oy << std::endl;
		//	std::cout << "oz : " << oz << std::endl;
		//}

		if (0 < (ns = Gimli_9r_sol_finder_5(&solutions, x, ox, nox, fox)))
		{
			for (j = 0; j < ns; j++)
			{
				column[0] = x;
				column[1] = solutions[4*j + 0];
				column[2] = solutions[4*j + 1];
				gimli_SP_column(column);
				if (column[0] != ox)
				{
					std::cout << "sol_finder5_ox_error" << std::endl;
					std::cout << "x : " << x << std::endl;
					std::cout << "ox : " << ox << std::endl;
					std::cout << "nox : " << nox << std::endl;
					std::cout << "fox : " << fox << std::endl;
					//break;
				}
				column[0] = nox;
				gimli_SPSP_column(column);
				if ((column[0] != fox) | (column[1] != solutions[4*j + 2]) | (column[2] != solutions[4*j + 3]))
				{
					std::cout << "sol_finder5_fox_error" << std::endl;
					std::cout << "x : " << x << std::endl;
					std::cout << "ox : " << ox << std::endl;
					std::cout << "nox : " << nox << std::endl;
					std::cout << "fox : " << fox << std::endl;
					//break;
				}
				solutions.clear();
			}
			ns5 += ns;
		}
		//else
		//{
		//	std::cout << "not found solution for finder 5" << std::endl;
		//	std::cout << "x : " << x << std::endl;
		//	std::cout << "ox : " << ox << std::endl;
		//	std::cout << "nox : " << nox << std::endl;
		//	std::cout << "fox : " << fox << std::endl;
		//}
		i++;
		//std::cout << "." << std::endl;
	}
	std::cout << "average solutions of ref 8 : " << (double)(nsref)/(double)(10000) << std::endl;
	std::cout << "average solutions of finder 1 : " << (double)(ns1)/(double)(10000) << std::endl;
	std::cout << "average solutions of finder 2 : " << (double)(ns2)/(double)(10000) << std::endl;
	std::cout << "average solutions of finder 3 : " << (double)(ns3)/(double)(10000) << std::endl;
	std::cout << "average solutions of finder 4 : " << (double)(ns4)/(double)(10000) << std::endl;
	std::cout << "average solutions of finder 5 : " << (double)(ns5)/(double)(10000) << std::endl;
}

void speed_test_gimli(int iter)
{
	clock_t start_t, finish_t;
	uint8_t state[48] = { 0, };

	std::cout << "Gimli permutation speed test" << std::endl;

	for (int i = 0; i < 5; < i++)
	{
		start_t = clock();
		for (int j = 0; j < iter; j++)
		{
			gimli(state);
		}
		finish_t = clock();
		double time_elapsed = ((double)((finish_t - start_t) * 1000 * 1000)) / (CLOCKS_PER_SEC * iter);
		std::cout << "test #" << j << " time : " << time_elapsed << "micro sec" << std::endl;
	}

}

void speed_test_ref_eq8(int iter)
{
	clock_t start_t, finish_t;
	vector<uint32_t> solution;
	double t_num_of_solutions=0;
	//uint32_t column[3];

	std::random_device rand_seed;
	std::mt19937 gen_rand_num(rand_seed());
	std::uniform_int_distribution<uint32_t> dis;

	std::cout << "reference eq8 time test" << std::endl;

	for (int j = 0; j < 5; j++)
	{
		t_num_of_solutions = 0;
		start_t = clock();

		for (int i = 0; i < iter; i++)
		{
			uint32_t given_y = dis(gen_rand_num);
			uint32_t given_z = dis(gen_rand_num);
			uint32_t given_yp = dis(gen_rand_num);
			uint32_t given_zp = dis(gen_rand_num);

			t_num_of_solutions += (double)Gimli_ref_eq8_sol(&solution, given_y, given_z, given_yp, given_zp);

			solution.clear();
		}

		finish_t = clock();
		double time_elapsed = ((double)((finish_t - start_t) * 1000 * 1000)) / (CLOCKS_PER_SEC*iter);
		std::cout << "test #" << j << " time : " << time_elapsed << " micro sec" << std::endl;
		std::cout << "test #" << j << " average solution number : " << t_num_of_solutions/(double)iter << std::endl;
	}
}

void speed_test_1(int iter)
{
	clock_t start_t, finish_t;
	vector<uint32_t> solution;
	double t_num_of_solutions=0;
	//uint32_t column[3];

	std::random_device rand_seed;
	std::mt19937 gen_rand_num(rand_seed());
	std::uniform_int_distribution<uint32_t> dis;

	std::cout << "sol finder 1 time test" << std::endl;

	for (int j = 0; j < 5; j++)
	{
		t_num_of_solutions = 0;
		start_t = clock();

		for (int i = 0; i < iter; i++)
		{
			uint32_t given_y = dis(gen_rand_num);
			uint32_t given_z = dis(gen_rand_num);
			uint32_t given_toy = dis(gen_rand_num);
			uint32_t given_toz = dis(gen_rand_num);

			t_num_of_solutions += (double)Gimli_9r_sol_finder_1(&solution, given_y, given_z, given_toy, given_toz);

			solution.clear();
		}

		finish_t = clock();
		double time_elapsed = ((double)((finish_t - start_t) * 1000 * 1000)) / (CLOCKS_PER_SEC*iter);
		std::cout << "test #" << j << " time : " << time_elapsed << " micro sec" << std::endl;
		std::cout << "test #" << j << " total solution number : " << t_num_of_solutions << std::endl;
	}
}

void speed_test_2(int iter)
{
	clock_t start_t, finish_t;
	vector<uint32_t> solution;
	double t_num_of_solutions=0;
	//uint32_t column[3];

	std::random_device rand_seed;
	std::mt19937 gen_rand_num(rand_seed());
	std::uniform_int_distribution<uint32_t> dis;

	std::cout << "sol finder 2 time test" << std::endl;

	for (int j = 0; j < 5; j++)
	{
		t_num_of_solutions = 0;
		start_t = clock();

		for (int i = 0; i < iter; i++)
		{
			uint32_t given_y = dis(gen_rand_num);
			uint32_t given_z = dis(gen_rand_num);
			uint32_t given_tox = dis(gen_rand_num);

			t_num_of_solutions += (double)Gimli_9r_sol_finder_2(&solution, given_y, given_z, given_tox);

			solution.clear();
		}

		finish_t = clock();
		double time_elapsed = ((double)((finish_t - start_t) * 1000 * 1000)) / (CLOCKS_PER_SEC*iter);
		std::cout << "test #" << j << " time : " << time_elapsed << " micro sec" << std::endl;
		std::cout << "test #" << j << " total solution number : " << t_num_of_solutions/(double)iter << std::endl;
	}
}

void speed_test_3(int iter)
{
	clock_t start_t, finish_t;
	vector<uint32_t> solution;
	double t_num_of_solutions=0;
	//uint32_t column[3];

	std::random_device rand_seed;
	std::mt19937 gen_rand_num(rand_seed());
	std::uniform_int_distribution<uint32_t> dis;

	std::cout << "sol finder 3 time test" << std::endl;

	for (int j = 0; j < 5; j++)
	{
		t_num_of_solutions = 0;
		start_t = clock();
		for (int i = 0; i < iter; i++)
		{
			uint32_t given_x = dis(gen_rand_num);
			uint32_t given_toy = dis(gen_rand_num);
			uint32_t given_toz = dis(gen_rand_num);

			t_num_of_solutions += (double)Gimli_9r_sol_finder_3(&solution, given_x, given_toy, given_toz);

			solution.clear();
		}

		finish_t = clock();
		double time_elapsed = ((double)((finish_t - start_t) * 1000 * 1000)) / (CLOCKS_PER_SEC*iter);
		std::cout << "test #" << j << " time : " << time_elapsed << " micro sec" << std::endl;
		std::cout << "test #" << j << " total solution number : " << t_num_of_solutions/(double)iter << std::endl;
	}
}

void speed_test_4(int iter)
{
	clock_t start_t, finish_t;
	vector<uint32_t> solution;
	double t_num_of_solutions=0;
	//uint32_t column[3];

	std::random_device rand_seed;
	std::mt19937 gen_rand_num(rand_seed());
	std::uniform_int_distribution<uint32_t> dis;

	std::cout << "sol finder 4 time test" << std::endl;
	

	for (int j = 0; j < 5; j++)
	{
		t_num_of_solutions = 0;
		start_t = clock();
		for (int i = 0; i < iter; i++)
		{
			uint32_t given_h0 = dis(gen_rand_num);
			uint32_t given_oy = dis(gen_rand_num);
			uint32_t given_oz = dis(gen_rand_num);

			t_num_of_solutions += (double)Gimli_9r_sol_finder_4(&solution, given_h0, given_oy, given_oz);

			solution.clear();
		}

		finish_t = clock();
		double time_elapsed = ((double)((finish_t - start_t) * 1000 * 1000)) / (CLOCKS_PER_SEC*iter);
		std::cout << "test #" << j << " time : " << time_elapsed << " micro sec" << std::endl;
		std::cout << "test #" << j << " total solution number : " << t_num_of_solutions/(double)iter << std::endl;
	}
}



void speed_test_5(int iter)
{
	clock_t start_t, finish_t;
	vector<uint32_t> solution;
	double t_num_of_solutions=0;
	//uint32_t column[3];

	std::random_device rand_seed;
	std::mt19937 gen_rand_num(rand_seed());
	std::uniform_int_distribution<uint32_t> dis;

	std::cout << "sol finder 5 time test" << std::endl;

	for (int j = 0; j < 5; j++)
	{
		t_num_of_solutions = 0;
		start_t = clock();
		for (int i = 0; i < iter; i++)
		{
			uint32_t given_h0 = dis(gen_rand_num);
			uint32_t given_ox = dis(gen_rand_num);
			uint32_t given_nox = dis(gen_rand_num);
			uint32_t given_fox = dis(gen_rand_num);

			t_num_of_solutions += (double)Gimli_9r_sol_finder_5(&solution, given_h0, given_ox, given_nox, given_fox);

			solution.clear();
		}

		finish_t = clock();
		double time_elapsed = ((double)((finish_t - start_t) * 1000 )) / (CLOCKS_PER_SEC*iter);
		std::cout << "test #" << j << " time : " << time_elapsed << " mili sec" << std::endl;
		std::cout << "test #" << j << " total solution number : " << t_num_of_solutions/(double)iter << std::endl;
	}
}

int main()
{
	

	//match_test();
	speed_test_gimli(100000);
	speed_test_ref_eq8(100000);
	speed_test_1(100000);
	speed_test_2(10000);
	speed_test_3(10000);
	speed_test_4(10000);
	speed_test_5(1000);

	return 0;
}