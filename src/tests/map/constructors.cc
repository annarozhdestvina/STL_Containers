#include <gtest/gtest.h>

#include <stack>
#include <list>
#include <vector>
#include <deque>
#include <map>

#include "../../s21_map.h"
#include "../item.h"
#include "../map_helpers.h"

// GCOVR_EXCL_START

namespace
{


TEST(Map, T0DefaultConstructor)
{
    s21::Map<int, Item> s21_map;
    std::map<int, Item> map;

    // std::cout << s21_map << '\n';

    // for (auto it = map.cbegin(); it != map.cend(); ++it)
    //     std::cout << "{" << it->first << " : " << it->second << "}\n";

    // EXPECT_EQ(s21_map, map);
}

TEST(Map, T1Destructor)
{
    s21::Map<int, Item> s21_map;
    // s21_map.Insert(std::make_pair(4, Item()));
    // s21_map.Insert(std::make_pair(3, Item()));
    // s21_map.Insert(std::make_pair(5, Item()));

    // EXPECT_EQ(s21_map, map);
}

TEST(Map, T2Destructor)
{
    s21::Map<int, Item> s21_map;
    s21_map.Insert(std::make_pair(4, Item()));

    std::map<int, Item> map;
    map.insert(std::make_pair(4, Item()));


    EXPECT_EQ(s21_map, map);

    // s21::Set<int> s21_set;
    // s21_set.Insert(4);
    // s21_set.Insert(1);
    // s21_set.Insert(0);
    // s21_set.Insert(1);
    // s21_set.Insert(2);
    // s21_set.Insert(4);

    // std::cout << s21_set << '\n';
}





TEST(Map, T0ConstructorInitializer)
{
    s21::Map<int, Item> s21_map {
        {1, Item(1, 'a', 0.1)},
        {2, Item(2, 'b', 0.2)},
        {3, Item(3, 'c', 0.3)},
        {4, Item(4, 'd', 0.4)},
        {5, Item(5, 'e', 0.5)},
        {6, Item(6, 'f', 0.6)},
    };
    std::map<int, Item> map {
        {1, Item(1, 'a', 0.1)},
        {2, Item(2, 'b', 0.2)},
        {3, Item(3, 'c', 0.3)},
        {4, Item(4, 'd', 0.4)},
        {5, Item(5, 'e', 0.5)},
        {6, Item(6, 'f', 0.6)},
    };

    EXPECT_EQ(s21_map, map);
}





TEST(Map, T0CopyConstructor)
{
    const s21::Map<int, Item> s21_source {
        {1, Item(1, 'a', 0.1)},
        {2, Item(2, 'b', 0.2)},
        {3, Item(3, 'c', 0.3)},
        {4, Item(4, 'd', 0.4)},
        {5, Item(5, 'e', 0.5)},
        {6, Item(6, 'f', 0.6)},
    };
    const std::map<int, Item> source {
        {1, Item(1, 'a', 0.1)},
        {2, Item(2, 'b', 0.2)},
        {3, Item(3, 'c', 0.3)},
        {4, Item(4, 'd', 0.4)},
        {5, Item(5, 'e', 0.5)},
        {6, Item(6, 'f', 0.6)},
    };

    s21::Map<int, Item> s21_copy = s21_source;  // copy constructor, not operator=
    std::map<int, Item> copy = source;



    EXPECT_EQ(s21_copy, copy);
}

}  // namespace

// GCOVR_EXCL_STOP
