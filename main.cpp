#include <hpx/local/algorithm.hpp>
#include <hpx/local/execution.hpp>
#include <hpx/local/init.hpp>

#include <cstddef>
#include <iostream>
#include <random>
#include <vector>

std::mt19937 gen;

using namespace std;
using namespace hpx::program_options;

void print_matrix(std::vector<int >M,std::size_t r,std::size_t c,const char* message)
{
    std::cout << "\nMatrix "<<message<<" is:"<<std::endl;
    for(std::size_t i=0;i<r;i++)
    {
        for(std::size_t j = 0;j<c;j++)
            std::cout<< M[i*c +j]<<" ";
        std::cout<< "\n";
    }
}

int hpx_main(hpx::program_options::variables_map& vm)
{
    using element_type = int;

    // defining matrix sizes
    std::size_t row_1 = vm["n"].as<std::size_t>();
    std::size_t col_1 = vm["m"].as<std::size_t>();
    std::size_t row_2 = col_1;
    std::size_t col_2 = vm["x"].as<std::size_t>();
    std::size_t resultant_row = row_1;
    std::size_t resultant_col = col_2;

    //initialising matrices
    std::vector<int> A(row_1*col_1);
    std::vector<int> B(row_2*col_2);
    std::vector<int> R(resultant_row*resultant_col);
    
    // defining random integer
    unsigned int random = std::random_device{}();
    if(vm.count("seed"))
        random = vm["seed"].as<unsigned int>();
    
    gen.seed(random);
    std::cout << "using random "<<random<<std::endl;

    // defining range of values
    int const lower = vm["l"].as<int>();
    int const upper = vm["u"].as<int>();

    // matrices will have random values from lower to upper
    std::uniform_int_distribution<element_type> dis(lower,upper);
    auto generator = std::bind(dis,gen);
    hpx::ranges::generate(A,generator);
    hpx::ranges::generate(B,generator);

    // performing matrix multiplication
    hpx::experimental::for_loop(hpx::execution::par, 0, row_1, [&](auto i) {
        hpx::experimental::for_loop(0, col_2, [&](auto j) {
            R[i * resultant_col + j] = 0;
            hpx::experimental::for_loop(0, row_2, [&](auto x) {
                R[i * resultant_col + j] += A[i * col_1 + x] * B[x * col_2 + j];
            });
        });
    });


    // printing all three matrices
    print_matrix(A,row_1,col_1,"A");
    print_matrix(B,row_2,col_2,"B");
    print_matrix(R,resultant_col,resultant_row,"C");

    return hpx::local::finalize();
}


int main(int argc,char* argv[])
{
    options_description cmdline("usage: " HPX_APPLICATION_STRING " [options]");
    cmdline.add_options()
    ("n",
    hpx::program_options::value<std::size_t>()->default_value(2),
    "number of rows in first matrix")
    ("m",
    hpx::program_options::value<std::size_t>()->default_value(3),
    "number of columns in first matrix")
    ("x",
    hpx::program_options::value<std::size_t>()->default_value(2),
    "number of columns in second matrix")
    ("seed,s",
    hpx::program_options::value<unsigned int>(),
    "random number generator")
    ("l",
    hpx::program_options::value<int>()->default_value(0),
    "lower limit range for random number generator")
    ("u",
    hpx::program_options::value<int>()->default_value(10),
    "upper limit range for random number generator");

    hpx::local::init_params init_args;
    init_args.desc_cmdline = cmdline;

    return hpx::local::init(hpx_main, argc, argv, init_args);

}

