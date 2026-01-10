#include <bits/stdc++.h>
using namespace std;

class KuhnMatching {
public:
    int n = 0, m = 0;
    vector<vector<int>> g;
    vector<int> side;     
    vector<int> mt;        
    vector<bool> used;   

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

    void colorize_bfs() {
        side.assign(n, -1);
        for (int i = 0; i < n; ++i) {
            if (side[i] != -1) continue;

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

    vector<pair<int,int>> solve() {
        colorize_bfs();

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
        return ans;
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    KuhnMatching solver;

    if (!(cin >> solver.n >> solver.m)) {
        return 0;
    }

    solver.g.assign(solver.n, {});
    for (int i = 0; i < solver.m; ++i) {
        int u, v;
        cin >> u >> v;
        --u; --v;
        solver.g[u].push_back(v);
        solver.g[v].push_back(u);
    }

    for (int i = 0; i < solver.n; ++i) {
        sort(solver.g[i].begin(), solver.g[i].end());
    }

    auto ans = solver.solve();

    cout << ans.size() << "\n";
    for (auto &e : ans) {
        cout << e.first << " " << e.second << "\n";
    }

    return 0;
}
