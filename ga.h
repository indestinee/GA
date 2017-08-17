/*
 *  Author:
 *      Indestinee
 *  Date:
 *      2017/08/17
 *  Name:
 *      ga.h
 */

#ifndef STD_HEADER
#define STD_HEADER
#include <bits/stdc++.h>
using namespace std;
#endif

typedef vector<int> Gene;

const long long RAND2_MAX = 1LL << 62;
inline long long rand2() {
    return ((long long) rand()) << 31 | rand();
}

inline double random(const double &l, const double &r) {
    return ((double) rand2()) / (double) RAND2_MAX * (r - l) + l;
}

inline void mutation(Gene &new_gene) {
    int n = new_gene.size(), a = rand() % n, b = rand() % n;
    swap(new_gene[a], new_gene[b]);
}

inline void print(const Gene &gene) {
    printf("Gene: ");
    int m = gene.size();
    for (int i = 0; i < m; i++)
        printf("%d%c", gene[i], (i + 1 == m) ? '\n' : ' ');
}

struct Life{
    Gene gene;
    double score;
    Life() { }
    Life (const Gene &gene) : gene(gene), score(-1) { }
    inline void out() {
        print(gene);
        printf("Score = %.10f, dis = %.10f\n\n", score, 1.0 / score);
    }
    inline friend bool operator < (const Life &a, const Life &b) {
        return a.score > b.score;
    }
};

inline void print(Life a) {
    a.out();
}

inline void cross(const Life &father, const Life &mother, Gene &new_gene) {
    new_gene = father.gene;
    int n = new_gene.size(), a = rand() % n, b = rand() % n;
    if (a > b)
        swap(a, b);
    b++;
    unordered_set<int> s;
    vector<int> f(b - a);
    for (int i = a; i < b; i++) {
        new_gene[i] = mother.gene[i];
        s.insert(mother.gene[i]);
    }
    int cnt = 0;
    for (int i = a; i < b; i++)
        if (s.find(father.gene[i]) == s.end())
            f[cnt++] = father.gene[i];
    
    int p = 0;
    for (int i = 0; i < n; i++) {
        if (i == a) {
            i = b - 1;
            continue;
        }
        if (s.find(new_gene[i]) != s.end())
            new_gene[i] = f[p++];
    }

/*
    cout << a << " " << b << endl;
    print(father);
    print(mother);
    print(new_gene);
    if (p != cnt) {
        printf("p = %d != %d = cnt\n", p, cnt);
        exit(-1);
    }
    s.clear();
    for (int i = 0; i < n; i++) {
        if (s.find(new_gene[i]) != s.end()) {
            printf("find %d twice!\n", new_gene[i]);
            exit(-1);
        }
        s.insert(new_gene[i]);
    }
*/

}   


struct Config{
    double corss_rate, mutation_rate;
    int life_cnt, gene_len;
    double (*fun) (Life);
    Config () { }
    Config (
        const double &corss_rate,
        const double &mutation_rate,
        const int &life_cnt,
        const int &gene_len,
        double (*const fun) (Life)
    ) : corss_rate(corss_rate), mutation_rate(mutation_rate), \
        life_cnt(life_cnt), gene_len(gene_len), fun(fun) { }
};

class GA{
    vector<Life> lives;
    Life *best;
    Config config;
    double sum;
    int generation;
private:
    inline void init_population() {
        generation = 0;
        lives.clear();
        Gene gene(config.gene_len);
        for (int i = 0; i < config.gene_len; i++)
            gene[i] = i;
        for (int i = 0; i < config.life_cnt; i++) {
            random_shuffle(gene.begin(), gene.end());
            lives.push_back(Life(gene));
        }
    }
public:
    GA(const Config &config) : config(config) {
        init_population();
    }
    inline Life leader() {
        estimate();
        return *best;
    }
    inline void out() {
        estimate();
        printf("Generation = %d\n", generation);
        for (auto each: lives)
            each.out();
        puts("-------------------------------");
    }


    inline void estimate() {
        best = &lives[0];
        sum = 0;
        for (auto &life: lives) {
            sum += (life.score = config.fun(life));
            if (best->score < life.score)
                best = &life;
        }
    }
    
    inline Life* one() {
        auto r = random(0, sum);
        for (auto &life: lives) {
            r -= life.score;
            if (r <= 0)
                return &life;
        }
        return &(*lives.rbegin());
    }
    
    inline Life multiply() {
        Life *father = one();
        auto rate = random(0, 1);
        Gene gene;
        if (rate < config.corss_rate) {
            Life *mather = one();
            cross(*father, *mather, gene);
        } else {
            gene = father->gene;
        }
        rate = random(0, 1);
        if (rate < config.mutation_rate)
            mutation(gene);
        return Life(gene);
    }

    inline void next_generation() {
        estimate();
        vector<Life> children(config.life_cnt);
        children[0] = *best;
        int cnt = 1;
        while (cnt < config.life_cnt)
            children[cnt++] = multiply();
        
        if (1) {
            sort(lives.begin(), lives.end());
            sort(children.begin(), children.end());
            int half = config.life_cnt >> 1;
            for (int i = half; i < config.life_cnt; i++)
                lives[i] = children[i - half];
        } else {
            lives.clear();
            lives = children;
        }
        generation++;
    }
    
};


