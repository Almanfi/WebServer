/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   uuid.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fdiraa <fdiraa@student.1337.ma>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/21 22:29:56 by maboulkh          #+#    #+#             */
/*   Updated: 2024/02/25 13:13:27 by fdiraa           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ctime>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <cstring>

using namespace std;


// #define SET_BIT(n, x, v) (x = x & ~(1 << n) | (v << n))
// #define CHECK_BIT(n, x) ((x >> n) & 1)

// template <typename T>
// class bitManip {
// public:
//     static void print_bits(T x);
//     static void print_bits(T* x, int n);
//     static void set_bit(T* addr, int bitNum, bool val);
//     static char reversebyte(char byte);
//     static void reverseBits(T* T_uuid, int T_nbr);
// private:
//     bitManip();
//     ~bitManip();
// };

// class UUID {
// public:
//     UUID();
//     UUID(const string& uuid_str);
//     UUID(const int* uuid_val);
//     ~UUID();
//     UUID(const UUID& other);
//     UUID& operator=(const UUID& other);
//     const string& getStr() const;
//     const int* getVal() const;
//     bool isSame(const UUID& other) const;
//     bool isSame(const string& other) const;
//     bool isSame(const int* other) const;
//     bool operator==(const UUID& other) const;
//     bool operator==(const string& other) const;
//     bool operator==(const int* other) const;
//     bool operator!=(const UUID& other) const;
//     bool operator!=(const string& other) const;
//     bool operator!=(const int* other) const;

//     static void generate(int* uuid);
//     static string strGen(char* uuid);
//     static void strToVal(const string uuid_str, char* uuid);
// private:
//     int val[4];
//     string str;
//     static long secret;
// };

// ostream& operator<<(ostream& os, const UUID& uuid);

#define SET_BIT(n, x, v) (x = x & ~(1 << n) | (v << n))
#define CHECK_BIT(n, x) ((x >> n) & 1)

template <typename T>
class bitManip {
public:
    static void print_bits(T x);
    static void print_bits(T* x, int n);
    static void set_bit(T* addr, int bitNum, bool val);
    static char reversebyte(char byte);
    static void reverseBits(T* T_uuid, int T_nbr);
private:
    bitManip();
    ~bitManip();
};


template <typename T>
void bitManip<T>::print_bits(T x) {
    for (int i = 8 * sizeof(T) - 1; i >= 0 ; i--)
       // -- cout << (CHECK_BIT(i, x) ? 1 : 0);
}

template <typename T>
void bitManip<T>::print_bits(T* x, int n) {
    int count = 0;
    for (int i = 0; i < n * sizeof(T) * 8 ; i++) {
        if (i % (sizeof(*x) * 8) == 0 && i != 0)
           // -- cout << "|";
       // -- cout << 7 - count++ % 8;
    }
   // -- cout << endl;
    for (int i = 0; i < n ; i++) {
        print_bits(*(x + i));
        if (i != n - 1)
           // -- cout << "|";
    }
}

template <typename T>
void bitManip<T>::set_bit(T* addr, int bitNum, bool val) {
    T* x = addr + bitNum / (sizeof(T) * 8);
    int n = bitNum % (sizeof(T) * 8);
    SET_BIT(n, *x, val);
}

template <typename T>
char bitManip<T>::reversebyte(char byte) {
    char reversedByte = 0;
    for (int i = 0; i < 8; i++) {
        reversedByte |= ((byte >> i) & 1) << (7 - i);
    }
    return reversedByte;
}

template <typename T>
void bitManip<T>::reverseBits(T* T_uuid, int T_nbr) {
    char*   uuid = (char*) T_uuid;
    char    help;
    int size = sizeof(T) * T_nbr;
    for (int i = 0; i <= size / 2; i++) {
       // -- cout << " reversing byte " << i << " and " << size - i - 1 << endl;
        help = uuid[i];
        uuid[i] = reversebyte(uuid[size - i - 1]);
        if (i >= size / 2)
            break;
        uuid[size - i - 1] = reversebyte(help);
    }
}


class UUID {
public:
    UUID();
    UUID(const string& uuid_str);
    UUID(const int* uuid_val);
    ~UUID();
    UUID(const UUID& other);
    UUID& operator=(const UUID& other);
    const string& getStr() const;
    const int* getVal() const;
    bool isSame(const UUID& other) const;
    bool isSame(const string& other) const;
    bool isSame(const int* other) const;
    bool operator==(const UUID& other) const;
    bool operator==(const string& other) const;
    bool operator==(const int* other) const;
    bool operator!=(const UUID& other) const;
    bool operator!=(const string& other) const;
    bool operator!=(const int* other) const;

    static void generate(int* uuid);
    static string strGen(char* uuid);
    static void strToVal(const string uuid_str, char* uuid);
private:
    int val[4];
    string str;
    static long secret;
};

ostream& operator<<(ostream& os, const UUID& uuid);

long UUID::secret = 113;

void UUID::generate(int* uuid) {
    for (int i = 0; i < 4; i++) {
        secret += rand() % 839;
        srand(time(NULL) + secret);
        uuid[i] = rand();
    }
    // set variant
    bitManip<int>::set_bit(uuid, 3 * 16 - 1, 0);
    bitManip<int>::set_bit(uuid, 3 * 16 - 2, 1);
    bitManip<int>::set_bit(uuid, 3 * 16 - 3, 0);
    bitManip<int>::set_bit(uuid, 3 * 16 - 4, 0);
    //set version
    bitManip<int>::set_bit(uuid, 6 * 16 - 4, 0);
    bitManip<int>::set_bit(uuid, 6 * 16 - 3, 1);
    bitManip<int>::set_bit(uuid, 6 * 16 - 2, 0);
    bitManip<int>::set_bit(uuid, 6 * 16 - 1, 1);
}

string UUID::strGen(char* uuid) {
    std::stringstream ss;
    int shift;
    int mask;

    shift = 128;
    mask = 0xf;
    int l = 0;
    uuid -= 4;
    while (shift > 0) {
        if (l++ % 4 == 0)
            uuid += 7;
        else
            uuid -= 1;
        if (shift == 48 || shift == 64 || shift == 80 || shift == 96)
            ss << "-";
        int c;
        c = ((*uuid >> 4) & mask);
        ss << std::hex << c;
        c = (*uuid & mask);
        ss << std::hex << c;
        shift -= 8;
    }
    return ss.str();
}

void UUID::strToVal(const string uuid_str, char* uuid) {
    const char* hex = "0123456789abcdef";
    int l = 0;
    uuid -= 4;
    int val = 0;
    for (int i = 0; i < uuid_str.size(); i++) {
        if (uuid_str[i] == '-')
            continue;
        if (l++ % 4 == 0)
            uuid += 7;
        else
            uuid -= 1;
        val = std::strchr(hex , uuid_str[i]) - hex;
        SET_BIT(4, *uuid, (val & 0xf));
        i++;
        val = std::strchr(hex , uuid_str[i]) - hex;
        SET_BIT(0, *uuid, (val & 0xf));
    }    
}

UUID::UUID() {
    std::memset(val, 0, sizeof(val));
    generate(val);
    str = strGen((char*)val);
}

UUID::UUID(const string& uuid_str) {
    str = uuid_str;
    std::memset(val, 0, sizeof(val));
    strToVal(str, (char*)val);
}

UUID::UUID(const int* uuid_val) {
    for (int i = 0; i < 4; i++)
        val[i] = uuid_val[i];
    str = strGen((char*)val);
}

UUID::~UUID() {
}

UUID::UUID(const UUID& other) {
    *this = other;
}

UUID& UUID::operator=(const UUID& other) {
    if (this != &other) {
        for (int i = 0; i < 4; i++)
            val[i] = other.val[i];
        str = other.str;
    }
    return *this;
}

const string& UUID::getStr() const {
    return str;
}

const int* UUID::getVal() const {
    return val;
}

bool UUID::isSame(const UUID& other) const {
    for (int i = 0; i < 4; i++) {
        if (val[i] != other.val[i])
            return false;
    }
    return true;
}

bool UUID::isSame(const string& other) const {
    return (str == other);
}

bool UUID::isSame(const int* other) const {
    for (int i = 0; i < 4; i++) {
        if (val[i] != other[i])
            return false;
    }
    return true;
}

bool UUID::operator==(const UUID& other) const {
    return isSame(other);
}

bool UUID::operator==(const string& other) const {
    return isSame(other);
}

bool UUID::operator==(const int* other) const {
    return isSame(other);
}

bool UUID::operator!=(const UUID& other) const {
    return !isSame(other);
}

bool UUID::operator!=(const string& other) const {
    return !isSame(other);
}

bool UUID::operator!=(const int* other) const {
    return !isSame(other);
}

ostream& operator<<(ostream& os, const UUID& uuid) {
    os << uuid.getStr();
    return os;
}

//  generate_uuid() {
//     uuid_t uuid;
//     fill_uuid(uuid.val);
//     uuid.str = make_uuid_str((char*)uuid.val);
//     return uuid;
// }

int main() {
    int a = 8743892;

    int b = a ^ 0xffffffff;

    bitManip<int>::print_bits(a);
   // -- cout << endl;
    bitManip<int>::reverseBits(&a, 1);
    bitManip<int>::print_bits(b);
   // -- cout << endl;
    
    // UUID u;
    //// -- cout << u << endl;
    //// -- cout    << u.getVal()[0] << "|"
    //         << u.getVal()[1] << "|"
    //         << u.getVal()[2] << "|"
    //         << u.getVal()[3] << endl;
    // UUID u2;
    //// -- cout << u2 << endl;
    //// -- cout    << u2.getVal()[0] << "|"
    //         << u2.getVal()[1] << "|"
    //         << u2.getVal()[2] << "|"
    //         << u2.getVal()[3] << endl;
    // if (u != u2.getVal())
    //    // -- cout << "not same" << endl;
    // else
    //    // -- cout << "same" << endl;
    return 0;
}
