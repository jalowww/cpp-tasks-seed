#include <algorithm>
#include <vector>
#include <gtest/gtest.h>

#include "sorting.h"

namespace
{
template <typename Sort>
void check_sort(Sort sort)
{
    std::vector<std::vector<int>> cases =
    {
        {},
        {1},
        {2, 1},
        {1, 2, 3, 4, 5},
        {5, 4, 3, 2, 1},
        {3, 1, 2, 3, 1, 2},
        {-5, 10, 0, -1, 7, 7, 3},
        {42, 42, 42, 42},
    };

    for (auto data : cases)
    {
        auto expected = data;
        std::sort(expected.begin(), expected.end());

        sort(data.begin(), data.end());

        EXPECT_EQ(data, expected);
    }
}

}

TEST(BubbleSort, SortsCorrectly)
{
    check_sort([](auto begin, auto end)
    {
        bubble_sort(begin, end);
    });
}

TEST(QuickSort, SortsCorrectly)
{
    check_sort([](auto begin, auto end)
    {
        quick_sort(begin, end);
    });
}
