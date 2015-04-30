//
//  Move.h
//  ChineseCheckersAI
//
//  Created by Mohammed Bellifa on 4/20/15.
//  Copyright (c) 2015 Mohammed Bellifa. All rights reserved.
//

#ifndef __ChineseCheckersAI__Move__
#define __ChineseCheckersAI__Move__

#include <stdio.h>
#include <string>
#include <cstdint>

struct Move
{
    unsigned from;
    unsigned to;
    operator uint32_t() const;
    
    operator std::string() const; //conversion operator
};
bool operator==(const Move &lhs, const Move &rhs);
bool operator<(const Move &lhs, const Move &rhs);
std::ostream &operator<<(std::ostream &out, const Move &m);

//class Move {
//    friend bool operator==(const Move &lhs, const Move &rhs);
//    friend bool operator<(const Move &lhs, const Move &rhs);
//    friend std::ostream &operator<<(std::ostream &out, const Move &m);
//public:
//    Move();
//    Move(unsigned _from, unsigned _to);
//    uint16_t getRepresentation() const;
//    operator uint32_t() const;
//    operator std::string() const; //conversion operator
//    unsigned from;
//    unsigned to;
//private:
//
//    unsigned binaryRepresentation; //Stores the 7 bits of from, and then the 7 bits of 2 for easy comparisons.
//    
//};
//bool operator==(const Move &lhs, const Move &rhs);
//bool operator<(const Move &lhs, const Move &rhs);
//std::ostream &operator<<(std::ostream &out, const Move &m);


#endif /* defined(__ChineseCheckersAI__Move__) */
