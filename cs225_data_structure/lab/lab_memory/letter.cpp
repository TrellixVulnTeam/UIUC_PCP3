/**
 * @file letter.cpp
 */

#include "letter.h"
using namespace std;

Letter::Letter() : letter('-'), count(0)
{

}

void Letter::addStudent()
{
    count++;
}

bool Letter::operator<(const Letter & other) const
{
    return count > other.count;
}
