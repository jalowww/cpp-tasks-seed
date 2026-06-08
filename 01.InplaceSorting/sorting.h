#ifndef SORTING_H
#define SORTING_H

#include <utility>

#include "collvalue.h"

template <typename Iterator>
void bubble_sort(Iterator begin, Iterator end)
{
  for (Iterator last = end; last != begin; --last)
    {
        bool swapped = false;

        for (Iterator it = begin; it + 1 != last; ++it)
        {
            Iterator next = it + 1;

            if (*next < *it)
            {
                using std::swap;
                swap(*it, *next);
                swapped = true;
            }
        }

        if (!swapped)
        {
            return;
        }
    }
}

template <typename Iterator>
void quick_sort(Iterator begin, Iterator end)
{
  if (end - begin < 2)
    {
        return;
    }
    auto pivot = *(begin + (end - begin) / 2);
    Iterator less_end = begin;
    Iterator current = begin;
    Iterator greater_begin = end;
    while (current != greater_begin)
    {
        if (*current < pivot)
        {
            using std::swap;
            swap(*less_end, *current);
            ++less_end;
            ++current;
        }
        else if (pivot < *current)
        {
            --greater_begin;
            using std::swap;
            swap(*current, *greater_begin);
        }
        else
        {
            ++current;
        }
    }
    quick_sort(begin, less_end);
    quick_sort(greater_begin, end);
}

#endif
