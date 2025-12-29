#include <bits/stdc++.h>
using namespace std;

int n, m;
vector<vector<int>> g;
vector<int> side;   
vector<int> mt;     
vector<char> used;

bool kuhn(int v) {
    if (used[v]) return false;
    used[v] = true;
    for (int to : g[v]) {
        if (mt[to] == -1 || kuhn(mt[to])) {
            mt[to] = v;
            return true;
        }
    }
    return false;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    if (!(cin >> n >> m)) {
        return 0;
    }

    g.assign(n, {});
    for (int i = 0; i < m; ++i) {
        int u, v;
        cin >> u >> v;
        --u; --v;
        g[u].push_back(v);
        g[v].push_back(u);
    }

    for (int i = 0; i < n; ++i) {
        sort(g[i].begin(), g[i].end());
    }

    side.assign(n, -1);
    for (int i = 0; i < n; ++i) {
        if (side[i] == -1) {
            queue<int> q;
            side[i] = 0;
            q.push(i);
            while (!q.empty()) {
                int v = q.front(); q.pop();
                for (int to : g[v]) {
                    if (side[to] == -1) {
                        side[to] = side[v] ^ 1;
                        q.push(to);
                    }
                }
            }
        }
    }

    mt.assign(n, -1);  
    for (int v = 0; v < n; ++v) {
        if (side[v] == 0) {
            used.assign(n, false);
            kuhn(v);
        }
    }

    vector<pair<int,int>> ans;
    for (int v = 0; v < n; ++v) {
        if (mt[v] != -1) {
            int a = mt[v] + 1;
            int b = v + 1;
            if (a > b) swap(a, b);
            ans.emplace_back(a, b);
        }
    }

    sort(ans.begin(), ans.end());
    
    cout << ans.size() << "\n";
    for (auto &e : ans) {
        cout << e.first << " " << e.second << "\n";
    }

    return 0;
}