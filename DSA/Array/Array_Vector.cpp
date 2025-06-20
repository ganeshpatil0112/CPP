#include <iostream>
#include <vector>
using namespace std;

int main()
{
    // 1D Array and vector initialisation
    int arr[] = {1,2,3,4,5};
    vector<int> varr(5,1); //vector of 5 size with value 1 initialization

    // 2D Array and Vector initialisation
    // In the 2D array initiliztion, declartion or passing to function at least column size should be given.
    // For the 2D array: to find the position of location in array arrangment. formula is, position = total_column*column + row;
    // If your 2D array is:
    // [1, 2, 3],
    // [4, 5, 6],
    // [7, 8, 9];
    // Then in the actual memory it will be stored like in continuous memory location: find exact position in memory with formula: position = total_column*column + row;
    // [[1, 2, 3], [4, 5, 6], [7, 8, 9]];
    
    // 2D array initilization:
    int arr2[][3] = {{1, 2, 3},
                     {4, 5, 6},
                     {7, 8, 9}};

    // 2D vector initialization:
    vector<vector<int>> varr2(3, vector<int>(3, 1));
    // 2D Vector traversal:
    for(int i = 0; i < varr2.size(); i++)
    {
        for(int j = 0; j < varr2[i].size(); j++)
        {
            cout<<varr2[i][j];
        }
    }

}