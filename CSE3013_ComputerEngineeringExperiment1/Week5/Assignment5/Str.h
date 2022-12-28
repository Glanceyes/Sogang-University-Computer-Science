#include <iostream>

class Str {
private:
    char *str;
    int len;
public:
    Str(int leng);
    Str(const char *neyong);
    ~Str();
    int length(void);
    char *contents(void);
    int compare(class Str& a);
    int compare(const char *a);
    void operator=(const char *a);
    void operator=(class Str& a);
};