#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    
    string N;
    int m;
    if (!(cin >> N >> m)) return 0;

    const int L = (int)N.size();
    vector<int> a(L);
    for (int i = 0; i < L; ++i) a[i] = N[i] - '0';

    using ull = unsigned long long;

    vector<ull> 
        dp00(m, 0), 
        dp01(m, 0), 
        dp10(m, 0), 
        dp11(m, 0);
        
    dp00[0] = 1;  

    ull answer = 0;

    for (int i = 0; i < L; ++i) {
        vector<ull> 
            ndp00(m, 0), 
            ndp01(m, 0), 
            ndp10(m, 0), 
            ndp11(m, 0);

        auto relax = [&](bool lex_less, bool num_less, int rem, ull ways) {
            if (ways == 0) return;
            int ub = 9;
            if (!lex_less) ub = min(ub, a[i]);
            if (!num_less) ub = min(ub, a[i]);
            int start = (i == 0 ? 1 : 0); 

            for (int d = start; d <= ub; ++d) {
                bool nlex = lex_less || (d < a[i]);
                bool nnum = num_less || (d < a[i]);
                int nrem = ( (rem * 10) + d ) % m;

                if (!nlex && !nnum) ndp00[nrem] += ways;
                else if (!nlex &&  nnum) ndp01[nrem] += ways;
                else if ( nlex && !nnum) ndp10[nrem] += ways;
                else                      ndp11[nrem] += ways;

                int len = i + 1;
                bool lex_ok = (len < L) || nlex; 
                bool num_ok = (len < L) || nnum; 
                if (nrem == 0 && lex_ok && num_ok) {
                    answer += ways;
                }
            }
        };

        for (int r = 0; r < m; ++r) {
            relax(false, false, r, dp00[r]);
            relax(false, true , r, dp01[r]);
            relax(true , false, r, dp10[r]);
            relax(true , true , r, dp11[r]);
        }

        dp00.swap(ndp00);
        dp01.swap(ndp01);
        dp10.swap(ndp10);
        dp11.swap(ndp11);
    }

   cout << answer << '\n';
    return 0;
}
