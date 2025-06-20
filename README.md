# CPP
C++ code

File = sum.cpp
g++ sum.cpp -o sum
./sum

g++ -std=c++23 program.cpp -o program -lboost_system

#include<bits/stdc++.h>



for the secp256k1 library install
// git clone https://github.com/bitcoin-core/secp256k1.git
// cd secp256k1
// ./autogen.sh
// ./configure --enable-module-recovery
// make
// sudo make install






# CPP
> [!NOTE]
> Useful information that users should know, even when skimming content.

> [!TIP]
> Helpful advice for doing things better or more easily.

> [!IMPORTANT]
> Key information users need to know to achieve their goal.

> [!WARNING]
> Urgent info that needs immediate user attention to avoid problems.

> [!CAUTION]
> Advises about risks or negative outcomes of certain actions.

Let's rename \*our-new-project\* to \*our-old-project\*.

Here is a simple footnote[^1].

A footnote can also have multiple lines[^2].

[^1]: My reference.
[^2]: To add line breaks within a footnote, prefix new lines with 2 spaces.
This is a second line.

@octocat :+1: This PR looks great - it's ready to merge! :shipit:

@github/support What do you think about these updates?

- [x] #739
- [ ] https://github.com/octo-org/octo-repo/issues/740
- [ ] Add delight to the experience when all tasks are complete :tada:

100. First list item
     - First nested list item
       - Second nested list item

       # Section Heading

Some body text of this section.

<a name="my-custom-anchor-point"></a>
Some text I want to provide a direct link to, but which doesn't have its own heading.

(… more content…)

[A link to that custom anchor](#my-custom-anchor-point)

# A first-level heading
## A second-level heading
### A third-level heading


Text that is not a quote

> Text that is a quote



Some basic Git commands are:
```C++
void maps::operations()
{
    //update and .find() value of map
    rank[4] = "Fourth";
    rankss.insert({"Akshay", "Borase"});
    print(rankss);
    string value = to_string(ranks["Tenth"]);
    cout<<endl<<ranks["Tenth"]<<", At: "<<ranks.at("Tenth")<<", Size: "<<ranks.size()<<", Count: "<<ranks.count("Tenth")<<", ";

    // auto it1 = find(ranks.begin(), ranks.end(), "First");       This find function not works in map
    // cout<<"\nFind stl function : "<<it1->second<<endl;
    
    auto it = ranks.find("First");
    if(it != ranks.end())
    {
        cout<<"Find: {"<<it->first<<":"<<it->second<<"}";
    }
}
```


<div align="center">
  <span><img src="./loginScreen.jpg" height=100 width=100 /></span>
  <span><img src="./aligner.png" height=50 width=50 /></span> <!--invisible-->
  <span><img src="./Expenses.jpg" height=100 width=150 /></span>
</div>


<div style="display: flex; justify-content: space-between; gap: 20px;">

<div style="flex: 1; padding: 10px; border: 1px solid #ddd;">
<h3>Function 1: `rtrim`</h3>
<pre>
<code class="cpp">
string rtrim(const string &str) {
    string s(str);

    s.erase(
        find_if(s.rbegin(), s.rend(), not1(ptr_fun<int, int>(isspace))).base(),
        s.end()
    );

    return s;
}
</code>
</pre>
</div>

<div style="flex: 1; padding: 10px; border: 1px solid #ddd;">
<h3>Function 2: `maps::operations`</h3>
<pre>
<code class="cpp">
void maps::operations() {
    //update and .find() value of map
    rank[4] = "Fourth";
    rankss.insert({"Akshay", "Borase"});

    print(rankss);
    string value = to_string(ranks["Tenth"]);
    cout<<endl<<ranks["Tenth"]<<", At: "<<ranks.at("Tenth")<<", Size: "<<ranks.size()<<", Count: "<<ranks.count("Tenth")<<", ";

    auto it = ranks.find("First");
    if(it != ranks.end()) {
        cout<<"Find: {"<<it->first<<":"<<it->second<<"}";
    }
}
</code>
</pre>
</div>

</div>