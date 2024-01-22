/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   uuid.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maboulkh <maboulkh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/21 22:29:56 by maboulkh          #+#    #+#             */
/*   Updated: 2024/01/22 14:27:24 by maboulkh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ctime>
#include <cstdlib>
#include <iostream>
#include <sstream>

using namespace std;

#define SET_BIT(n, x, v) (x = x & ~(1 << n) | (v << n))
#define CHECK_BIT(n, x) ((x >> n) & 1)

template <typename T>
void print_bits(T x) {
    for (int i = 8 * sizeof(T) - 1; i >= 0 ; i--)
        cout << (CHECK_BIT(i, x) ? 1 : 0);
}

template <typename T>
void print_bits(T* x, int n) {
    int count = 0;
    for (int i = 0; i < n * sizeof(T) * 8 ; i++) {
        if (i % (sizeof(*x) * 8) == 0 && i != 0)
            cout << "|";
        cout << 7 - count++ % 8;
    }
    cout << endl;
    for (int i = 0; i < n ; i++) {
        print_bits(*(x + i));
        if (i != n - 1)
            cout << "|";
    }
}

template <typename T>
void set_bit(T* addr, int bitNum, bool val) {
    T* x = addr + bitNum / (sizeof(T) * 8);
    int n = bitNum % (sizeof(T) * 8);
    SET_BIT(n, *x, val);
}

char reversebyte(char byte) {
    char reversedByte = 0;
    for (int i = 0; i < 8; i++) {
        reversedByte |= ((byte >> i) & 1) << (7 - i);
    }
    return reversedByte;
}

void reverseverybit(char* uuid, int size) {
    char help;
    for (int i = 0; i <= size / 2; i++) {
        cout << " reversing byte " << i << " and " << size - i - 1 << endl;
        help = uuid[i];
        uuid[i] = reversebyte(uuid[size - i - 1]);
        if (i >= size / 2)
            break;
        uuid[size - i - 1] = reversebyte(help);
    }
}

struct uuid_t {
    int val[4];
    string str;
};

void fill_uuid(int* uuid) {
    srand(time(NULL) + 113);
    for (int i = 0; i < 4; i++) {
        uuid[i] = rand();
        srand(uuid[i] + 113);
    }
    // set variant
    set_bit(uuid, 3 * 16 + 0, 0);
    set_bit(uuid, 3 * 16 + 1, 0);
    set_bit(uuid, 3 * 16 + 2, 1);
    set_bit(uuid, 3 * 16 + 3, 0);
    //set version
    set_bit(uuid, 4 * 16 + 0, 0);
    set_bit(uuid, 4 * 16 + 1, 1);
    set_bit(uuid, 4 * 16 + 2, 0);
    set_bit(uuid, 4 * 16 + 3, 1);
}

string make_uuid_str(char* uuid) {
    std::stringstream ss;
    int shift;
    int mask;

    shift = 128;
    mask = 0xf;
    while (shift > 0) {
        if (shift == 96 || shift == 80 || shift == 64 || shift == 48)
            ss << "-";
        ss << std::hex << (*uuid & mask);
        ss << std::hex << ((*uuid >> 4) & mask);
        shift -= 8;
        uuid++;
    }
    return ss.str();
}

uuid_t generate_uuid() {
    uuid_t uuid;
    fill_uuid(uuid.val);
    uuid.str = make_uuid_str((char*)uuid.val);
    return uuid;
}

int main() {
    for (int i = 0; i < 100000; i++) {
        
        uuid_t u = generate_uuid();
        u.str = make_uuid_str((char*)u.val);
        cout << u.str << endl;
    }
    return 0;
}
