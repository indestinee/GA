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
#include <thread>


typedef vector<int> Gene;
typedef unsigned long long U64;
typedef long double LD;

const int MUTATION_TIMES = 1, THREAD_NUM = 16, RAND_MAX_BITS = 31;
const long long RAND2_MAX = 1LL << 62;
const bool FITTEST = true;
thread t[THREAD_NUM];

inline U64 rand2() {/*{{{*/
    return (U64(rand())) << RAND_MAX_BITS | rand();
}/*}}}*/
inline LD random(const LD &l, const LD &r) {/*{{{*/
    return LD(rand2()) / LD(RAND2_MAX) * (r - l) + l;
}/*}}}*/

inline void mutation(Gene &new_gene) {/*{{{*/
    for (int i = 0; i < MUTATION_TIMES; i++) {
        int n = new_gene.size(), a = rand() % n, b = rand() % n;
        swap(new_gene[a], new_gene[b]);
    }
}/*}}}*/
inline void reverse(Gene &new_gene) {/*{{{*/
    int n = new_gene.size(), a = rand() % n, b = rand() % n;
    if (a > b) swap(a, b);
    reverse(new_gene.begin() + a, new_gene.begin() + ++b);
}/*}}}*/
inline void print(const Gene &gene) {/*{{{*/
    printf("Gene: ");
    int m = gene.size();
    for (int i = 0; i < m; i++)
        printf("%d%c", gene[i], (i + 1 == m) ? '\n' : ' ');
}/*}}}*/

struct Life{/*{{{*/
    Gene gene;
    double score;
    Life() { }
    Life (const Gene &gene) : gene(gene), score(-1) { }
    inline void out() {
        /* print(gene); */
        printf("Score = %.10f, dis = %.10f\n\n", score, 1.0 / score);
    }
    inline friend bool operator < (const Life &a, const Life &b) {
        return a.score > b.score;
    }
};/*}}}*/

inline void print(Life a) {/*{{{*/
    a.out();
}/*}}}*/
inline void cross(const Life &father, const Life &mother, Gene &new_gene) {/*{{{*/
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
}   /*}}}*/


struct Config{/*{{{*/
    double corss_rate, mutation_rate;
    int life_cnt, gene_len;
    double (*fun) (Life&);
    Config () { }
    Config (
        const double &corss_rate,
        const double &mutation_rate,
        const int &life_cnt,
        const int &gene_len,
        double (*const fun) (Life&)
    ) : corss_rate(corss_rate), mutation_rate(mutation_rate), \
        life_cnt(life_cnt), gene_len(gene_len), fun(fun) { }
};/*}}}*/
class GA;
inline void work(GA *ga, const int &first);
class GA{/*{{{*/
public:
    vector<Life> lives;
    vector<Life> children;
    Life *best;
    double sum;
    int generation;
    Config config;
private:
    inline void load(string s) {
        if (s == "") 
            return;
        ifstream fin;
        fin.open(s.c_str(), ios_base::binary);
        if (fin.is_open()) {
            int each = lives[0].gene.size() * sizeof(int), n = lives.size();
            char *data = (char *) malloc(each * n); 
            fin.read(data, n * each);
            /* printf("Read %d * %d Byte..\n", each, n); */
            for (int i = 0; i < n; i++)
                memcpy(lives[i].gene.data(), data + i * each, each);
        } else {
            printf("Open %s failed..\n", s.c_str());
        }
    }
    inline void init_population(string s) {
        children.resize(config.life_cnt);
        generation = 0;
        lives.clear();
        Gene gene(config.gene_len);
        for (int i = 0; i < config.gene_len; i++)
            gene[i] = i;
        for (int i = 0; i < config.life_cnt; i++) {
            random_shuffle(gene.begin(), gene.end());
            lives.push_back(Life(gene));
        }
        load(s);
    }
public:
    void save(string s = "save.bin") {
        ofstream fout;
        fout.open(s.c_str(), ios_base::binary);
        if (!fout.is_open()) {
            puts("[ERR] Open data.bin failed");
            exit(-1);
        }
        int each = lives[0].gene.size() * sizeof(int), n = lives.size();
        char *data = (char *) malloc(each * n); 
        for (int i = 0; i < n; i++)
            memcpy(data + i * each, lives[i].gene.data(), each);
        /* printf("Dump %d * %d Byte..\n", each, n); */
        fout.write(data, n * each);
    }
    GA(const Config &config, const string &s = "") : config(config) {
        init_population(s);
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
        if (random(0, 1) < 0.5)
            reverse(gene);
        return Life(gene);
    }
    
    inline void next_generation() {
        estimate();
        sort(lives.begin(), lives.end());
        children[0] = *best;
        
        /* while (cnt < config.life_cnt) */ 
            /* children[cnt++] = multiply(); */
        for (int i = 0; i < THREAD_NUM; i++)
            t[i] = thread(work, this, i + 1);
        for (int i = 0; i < THREAD_NUM; i++)
            t[i].join();
        
        if (FITTEST) {
            sort(children.begin(), children.end());
            int half = config.life_cnt * 0.45;
            for (int i = half; i < config.life_cnt; i++)
                lives[i] = children[i - half];
            for (int i = config.life_cnt * 0.9; i < config.life_cnt; i++)
                random_shuffle(lives[i].gene.begin(), lives[i].gene.end());
        } else {
            lives.clear();
            lives = children;
        }
        generation++;
    }
    
};/*}}}*/


inline void work(GA *ga, const int &first) {
    for (int i = first; i < ga->config.life_cnt; i += THREAD_NUM) {
        ga->children[i] = ga->multiply();
    }
}
