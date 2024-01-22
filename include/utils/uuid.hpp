/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   uuid.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maboulkh <maboulkh@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/22 20:19:57 by maboulkh          #+#    #+#             */
/*   Updated: 2024/01/22 22:17:42 by maboulkh         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UUID_HPP
# define UUID_HPP

# include "definition.hpp"

#include <ostream>

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

#endif // UUID_HPP