#include "deque.h"
#include <gtest/gtest.h>
#include <algorithm>
#include <deque>

using nic11::Deque;

class DequeTest : public testing::Test {};

TEST_F(DequeTest, checkPushBackAndIterator) {
    Deque<int> d;
    for (int i = -24; i < 147; ++i) {
        d.push_back(i);
    }
    ASSERT_EQ(d.size(), size_t(147 + 24));
    Deque<int>::const_iterator it = d.cbegin();
    for (int i = -24; i < 147; ++i, ++it) {
        ASSERT_EQ(*it, i);
    }
}

TEST_F(DequeTest, checkPushFrontAndReverseIterator) {
    Deque<int> d;
    for (int i = -24; i < 147; ++i) {
        d.push_front(i);
    }
    ASSERT_EQ(d.size(), size_t(147 + 24));
    Deque<int>::const_reverse_iterator it = d.crbegin();
    for (int i = -24; i < 147; ++i, ++it) {
        ASSERT_EQ(*it, i);
    }
}

TEST_F(DequeTest, checkIterators) {
    Deque<int> d(100, 1);

    Deque<int>::iterator a = d.begin();
    for (int i = 0; i < 100; ++i) {
        a[i] *= i;
    }

    Deque<int>::reverse_iterator b = d.rbegin();
    for (int i = 0; i < 100; ++i) {
        b[i] *= i;
    }

    ASSERT_EQ(b.base() - a, 100);

    for (int i = 0; i < 100; ++i) {
        ASSERT_EQ(d[i], i * (99 - i));
    }

    Deque<int>::iterator c = d.end() - 1;
    ASSERT_EQ(c[0], 0);
    ASSERT_EQ(*c, 0);
    ASSERT_EQ(c[-1], 98);
    ASSERT_EQ(*(c + (-1)), 98);
    ASSERT_EQ(*(c - 1), 98);
    ASSERT_EQ(c[-50], 50 * (99 - 50));
    ASSERT_EQ(*(c - 50), 50 * (99 - 50));
    ASSERT_EQ(*(c + (-50)), 50 * (99 - 50));

    a[0] = 1337;
    ASSERT_EQ(b[99], 1337);
    ASSERT_EQ(c[-99], 1337);
    ASSERT_EQ(*(c + (-99)), 1337);
    ASSERT_EQ(*(c - 99), 1337);

    ASSERT_EQ(d.begin() + d.size(), d.end());
}

TEST_F(DequeTest, checkWithPairs) {
    typedef std::pair<int, int> Pair;

    Deque<Pair> d(100);
    for (int i = 0; i < 100; ++i) {
        d[i].first = i / 10;
        d[i].second = i % 10;
    }

    for (int i = 0; i < 100; ++i) {
        ASSERT_EQ(d[i].first, i / 10);
        ASSERT_EQ((d.begin() + i)->first, i / 10);
        ASSERT_EQ(d[i].second, i % 10);
        ASSERT_EQ((d.begin() + i)->second, i % 10);
    }

    for (int i = 0; i < 100; ++i) {
        (d.begin() + i)->first = i % 10;
        (d.begin() + i)->second = i / 10;
    }

    for (int i = 0; i < 100; ++i) {
        ASSERT_EQ(d[i].first, i % 10);
        ASSERT_EQ((d.begin() + i)->first, i % 10);
        ASSERT_EQ(d[i].second, i / 10);
        ASSERT_EQ((d.begin() + i)->second, i / 10);
    }

    ASSERT_FALSE(std::is_sorted(d.begin(), d.end()));

    std::sort(d.begin(), d.end(), std::greater<Pair>());
    ASSERT_TRUE(std::is_sorted(d.rbegin(), d.rend()));
}

TEST_F(DequeTest, checkIteratorPostfixIncDec) {
    Deque<int> d(10);
    for (int i = 0; i < 10; ++i) {
        d[i] = i;
    }
    d.push_front(11);
    std::deque<int> sd(d.begin(), d.end());

    ASSERT_TRUE(std::equal(d.begin(), d.end(), sd.begin()));
    ASSERT_TRUE(std::equal(d.cbegin(), d.cend(), sd.begin()));
    ASSERT_TRUE(std::equal(d.rbegin(), d.rend(), sd.rbegin()));
    ASSERT_TRUE(std::equal(d.crbegin(), d.crend(), sd.rbegin()));

    for (Deque<int>::iterator it = d.begin(); it < d.end(); ++it);
    for (Deque<int>::iterator it = d.begin(); it < d.end(); it++);
    for (Deque<int>::const_iterator it = d.cbegin(); it < d.cend(); ++it);
    for (Deque<int>::const_iterator it = d.cbegin(); it < d.cend(); it++);

    for (Deque<int>::reverse_iterator it = d.rbegin(); it < d.rend(); ++it);
    for (Deque<int>::reverse_iterator it = d.rbegin(); it < d.rend(); it++);
    for (Deque<int>::const_reverse_iterator it = d.crbegin(); it < d.crend(); ++it);
    for (Deque<int>::const_reverse_iterator it = d.crbegin(); it < d.crend(); it++);

    int i = 1;
    for (Deque<int>::iterator it = d.begin() + 1; it < d.end() - 1; ++it, ++i) {
        ASSERT_EQ(d.begin() + i, it);
        ASSERT_EQ(*(d.begin() + i), *it);
        ASSERT_EQ(d[i], *it);
    }
}

TEST_F(DequeTest, checkFrontAndBack) {
    Deque<int> d;
    d.push_back(1337);
    ASSERT_EQ(d.front(), 1337);
    ASSERT_EQ(d.back(), 1337);
    d.push_back(0xfeedbea);
    ASSERT_EQ(d.front(), 0xfeedbea);
    ASSERT_NE(d.back(), 1337);

    srand(random_seed());
    for (int i = 0; i < 100500; ++i) {
        int v = rand();
        if (rand() & 1) {
            d.push_front(v);
            ASSERT_EQ(d.front(), v);
            ASSERT_NE(d.back(), v);
        } else {
            d.push_back(v);
            ASSERT_EQ(d.back(), v);
            ASSERT_NE(d.front(), v);
        }
    }
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
