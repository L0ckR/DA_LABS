#include <bits/stdc++.h>
using namespace std;

struct FastScanner {
    static const int BUFSIZE = 1 << 20;
    int idx = 0, size = 0;
    char buf[BUFSIZE];

    inline char readChar() {
        if (idx >= size) {
            size = (int)fread(buf, 1, BUFSIZE, stdin);
            idx = 0;
            if (!size) return 0;
        }
        return buf[idx++];
    }

    template <class T>
    bool readInt(T &out) {
        char c;
        do {
            c = readChar();
            if (!c) return false;
        } while (c <= ' ');

        T sign = 1;
        if (c == '-') { sign = -1; c = readChar(); }

        T val = 0;
        while (c > ' ') {
            val = val * 10 + (c - '0');
            c = readChar();
        }
        out = val * sign;
        return true;
    }

    bool readString(string &s) {
        char c;
        do {
            c = readChar();
            if (!c) return false;
        } while (c <= ' ');
        s.clear();
        while (c > ' ') {
            s.push_back(c);
            c = readChar();
        }
        return true;
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    FastScanner fs;
    int n, m;
    if (!fs.readInt(n)) return 0;
    fs.readInt(m);

    vector<string> grid(n);
    for (int i = 0; i < n; i++) fs.readString(grid[i]);

    const int N = n * m;
    vector<uint8_t> freeCell(N, 0);
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            freeCell[i * m + j] = (grid[i][j] == '.');

    auto inside = [&](int x, int y) {
        return (unsigned)x < (unsigned)n && (unsigned)y < (unsigned)m;
    };

    const int dx[4] = {1, -1, 0, 0};
    const int dy[4] = {0, 0, 1, -1};

    vector<int> comp(N, -1);
    vector<int> bfs(N);
    int cc = 0;

    for (int start = 0; start < N; start++) {
        if (!freeCell[start] || comp[start] != -1) continue;

        int head = 0, tail = 0;
        bfs[tail++] = start;
        comp[start] = cc;

        while (head < tail) {
            int v = bfs[head++];
            int x = v / m, y = v % m;

            for (int k = 0; k < 4; k++) {
                int nx = x + dx[k], ny = y + dy[k];
                if (!inside(nx, ny)) continue;

                int to = nx * m + ny;
                if (!freeCell[to] || comp[to] != -1) continue;

                comp[to] = cc;
                bfs[tail++] = to;
            }
        }
        cc++;
    }

    vector<int> dist(N, 0);
    vector<int> used(N, 0);
    vector<int> closed(N, 0);
    int stamp = 0;

    vector<int> cur, nxt;
    cur.reserve(1024);
    nxt.reserve(1024);

    int q;
    fs.readInt(q);

    while (q--) {
        int x1, y1, x2, y2;
        fs.readInt(x1); fs.readInt(y1); fs.readInt(x2); fs.readInt(y2);
        --x1; --y1; --x2; --y2;

        int s = x1 * m + y1;
        int t = x2 * m + y2;

        if (s == t) {
            cout << 0 << "\n";
            continue;
        }
        if (comp[s] != comp[t]) {
            cout << -1 << "\n";
            continue;
        }

        ++stamp;

        auto Hxy = [&](int x, int y) -> int {
            return abs(x - x2) + abs(y - y2);
        };

        cur.clear();
        nxt.clear();

        used[s] = stamp;
        closed[s] = 0;   
        dist[s] = 0;

        int curF = Hxy(x1, y1);
        cur.push_back(s);

        int answer = -1;
        size_t headCur = 0;

        while (true) {
            while (headCur < cur.size()) {
                int v = cur[headCur++];

                if (used[v] != stamp) continue;
                if (closed[v] == stamp) continue;

                int x = v / m, y = v % m;
                int g = dist[v];

                if (g + Hxy(x, y) != curF) continue;

                if (v == t) {
                    answer = g;
                    break;
                }

                closed[v] = stamp;

                for (int k = 0; k < 4; k++) {
                    int nx = x + dx[k], ny = y + dy[k];
                    if (!inside(nx, ny)) continue;

                    int to = nx * m + ny;
                    if (!freeCell[to]) continue;
                    if (closed[to] == stamp) continue;

                    int ng = g + 1;
                    if (used[to] != stamp || ng < dist[to]) {
                        used[to] = stamp;
                        dist[to] = ng;

                        int nf = ng + Hxy(nx, ny);
                        if (nf == curF) cur.push_back(to);
                        else nxt.push_back(to);
                    }
                }
            }

            if (answer != -1) break;
            if (nxt.empty()) break;

            cur.swap(nxt);
            nxt.clear();
            headCur = 0;
            curF += 2;
        }

        cout << answer << "\n";
    }

    return 0;
}
