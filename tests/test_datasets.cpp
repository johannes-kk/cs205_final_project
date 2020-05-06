#include <iostream>
#include "../src/datasets.cpp"
#include <random>


int main(){

    DataLoader df_test_loader = DataLoader();  // Load test data.
    DataFrame df_test = df_test_loader.load();

    // Print as print method:
    std::cout << "Print test dataframe (with print method):" << std::endl;
    df_test.print(4);  // Optionally specify colwidth (truncates if too long).

    // Print with overloaded stream operator:
    std::cout << "Print test dataframe (with overloaded steam operator):" << std::endl;
    std::cout << df_test << std::endl;

    // Get last row:
    std::cout << "Print last row:" << std::endl;
    DataVector* test_row = df_test.row(-1);
    std::cout << *test_row << std::endl;

    // Get copy of last column:
    std::cout << "Print last column:" << std::endl;
    DataVector* test_col = df_test.col(-1)->copy();
    std::cout << *test_col << std::endl;
    
    // Get transpose of first column:
    std::cout << "Print transpose of first column:" << std::endl;
    DataVector* test_transpose_col = df_test.col(0)->transpose();
    std::cout << *test_transpose_col << std::endl;
    
    // Get copy of data frame:
    std::cout << "Print copy of data frame:" << std::endl;
    DataFrame test_copy_df = df_test.copy();
    std::cout << test_copy_df << std::endl;
    
    // Get transpose of data frame:
    std::cout << "Print transpose of data frame:" << std::endl;
    DataFrame test_transpose_df = df_test.transpose();
    std::cout << test_transpose_df << std::endl;
    
    // Test splitting by threshold (vector):
    std::cout << "Split last column with threshold 0.5 (transposed for better printing):" << std::endl;
    std::vector<DataVector> test_split_vector = df_test.col(-1)->split(0.5);
    std::cout << "L: " << test_split_vector[0].transpose() << std::endl;
    std::cout << "R: " << test_split_vector[1].transpose() << std::endl;
    
    // Test splitting by threshold (table):
    std::cout << "Split table on first column with threshold 3.396562:" << std::endl;
    std::vector<DataFrame> test_split_table = df_test.split(0,3.396562,false);
    std::cout << "L:\n" << test_split_table[0] << std::endl;
    std::cout << "R:\n" << test_split_table[1] << std::endl;
    
    // Test min, max, sum, mean:
    std::cout << "Get min along row axis:" << std::endl;
    std::cout << df_test.min(0)->to_string();
    std::cout << "Get min along column axis:" << std::endl;
    std::cout << df_test.min(1)->to_string();
    std::cout << "Get max along row axis:" << std::endl;
    std::cout << df_test.max(0)->to_string();
    std::cout << "Get max along column axis:" << std::endl;
    std::cout << df_test.max(1)->to_string();
    std::cout << "Get sum along row axis:" << std::endl;
    std::cout << df_test.sum(0)->to_string();
    std::cout << "Get sum along column axis:" << std::endl;
    std::cout << df_test.sum(1)->to_string();
    std::cout << "Get mean along row axis:" << std::endl;
    std::cout << df_test.mean(0)->to_string();
    std::cout << "Get mean along column axis:" << std::endl;
    std::cout << df_test.mean(1)->to_string();

    // Bootstrap a sample and print, optionally specifying 5 rows and seed 1337
    std::cout << "Bootstrap and print dataframe:" << std::endl;
    DataFrame df_boot = df_test.sample(5, 1337);
    df_boot.print(4);
};