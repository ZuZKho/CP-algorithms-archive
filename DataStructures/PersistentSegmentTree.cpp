/*

    02.08.2023
    by ZuZKho

    Персистентное ДО на сумму

    XI ВКОШП 21 ноября 2010 года, задача I "откат"
    условие: http://neerc.ifmo.ru/school/archive/2010-2011/ru-olymp-team-russia-2010-statements.pdf
    разбор: http://neerc.ifmo.ru/school/archive/2010-2011/ru-olymp-team-russia-2010-analysis.pdf
    тестирующая система: https://informatics.msk.ru/mod/statements/view.php?chapterid=2980#1
                         https://codeforces.com/gym/100043/attachments  (файловый ввод)

*/


#include<bits/stdc++.h>

using namespace std;

const int MAXV = 40 * 1000 * 100 + 5;
const int MAXN = 1000*100 + 5;

struct SegTree {

    struct Node {
        int left = -1, right = -1;
        int summ = 0;
    };

    vector<int> versions;
    int arrSize;

    int sz = 1;
    vector<Node> nodes;

    void build(int v,int tl, int tr, vector<int> &a) {
        if (tr - tl == 1) {
            nodes[v].left = nodes[v].right = -1;
            if (tl < a.size()) {
                nodes[v].summ = a[tl];
            } else {
                nodes[v].summ = 0;
            }
        } else {
            int m = (tr + tl) / 2;
            nodes[v].left = sz++;
            nodes[v].right = sz++;
            build(nodes[v].left, tl, m, a);
            build(nodes[v].right, m, tr, a);
            nodes[v].summ = nodes[nodes[v].left].summ + nodes[nodes[v].right].summ;
        }
    }

    void Build(vector<int> &a) {
        versions.push_back(0);
        nodes.resize(MAXV);

        arrSize = 1;
        while(arrSize < a.size()) arrSize *= 2;

        build(0, 0, arrSize, a);
    }

    void set(int v,int tl, int tr, int pos,int val) {
        // Обновление элемента в позиции pos, когда элемент nodes[v] уже обновлен ранее
        if (tr - tl == 1) {
            nodes[v].summ = val;
            return;
        }
        int m = (tl + tr) / 2;
        if (pos < m) {
            // идем влево
            int nwNode = sz++;
            nodes[nwNode] = nodes[nodes[v].left];
            nodes[v].left = nwNode;
            set(nwNode, tl, m, pos ,val);
        } else {
            int nwNode = sz++;
            nodes[nwNode] = nodes[nodes[v].right];
            nodes[v].right = nwNode;
            set(nwNode, m, tr, pos, val);
        }
        nodes[v].summ = nodes[nodes[v].left].summ + nodes[nodes[v].right].summ;
    }

    void NewSet(int pos, int val) {
        // Обновление элемента с созданием нового корня
        int oldRoot = versions.back();
        versions.push_back(sz++);
        int nwRoot = versions.back();
        nodes[nwRoot] = nodes[oldRoot];
        set(nwRoot, 0, arrSize, pos, val);
    }

    void Set(int pos,int val) {
        // Обновление элемента без создания нового корня
        int root = versions.back();
        set(root, 0, arrSize, pos, val);
    }

    int get(int v,int tl, int tr, int k) {
        if (tr - tl == 1) {
            return tl;
        }

        int leftSum = nodes[nodes[v].left].summ;
        int rightSum = nodes[nodes[v].right].summ;
        int m = (tl + tr) /  2;

        if (leftSum >= k) {
            return get(nodes[v].left, tl, m, k);
        } else {
            return get(nodes[v].right, m, tr, k - leftSum);
        }
    }

    int Get(int version,int k) {
        int root = versions[version];
        if (nodes[root].summ < k) return -1; // Если во всем дереве нет суммы K
        // Найти минимальный индекс, что сумма хотя бы K
        return get(root, 0, arrSize, k);
    }

};

vector<int> positions[MAXN];
int pos[MAXN];

int next(int val) {
    pos[val]++;
    if (pos[val] >= positions[val].size()) return -1;
    return positions[val][pos[val]];
}

int main() {

    ios_base::sync_with_stdio(false);
    cin.tie(0);
    cout.tie(0);

    int P = 0;
    int n, m;
    cin >> n >> m;
    vector<int> a(n);
    for(int i = 0; i < n ; i++) {
        cin >> a[i];
        positions[a[i]].push_back(i);
    }


    // Строим массив b
    vector<int> used(MAXN);
    vector<int> b(n);
    for(int i = 0; i < n ; i ++) {
        if (used[a[i]]) b[i] = 0; else used[a[i]] = 1, b[i] = 1;
    }


    // Строим персистентное дерево
    SegTree tree;
    tree.Build(b);
    for(int i = 0; i < n; i++) {
        tree.NewSet(i, 0);
        int nxt = next(a[i]);
        if (nxt != - 1) {
            tree.Set(nxt, 1);
        }
    }

    int q;
    cin >> q;
    while(q--) {
        int x, y;
        cin >> x >> y;
        int l = (x + P) % n, k = (y + P) % m + 1;

        int answer = tree.Get(l, k) + 1; // версия l, k - число запроса
        cout << answer << '\n';
        P = answer;
    }

    return 0;
}