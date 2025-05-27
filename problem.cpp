#include <bits/stdc++.h>
//#include <cctype>
#include <string>
//#include <type_traits>
#include <unordered_set>

using namespace std;

string ltrim(const string &);
string rtrim(const string &);

/*
 * Complete the 'countCounterfeit' function below.
 *
 * The function is expected to return an INTEGER.
 * The function accepts STRING_ARRAY serialNumber as parameter.
 */
bool isValidSerial(const string& s, unordered_set<int>& validDenoms)
{
    int n = s.length();
    if(n < 10 || n > 12) return false;
    
    if(!isupper(s[0]) || !isupper(s[1]) || !isupper(s[2]) || s[0] == s[1] || s[0] == s[2] || s[1] == s[2])
    {
        return false;
    }
    
    if(n < 8) return false;
    
    string yearStr = s.substr(3,4);
    for(char ch : yearStr)
    {
        if(!isdigit(ch))
        {
            return false;
        }
    }
    
    int year = stoi(yearStr);
    if(year < 1900 || year > 2019)
    {
        return false;
    }
    
    int i = 7;
    string demoStr = "";
    while (i < n - 1 && isdigit(s[i])) 
    {
        demoStr += s[i];
        i++;
    }
    
    if(demoStr.empty() || i != n-1)
    {
        return false;
    }
    
    int denom = stoi(demoStr);
    if(validDenoms.find(denom) == validDenoms.end())
    {
        return false;
    }
    
    if(!isupper(s[n-1]))
    {
        return false;
    }
    
    return true;
}


int countCounterfeit(vector<string> serialNumber) {
    
    unordered_set<int> validDenoms = {10, 20, 50, 100, 200, 500, 1000};
    int total = 0;
    
    for(const string& s : serialNumber)
    {
        if(isValidSerial(s, validDenoms))
        {
            string denomStr = "";
            for(int i = 7; i < s.length() - 1; ++i)
            {
                denomStr += s[i];
            }
            total += stoi(denomStr);
        }
    }
    
    return total;

}

int main()
{
    ofstream fout(getenv("OUTPUT_PATH"));

    string serialNumber_count_temp;
    getline(cin, serialNumber_count_temp);

    int serialNumber_count = stoi(ltrim(rtrim(serialNumber_count_temp)));

    vector<string> serialNumber(serialNumber_count);

    for (int i = 0; i < serialNumber_count; i++) {
        string serialNumber_item;
        getline(cin, serialNumber_item);

        serialNumber[i] = serialNumber_item;
    }

    int result = countCounterfeit(serialNumber);

    fout << result << "\n";

    fout.close();

    return 0;
}

string ltrim(const string &str) {
    string s(str);

    s.erase(
        s.begin(),
        find_if(s.begin(), s.end(), not1(ptr_fun<int, int>(isspace)))
    );

    return s;
}

string rtrim(const string &str) {
    string s(str);

    s.erase(
        find_if(s.rbegin(), s.rend(), not1(ptr_fun<int, int>(isspace))).base(),
        s.end()
    );

    return s;
}


/*
You are given an array of length n which was originally sorted in ascending order.
It has now been rotated between 1 and n times. 
For example, the array nums = [1,2,3,4,5,6] might become:
- [6,1,2,3,4,5] if it was rotated 1 time.
- [3,4,5,6,1,2] if it was rotated 4 times.
- [1,2,3,4,5,6] if it was rotated 6 times.

You do not know how many times the array was rotated.

Find the minimum element in the given array nums.

You can assume that all elements in the array are unique.

int findMin(std::vector<int> nums)
{
   int minimum = nums[0];
   
   for_each(nums.begin(), nums.end(), [](const int& a){
      if(minimum > a)
          minimum = a;
          break;
   })
   
   return minimum;
	
}

++ Using Binary Search !!
*/




