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

static inline bool inBounds(int x, int y, int n, int m) {
    return (unsigned)x < (unsigned)n && (unsigned)y < (unsigned)m;
}

struct GridData {
    int n = 0, m = 0;
    vector<uint8_t> freeCell; 
    vector<int> comp;         
    int compCount = 0;
};

GridData readGrid(FastScanner &fs) {
    GridData G;
    fs.readInt(G.n);
    fs.readInt(G.m);

    vector<string> grid(G.n);
    for (int i = 0; i < G.n; i++) fs.readString(grid[i]);

    const int N = G.n * G.m;
    G.freeCell.assign(N, 0);
    for (int i = 0; i < G.n; i++) {
        for (int j = 0; j < G.m; j++) {
            G.freeCell[i * G.m + j] = (grid[i][j] == '.');
        }
    }

    G.comp.assign(N, -1);
    return G;
}

void buildComponents(GridData &G) {
    const int dx[4] = {1, -1, 0, 0};
    const int dy[4] = {0, 0, 1, -1};

    const int N = G.n * G.m;
    vector<int> q(N);

    int cc = 0;
    for (int start = 0; start < N; start++) {
        if (!G.freeCell[start] || G.comp[start] != -1) continue;

        int head = 0, tail = 0;
        q[tail++] = start;
        G.comp[start] = cc;

        while (head < tail) {
            int v = q[head++];
            int x = v / G.m, y = v % G.m;

            for (int k = 0; k < 4; k++) {
                int nx = x + dx[k], ny = y + dy[k];
                if (!inBounds(nx, ny, G.n, G.m)) continue;

                int to = nx * G.m + ny;
                if (!G.freeCell[to] || G.comp[to] != -1) continue;

                G.comp[to] = cc;
                q[tail++] = to;
            }
        }

        cc++;
    }
    G.compCount = cc;
}

int astarDistance(
    const GridData &G,
    int x1, int y1, int x2, int y2,
    vector<int> &dist, vector<int> &used, vector<int> &closed,
    int &stamp,
    vector<int> &cur, vector<int> &nxt
) {
    const int dx[4] = {1, -1, 0, 0};
    const int dy[4] = {0, 0, 1, -1};

    int s = x1 * G.m + y1;
    int t = x2 * G.m + y2;

    if (s == t) return 0;
    if (G.comp[s] != G.comp[t]) return -1;

    ++stamp;

    auto H = [&](int x, int y) -> int {
        return abs(x - x2) + abs(y - y2);
    };

    cur.clear();
    nxt.clear();
    cur.reserve(1024);
    nxt.reserve(1024);

    used[s] = stamp;
    dist[s] = 0;

    int curF = H(x1, y1);
    cur.push_back(s);

    size_t headCur = 0;

    while (true) {
        while (headCur < cur.size()) {
            int v = cur[headCur++];

           	if (used[v] != stamp) continue;
            if (closed[v] == stamp) continue;

            int x = v / G.m, y = v % G.m;
            int g = dist[v];

            if (g + H(x, y) != curF) continue;

            if (v == t) return g; 

            closed[v] = stamp;

            for (int k = 0; k < 4; k++) {
                int nx = x + dx[k], ny = y + dy[k];
                if (!inBounds(nx, ny, G.n, G.m)) continue;

                int to = nx * G.m + ny;
                if (!G.freeCell[to]) continue;
                if (closed[to] == stamp) continue;

                int ng = g + 1;
                if (used[to] != stamp || ng < dist[to]) {
                    used[to] = stamp;
                    dist[to] = ng;

                    int nf = ng + H(nx, ny);
					if (nf == curF) cur.push_back(to);
                    else nxt.push_back(to);
                }
            }
        }

        if (nxt.empty()) break;

        cur.swap(nxt);
        nxt.clear();
        headCur = 0;
        curF += 2;
    }

    return -1;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    FastScanner fs;

    GridData G = readGrid(fs);
    buildComponents(G);

    const int N = G.n * G.m;
    vector<int> dist(N, 0), used(N, 0), closed(N, 0);
    int stamp = 0;

    vector<int> cur, nxt;

    int q;
    fs.readInt(q);

    while (q--) {
        int x1, y1, x2, y2;
        fs.readInt(x1); fs.readInt(y1); fs.readInt(x2); fs.readInt(y2);
        --x1; --y1; --x2; --y2;

        int ans = astarDistance(G, x1, y1, x2, y2, dist, used, closed, stamp, cur, nxt);
        cout << ans << "\n";
    }

    return 0;
}
