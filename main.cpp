#include <iostream>
#include <vector>
#include <cstring>
#include <algorithm>

using namespace std;

#define BASE 1000000000
class My_arithmetic;
My_arithmetic abs_add(vector<int32_t> const &a, vector<int32_t> const &b, bool minus);
My_arithmetic abs_sub(vector<int32_t> const &a, vector<int32_t> const &b, bool minus);
My_arithmetic abs_mul(vector<int32_t> const &a, vector<int32_t> const &b, bool minus);
pair<My_arithmetic, My_arithmetic> abs_div_short(vector<int32_t> const &a, int32_t const &b, bool minus);
pair<My_arithmetic, My_arithmetic> abs_div_long(My_arithmetic a, My_arithmetic b, bool minus);
My_arithmetic near_sqrt(My_arithmetic const &a);

class My_arithmetic {
    vector<int32_t> number;
    bool minus = false;
    bool NaN = false;
public:
    explicit My_arithmetic() {
        NaN = true;
    }

    explicit My_arithmetic(char *s) {
        if (strlen(s) > 0) {
            if (s[0] == '-' || s[0] == '+') {
                if (s[0] == '-') {
                    minus = true;
                }
                s++;
            }
            size_t size = strlen(s);
            size_t part_size = size / 9;
            if (size % 9 != 0) {
                part_size++;
            }
            number.resize(part_size);
            size_t i = 0;
            while (size > 9) {
                s[size] = 0;
                size -= 9;
                number[i++] = atol(s + size);
            }
            s[size] = 0;
            number[i] = atol(s);
            lead_zero();
        } else {
            NaN = true;
        }
    }

    explicit My_arithmetic(vector<int32_t> v, bool minus) {
        if (!v.empty()) {
            this->number.swap(v);
            this->minus = minus;
            lead_zero();
        } else {
            NaN = true;
        }
    }

    explicit My_arithmetic(int32_t a) {
        if (a < 0) {
            minus = true;
            a *= -1;
        }
        if ((a / BASE) > 0) {
            number.push_back(a % BASE);
            a /= BASE;
        }
        number.push_back(a);
    }

    bool is_zero() {
        return (number.size() == 1 && number[0] == 0);
    }

    void lead_zero() {
        size_t size = number.size();
        for (size_t i = 0; i < size - 1; i++) {
            if (number.back() == 0) {
                number.pop_back();
            }
        }
        if (is_zero()) {
            minus = false;
        }
    }

    bool is_negative() {
        return (!is_zero() && minus);
    }

    bool foutput(FILE *f) {
        int check;
        if (!NaN) {
            if (minus) {
                if ((check = fprintf(f, "-")) != 1) {
                    printf("Write %i arguments when wanted (1)", check);
                    return false;
                }
            }
            if ((check = fprintf(f, "%i", number[number.size() - 1])) <= 0) {
                printf("Write %i arguments. Wanted more", check);
                return false;
            }
            for (size_t i = 1; i < number.size(); i++) {
                if ((check = fprintf(f, "%09i", number[number.size() - i - 1])) <= 0) {
                    printf("Write %i arguments. Wanted more", check);
                    return false;
                }
            }
        } else {
            if ((check = fprintf(f, "NaN")) != 3) {
                printf("Write %i arguments. Wanted more", check);
                return false;
            }
        }
        return true;
    }

    int32_t abs_compare(My_arithmetic const &b) {
        if (number.size() == b.number.size()) {
            size_t size = number.size();
            size_t i;
            for (i = 0; i < size; i++) {
                if (number[size - i - 1] != b.number[size - i - 1]) {
                    break;
                }
            }
            if (i == size) {
                return 0;
            }
            return (number[size - i - 1] > b.number[size - i - 1]) ? 1 : -1;
        } else {
            return (number.size() > b.number.size()) ? 1 : -1;
        }
    }

    int32_t compare(My_arithmetic const &b) {
        if (minus && !b.minus) {
            return -1;
        } else if (!minus && b.minus) {
            return 1;
        }
        int32_t res = abs_compare(b);
        if (minus && b.minus) {
            return -1 * res;
        } else {
            return res;
        }
    }

    bool operator==(My_arithmetic const &b) {
        if (!NaN && !b.NaN) {
            int32_t res = compare(b);
            return (res == 0);
        } else {
            return false;
        }
    }

    bool operator<(My_arithmetic const &b) {
        if (!NaN && !b.NaN) {
            int32_t res = compare(b);
            return (res == -1);
        } else {
            return false;
        }
    }

    bool operator>(My_arithmetic const &b) {
        if (!NaN && !b.NaN) {
            int32_t res = compare(b);
            return (res == 1);
        } else {
            return false;
        }
    }

    bool operator!=(My_arithmetic const &b) {
        if (!NaN && !b.NaN) {
            int32_t res = compare(b);
            return (res != 0);
        } else {
            return false;
        }
    }

    bool operator<=(My_arithmetic const &b) {
        if (!NaN && !b.NaN) {
            int32_t res = compare(b);
            return (res == 0 || res == -1);
        } else {
            return false;
        }
    }

    bool operator>=(My_arithmetic const &b) {
        if (!NaN && !b.NaN) {
            int32_t res = compare(b);
            return (res == 0 || res == 1);
        } else {
            return false;
        }
    }

    My_arithmetic operator+(My_arithmetic const &b) {
        if (!NaN && !b.NaN) {
            if ((minus && b.minus) || (!minus && !b.minus)) {
                return abs_add(number, b.number, minus);
            } else {
                if (abs_compare(b) == 1) {
                    return abs_sub(number, b.number, minus);
                } else {
                    return abs_sub(b.number, number, b.minus);
                }
            }
        } else {
            return My_arithmetic();
        }
    }

    My_arithmetic operator-(My_arithmetic const &b) {
        if (!NaN && !b.NaN) {
            if ((minus && !b.minus) || (!minus && b.minus)) {
                return abs_add(number, b.number, minus);
            } else {
                if (abs_compare(b) == 1) {
                    return abs_sub(number, b.number, minus);
                } else {
                    return abs_sub(b.number, number, !minus);
                }
            }
        } else {
            return My_arithmetic();
        }
    }

    My_arithmetic operator-(){
        if (!NaN) {
            return My_arithmetic(number, !minus);
        } else {
            return My_arithmetic();
        }
    }

    My_arithmetic operator*(My_arithmetic const &b) {
        if (!NaN && !b.NaN) {
            return abs_mul(number, b.number, minus ^ b.minus);
        } else {
            return My_arithmetic();
        }
    }

    My_arithmetic operator/(My_arithmetic const &b) {
         if (NaN || b.NaN || (b.number.size() == 1 && b.number[0] == 0)) {
             return My_arithmetic();
         }
        switch (abs_compare(b)) {
            case 1:
                if (b.number.size() == 1) {
                    return abs_div_short(number, b.number[0], minus ^ b.minus).first;
                } else {
                    return abs_div_long(*this, b, minus ^ b.minus).first;
                }
            case -1:
                return My_arithmetic(0);
            default:
                int32_t ans = 1;
                if (minus ^ b.minus) {
                    ans *= -1;
                }
                return My_arithmetic(ans);
        }
    }

    My_arithmetic operator%(My_arithmetic const &b) {
        if (NaN || b.NaN || (b.number.size() == 1 && b.number[0] == 0)) {
            return My_arithmetic();
        }
        switch (abs_compare(b)) {
            case 1:
                if (b.number.size() == 1) {
                    return abs_div_short(number, b.number[0], minus ^ b.minus).second;
                } else {
                    return abs_div_long(*this, b, minus ^ b.minus).second;
                }
            case -1:
                return My_arithmetic(number, minus ^ b.minus);
            default:
                return My_arithmetic(0);
        }
    }

    My_arithmetic sqrt() {
        if (is_negative()) {
            return My_arithmetic();
        } else if (is_zero()) {
            return My_arithmetic(0);
        } else {
            return near_sqrt(*this);
        }
    }
};

My_arithmetic abs_add(vector<int32_t> const &a, vector<int32_t> const &b, bool minus) {
    vector<int32_t> v;
    int32_t carry = 0;
    size_t i, min_size, max_size;
    min_size = min(a.size(), b.size());
    max_size = max(a.size(), b.size());
    v.resize(max_size + 1, 0);
    for (i = 0; i < min_size; i++) {
        v[i] = a[i] + b[i] + carry;
        carry = v[i] / BASE;
        v[i] %= BASE;
    }
    if (a.size() == min_size) {
        for (; i < max_size; i++) {
            v[i] = b[i] + carry;
            carry = v[i] / BASE;
            v[i] %= BASE;
        }
    } else {
        for (; i < max_size; i++) {
            v[i] = a[i] + carry;
            carry = v[i] / BASE;
            v[i] %= BASE;
        }
    }
    if (carry > 0) {
        v.back() = carry;
    }
    My_arithmetic res(v, minus);
    res.lead_zero();
    return res;
}

My_arithmetic abs_sub(vector<int32_t> const &a, vector<int32_t> const &b, bool minus) {
    vector<int32_t> v;
    int32_t carry = 0;
    size_t i, min_size, max_size;
    min_size = min(a.size(), b.size());
    max_size = max(a.size(), b.size());
    v.resize(max_size, 0);
    for (i = 0; i < min_size; i++) {
        v[i] = a[i] - b[i] - carry;
        if (v[i] < 0) {
            carry = 1;
            v[i] += BASE;
        } else {
            carry = 0;
        }
    }
    if (a.size() == min_size) {
        for (; i < max_size; i++) {
            v[i] = b[i] - carry;
            if (v[i] < 0) {
                carry = 1;
                v[i] += BASE;
            } else {
                carry = 0;
            }
        }
    } else {
        for (; i < max_size; i++) {
            v[i] = a[i] - carry;
            if (v[i] < 0) {
                carry = 1;
                v[i] += BASE;
            } else {
                carry = 0;
            }
        }
    }
    My_arithmetic res(v, minus);
    res.lead_zero();
    return res;
}

My_arithmetic abs_mul(vector<int32_t> const &a, vector<int32_t> const &b, bool minus) {
    vector<int32_t> v;
    v.resize(a.size() + b.size(), 0);
    int32_t carry = 0;
    for (size_t i = 0; i < a.size(); i++) {
        for (size_t j = 0; j < b.size(); j++) {
            long long tmp = v[i + j] + carry + (long long) a[i] * (long long) b[j];
            v[i + j] = (int32_t) (tmp % BASE);
            carry = (int32_t) (tmp / BASE);
        }
        for (size_t j = b.size(); carry > 0; j++) {
            v[i + j] += carry;
            carry = v[i + j] / BASE;
            v[i + j] %= BASE;
        }
    }
    My_arithmetic res(v, minus);
    res.lead_zero();
    return res;
}

pair<My_arithmetic, My_arithmetic> abs_div_short(vector<int32_t> const &a, int32_t const &b, bool minus) {
    int32_t carry = 0;
    vector<int32_t> v;
    v.resize(a.size(), 0);
    size_t size = a.size();
    for (size_t i = 0; i < size; i++) {
        long long tmp = a[size - i - 1] + (long long) carry * 1000000000LL;
        v[size - i - 1] = (int32_t) (tmp / b);
        carry = (int32_t) (tmp % b);
    }
    My_arithmetic div(v, minus);
    div.lead_zero();
    if (minus) {
        carry *= -1;
    }
    My_arithmetic mod(carry);
    return {div, mod};
}

pair<My_arithmetic, My_arithmetic> abs_div_long(My_arithmetic a, My_arithmetic b, bool minus) {
    if (a.is_negative()) {
        a = -a;
    }
    if (b.is_negative()) {
        b = -b;
    }
    My_arithmetic left(1);
    My_arithmetic right = a;
    My_arithmetic two(2);
    My_arithmetic res = (left + right) / two;
    My_arithmetic tmp = res * b;
    while (!(tmp <= a && a < (tmp + b))) {
        if (tmp <= a) {
            left = res;
        } else {
            right = res;
        }
        res = (left + right) / two;
        tmp = res * b;
    }
    if (minus) {
        return {-res, -(a - tmp)};
    } else {
        return {res, a - tmp};
    }
}

My_arithmetic near_sqrt(My_arithmetic const &a) {
    My_arithmetic left(1);
    My_arithmetic right = a;
    My_arithmetic two(2);
    My_arithmetic one(1);
    My_arithmetic res = (left + right) / two;
    My_arithmetic wrong_tmp = (res + one) * (res + one);
    My_arithmetic tmp = res * res;
    while (!(tmp <= a && wrong_tmp > a)) {
        if (tmp <= a) {
            left = res;
        } else {
            right = res;
        }
        res = (left + right) / two;
        wrong_tmp = (res + one) * (res + one);
        tmp = res * res;
    }
    return res;
}

int main(int argc, char** argv) {
    if (argc < 3) {
        printf("Wait 2 arguments: names of input and output files, but have %i argument(s)", argc - 1);
        return -1;
    }
    FILE *f;
    if ((f = fopen(argv[1], "r")) == nullptr) {
        printf("File %s didn't find", argv[1]);
        return -1;
    }
    const size_t SIZE = 1000000000;
    char *s;
    char command[3];
    if (!(s = (char*) malloc(SIZE  * sizeof(char)))) {
        printf("Fail in memory allocation");
        fclose(f);
        return -1;
    }
    int check;
    if ((check = fscanf(f, "%s", s)) != 1) {
        printf("Read %i arguments when wanted (1)", check);
        free(s);
        fclose(f);
        return -1;
    }
    try {
        My_arithmetic a(s);
        if ((check = fscanf(f, "%s", command)) != 1) {
            printf("Read %i arguments when wanted (1)", check);
            free(s);
            fclose(f);
            return -1;
        }
        if (strcmp(command, "#") == 0) {
            fclose(f);
            f = nullptr;
            if ((f = fopen(argv[2], "w")) == nullptr) {
                printf("File %s didn't find", argv[2]);
                free(s);
                return -1;
            }
            My_arithmetic res = a.sqrt();
            if (!res.foutput(f)) {
                free(s);
                fclose(f);
                return -1;
            }
        } else {
            if ((check = fscanf(f, "%s", s)) != 1) {
                printf("Read %i arguments when wanted (1)", check);
                free(s);
                fclose(f);
                return -1;
            }
            fclose(f);
            f = nullptr;
            if ((f = fopen(argv[2], "w")) == nullptr) {
                printf("File %s didn't find", argv[2]);
                free(s);
                return -1;
            }
            try {
                My_arithmetic b(s);
                if (strcmp(command, "==") == 0) {
                    if ((check = fprintf(f, "%i", (a == b))) != 1) {
                        printf("Write %i arguments when wanted (1)", check);
                        free(s);
                        fclose(f);
                        return -1;
                    }
                } else if (strcmp(command, "<") == 0) {
                    if ((check = fprintf(f, "%i", (a < b))) != 1) {
                        printf("Write %i arguments when wanted (1)", check);
                        free(s);
                        fclose(f);
                        return -1;
                    }
                } else if (strcmp(command, ">") == 0) {
                    if ((check = fprintf(f, "%i", (a > b))) != 1) {
                        printf("Write %i arguments when wanted (1)", check);
                        free(s);
                        fclose(f);
                        return -1;
                    }
                } else if (strcmp(command, "<=") == 0) {
                    if ((check = fprintf(f, "%i", (a <= b))) != 1) {
                        printf("Write %i arguments when wanted (1)", check);
                        free(s);
                        fclose(f);
                        return -1;
                    }
                } else if (strcmp(command, ">=") == 0) {
                    if ((check = fprintf(f, "%i", (a >= b))) != 1) {
                        printf("Write %i arguments when wanted (1)", check);
                        free(s);
                        fclose(f);
                        return -1;
                    }
                } else if (strcmp(command, "!=") == 0) {
                    if ((check = fprintf(f, "%i", (a != b))) != 1) {
                        printf("Write %i arguments when wanted (1)", check);
                        free(s);
                        fclose(f);
                        return -1;
                    }
                } else if (strcmp(command, "+") == 0) {
                    My_arithmetic res = a + b;
                    if (!res.foutput(f)) {
                        free(s);
                        fclose(f);
                        return -1;
                    }
                } else if (strcmp(command, "-") == 0) {
                    My_arithmetic res = a - b;
                    if (!res.foutput(f)) {
                        free(s);
                        fclose(f);
                        return -1;
                    }
                } else if (strcmp(command, "*") == 0) {
                    My_arithmetic res = a * b;
                    if (!res.foutput(f)) {
                        free(s);
                        fclose(f);
                        return -1;
                    }
                } else if (strcmp(command, "/") == 0) {
                    My_arithmetic res = a / b;
                    if (!res.foutput(f)) {
                        free(s);
                        fclose(f);
                        return -1;
                    }
                } else if (strcmp(command, "%") == 0) {
                    My_arithmetic res = a % b;
                    if (!res.foutput(f)) {
                        free(s);
                        fclose(f);
                        return -1;
                    }
                }
            } catch (bad_alloc const &e) {
                printf("Problem with memory allocation or underflow in operation");
                free(s);
                fclose(f);
                return -1;
            }
        }
    } catch (bad_alloc const &e) {
        printf("Problem with memory allocation or underflow in operation");
        free(s);
        fclose(f);
        return -1;
    }
    free(s);
    fclose(f);
    return 0;
}