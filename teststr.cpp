#include <string>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#include <sys/types.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <google/sparse_hash_map>
#include "HashUtil.h"

using namespace std;

struct Murmur {
    uint32_t operator()(const char* key) const
    {
        return HashUtil::MurmurHash(key, strlen(key), 42);
    }
};
struct eqstr {
    bool operator()(const char* s1, const char* s2) const
    {
        return (s1 && s2 && strcmp(s1, s2) == 0);
    }
};
    

class TestPAO {
public:
    TestPAO()
    {
        val = 0;
    }
    unsigned int val;
};
    
int getRSS()
{
    int fd, n;
    long size = 0;
    char s[1024];
    char buf[1024];

    n = snprintf(s, 1024, "/proc/%d/stat", getpid());
    if (n < 0) {
        perror("Cannot name proc stat file: ");
        exit(1);
    }
    
    fd = open(s, O_RDONLY);
    if (fd < 0) {
        perror("Cannot open proc stat file: ");
        exit(1);
    }

    n = read(fd, buf, 1024);
    if (n < 0) {
        perror("Cannot read proc stat file: ");
        exit(1);
    }

    char *p = buf;
    for (int i = 0; i < 23; i++) {
        p = index(p, ' ');
        if (!p) {
            perror("Cannot find RSS value: ");
            exit(1);
        }
        p++;
        //printf("%s\n", p);
    }
    n = sscanf(p, "%ld", &size);
    if (n < 0) {
        perror("Cannot parse RSS value: ");
        exit(1);
    }
    close(fd);

    return size * 4096;  // assume 4kB pages
}
    

int main(int argc, char* argv[])
{
    int n = 100000000;
    //char ch;

    if (argc < 2) {
        cout << "option? " << endl;
        exit(1);
    }
    
    int opt = atoi(argv[1]);
    switch(opt) {
        case 0:
            {
                cout << "Allocating " << n << " 8B C++ std::strings" << endl;
                int startRSS = getRSS();
                string* strlist = new string[n];
                for (int i=0; i<n; i++)
                    strlist[i] = "aasdsad";
                //cout << "Hit a key to continue" << endl;
                //ch = getchar(); 
                int endRSS = getRSS();
                printf("Total memory used per-entry = %.2f\n", (endRSS - startRSS)*1.0/n);
                delete[] strlist;
                break;
            }

        case 1:
            {
                cout << "Allocating " << n << " 8B C char array strings" << endl;
                int startRSS = getRSS();
                char** slist = new char*[n];
                for (int i=0; i<n; i++) {
                    slist[i] = new char[8];
                    strcpy(slist[i], "aasdsad");
                }
                //cout << "Hit a key to continue" << endl;
                //ch = getchar(); 
                int endRSS = getRSS();
                printf("Total memory used per-entry = %.2f\n", (endRSS - startRSS)*1.0/n);
                for (int i=0; i<n; i++)
                    delete[] slist[i];
                delete[] slist;
                break;
            }
            //    typedef google::sparse_hash_map<int, TestPAO*, Murmur, eqstr> Hash;

        case 2:
            {
                cout << "Allocating " << n << " int" << endl;
                int startRSS = getRSS();
                int* a = new int[n];
                for (int i=0; i<n; i++)
                    a[i] = 1;
                //cout << "Hit a key to continue" << endl;
                //ch = getchar(); 
                int endRSS = getRSS();
                printf("Total memory used per-entry = %.2f\n", (endRSS - startRSS)*1.0/n);
                delete[] a;
                break;
            }

        case 3:
            {
                cout << "Allocating " << n << " char*" << endl;
                int startRSS = getRSS();
                char** b = new char*[n];
                for (int i=0; i<n; i++)
                    b[i] = NULL;
                //cout << "Hit a key to continue" << endl;
                //ch = getchar(); 
                int endRSS = getRSS();
                printf("Total memory used per-entry = %.2f\n", (endRSS - startRSS)*1.0/n);
                delete[] b;
                break;
            }


        case 4:
            {
                cout << "Allocating " << n << " PAO" << endl;
                int startRSS = getRSS();
                TestPAO* c = new TestPAO[n];
                //cout << "Hit a key to continue" << endl;
                //ch = getchar(); 
                int endRSS = getRSS();
                printf("Total memory used per-entry = %.2f\n", (endRSS - startRSS)*1.0/n);
                delete c;
                break;
            }

        case 5:
            {
                cout << "Inserting " << n << " elements into <int, int> hash" << endl;
                int startRSS = getRSS();
                typedef google::sparse_hash_map<int, int> IntIntHash;
                IntIntHash d;
                for (int i=0; i<n; i++)
                    d[i] = i;
                //cout << "Hit a key to continue" << endl;
                //ch = getchar(); 
                int endRSS = getRSS();
                printf("Total memory used per-entry = %.2f\n", (endRSS - startRSS)*1.0/n);
                d.clear();
                break;
            }

        case 6:
            {
                cout << "Inserting " << n << " elements into <int, PAO*> hash" << endl;
                int startRSS = getRSS();
                typedef google::sparse_hash_map<int, TestPAO*> IntPtrHash;
                IntPtrHash e;
                for (int i=0; i<n; i++)
                    e[i] = NULL;
                //cout << "Hit a key to continue" << endl;
                //ch = getchar(); 
                int endRSS = getRSS();
                printf("Total memory used per-entry = %.2f\n", (endRSS - startRSS)*1.0/n);
                e.clear();
                break;
            }

        case 7:
            {
                cout << "Inserting " << n << " elements into <char*, PAO*> hash with NULL values" << endl;
                int startRSS = getRSS();
                typedef google::sparse_hash_map<char*, TestPAO*, Murmur, eqstr> CharPtrHash;
                CharPtrHash g;
                for (int i=0; i<n; i++) {
                    char *k = new char[8];
                    sprintf(k, "%x", i);
                    g[k] = NULL;
                }
                //cout << "Hit a key to continue" << endl;
                //ch = getchar(); 
                int endRSS = getRSS();
                printf("Total memory used per-entry = %.2f\n", (endRSS - startRSS)*1.0/n);
                g.clear();
                break;
            }

        case 8:
            {
                cout << "Inserting " << n << " elements into <char*, PAO*> hash with PAO values" << endl;
                int startRSS = getRSS();
                typedef google::sparse_hash_map<char*, TestPAO*, Murmur, eqstr> CharPtrHash;
                CharPtrHash g;
                for (int i=0; i<n; i++) {
                    char *k = new char[8];
                    TestPAO* p = new TestPAO();
                    sprintf(k, "%x", i);
                    g[k] = p;
                }
                //cout << "Hit a key to continue" << endl;
                //ch = getchar(); 
                int endRSS = getRSS();
                printf("Total memory used per-entry = %.2f\n", (endRSS - startRSS)*1.0/n);
                g.clear();
                break;
            }

        case 9:
            {
                cout << "Allocating " << n << " ints with indirection" << endl;
                int startRSS = getRSS();
                int** f = new int*[n];
                for (int i=0; i<n; i++) {
                    f[i] = new int;
                    *f[i] = 5;
                }
                //cout << "Hit a key to continue" << endl;
                //ch = getchar(); 
                int endRSS = getRSS();
                printf("Total memory used per-entry = %.2f\n", (endRSS - startRSS)*1.0/n);
                for (int i=0; i<n; i++)
                    delete f[i];
                delete[] f;
                break;
            }
    }
}
