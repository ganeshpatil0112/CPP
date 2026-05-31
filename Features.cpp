//12, 13, 14, 15, 17, 15, 14, 14, 12, 14, 19, 13, 14, 19, 14, 13, 15, 17, 18, 15, 14, 17, 18, 13, 13, 19, 18, 16, 17, 16, 13, 15, 17
# include <iostream>
#include <vector>
using namespace std;
void bubbleSort(vector<int>& arr){
  int n = arr.size();
  for(int i =0; i<n-1; i++){
    bool swapped = true;
    for(int j = 0; j < n-i-1; j++)
    {
      if(arr[j] > arr[j +1])
      {
        int temp = arr[j+1];
        arr[j+1] = arr[j];
        arr[j] = temp;

        swapped = true;
      }
    }

    if (!swapped)
    {
      break;
    }
  }
}

void remove_duplicates(vector<int>& arr)
{
    auto it = arr.begin();
    while( it != arr.end()-1)
    {
        if(*it == *(it+1))
        {
          arr.erase(it);
        }
        else{
          ++it;
        }
    }
}

int main()
{
  vector <int> arrlist = {12, 13, 14, 15, 17, 15, 14, 14, 12, 14, 19, 13, 14, 19, 14, 13, 15, 17, 18, 15, 14, 17, 18, 13, 13, 19, 18, 16, 17, 16, 13, 15, 17};

  bubbleSort(arrlist);

  remove_duplicates(arrlist);

  for(auto it = arrlist.begin(); it != arrlist.end(); ++it)
    {
        cout<< *it <<" ";
    } 
}