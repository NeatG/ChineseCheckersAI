//
//  Move.cpp
//  ChineseCheckersAI
//
//  Created by Mohammed Bellifa on 4/20/15.
//  Copyright (c) 2015 Mohammed Bellifa. All rights reserved.
//

#include "Move.h"
#include <sstream>

Move::operator std::string() const
{
    std::stringstream ss;
    ss << "MOVE FROM " << from << " TO " << to;
    return ss.str();
}

bool operator==(const Move &lhs, const Move &rhs)
{
    return lhs.from == rhs.from && lhs.to == rhs.to;
}

// Lexicographic
bool operator<(const Move &lhs, const Move &rhs)
{
    return lhs.from < rhs.from || (!(rhs.from < lhs.from) && lhs.to < rhs.to);
}

std::ostream &operator<<(std::ostream &out, const Move &m)
{
    out << "{" << m.from << " -> " << m.to << "}";
    return out;
}
Move::operator uint32_t() const {
    uint32_t binaryRepresentation = from;
    binaryRepresentation <<= 7;
    binaryRepresentation |= to;
    return binaryRepresentation;
}

//Move::Move() : from(0), to(0) {
//    Move(0,0);
//}
//Move::Move(unsigned _from, unsigned _to) : from(_from), to(_to) {
//    binaryRepresentation = _from;
//    binaryRepresentation <<= 7;
//    binaryRepresentation |= _to;
//}
//
//Move::operator std::string() const {
//    std::stringstream ss;
//    ss << "MOVE FROM " << from << " TO " << to;
//    return ss.str();
//}
//Move::operator uint32_t() const {
//    return binaryRepresentation;
//}
//
//
//bool operator==(const Move &lhs, const Move &rhs) {
//    return lhs.binaryRepresentation == rhs.binaryRepresentation;
//    //  return lhs.from == rhs.from && lhs.to == rhs.to;
//}
//
//// Lexicographic
//bool operator<(const Move &lhs, const Move &rhs) {
//    return lhs.binaryRepresentation < rhs.binaryRepresentation;
//    //return lhs.from < rhs.from || (!(rhs.from < lhs.from) && lhs.to < rhs.to);
//}
//
//std::ostream &operator<<(std::ostream &out, const Move &m) {
//    out << "{" << m.from << " -> " << m.to << "}";
//    return out;
//}
