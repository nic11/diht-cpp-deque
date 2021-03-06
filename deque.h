// Repository: https://github.com/nic11/diht-cpp-deque

#include <cassert>
#include <iterator>
#include <iostream>

namespace nic11 {
    using std::ptrdiff_t;
    template<class T, class Ptr, class Ref>
    class DequeIterator : public std::iterator<std::random_access_iterator_tag, T, ptrdiff_t, Ptr, Ref> {
    public:
        DequeIterator(Ptr buf, size_t bufSize, size_t dataStart, size_t index)
            : mBuf(buf), mBufSize(bufSize), mDataStart(dataStart), mIndex(index)
        {}

        DequeIterator(const DequeIterator &b): mBuf(b.mBuf), mBufSize(b.mBufSize), mDataStart(b.mDataStart), mIndex(b.mIndex) {}

        DequeIterator &operator++() {
            return *this += 1;
        }

        const DequeIterator operator++(int) {
            DequeIterator c(*this);
            ++*this;
            return c;
        }

        DequeIterator &operator--() {
            return *this -= 1;
        }

        const DequeIterator operator--(int) {
            DequeIterator c(*this);
            --*this;
            return c;
        }

        Ref operator*() const {
            return *(mBuf + (mDataStart + mIndex) % mBufSize);
        }

        Ptr operator->() const {
            return &**this;
        }

        DequeIterator &operator-=(ptrdiff_t s) {
            mIndex -= s;
            return *this;
        }

        DequeIterator &operator+=(ptrdiff_t s) {
            mIndex += s;
            return *this;
        }

        const DequeIterator operator-(ptrdiff_t s) const {
            DequeIterator c(*this);
            return c -= s;
        }

        const DequeIterator operator+(ptrdiff_t s) const {
            DequeIterator c(*this);
            return c += s;
        }

        ptrdiff_t operator-(const DequeIterator &b) const {
            return ptrdiff_t(mIndex) - b.mIndex;
        }

        Ref operator[](ptrdiff_t s) const {
            return *(*this + s);
        }

        bool operator==(const DequeIterator &b) const {
            return mBuf == b.mBuf && mIndex == b.mIndex;
        }

        bool operator!=(const DequeIterator &b) const {
            return !(*this == b);
        }

        bool operator<(const DequeIterator &b) const {
            return mIndex < b.mIndex;
        }

        bool operator>(const DequeIterator &b) const {
            return b < *this;
        }

        bool operator<=(const DequeIterator &b) const {
            return !(*this > b);
        }

        bool operator>=(const DequeIterator &b) const {
            return !(*this < b);
        }
    private:
        Ptr mBuf;
        size_t mBufSize, mDataStart, mIndex;
    };

    template<class T>
    class Deque {
    public:
        explicit Deque(size_t n = 0) {
            init(n, T());
        }

        Deque(size_t n, const T &basic) {
            init(n, basic);
        }

        Deque(const Deque &d2) {
            init(d2.size(), T());
            *this = d2;
        }

        ~Deque() {
            delete[] mBuf;
        }

        Deque &operator=(const Deque &d2) {
            if (this != &d2) {
                mDataLast = mDataFirst;
                realloc(d2.size());
                mDataLast = d2.size();
                std::copy(d2.begin(), d2.end(), mBuf);
            }
            return *this;
        }

        size_t size() const {
            return mDataLast < mDataFirst?
                mDataLast + mBufSize - mDataFirst : mDataLast - mDataFirst;
        }

        bool empty() const {
            return !size();
        }

        void push_back(const T &x) {
            before_push();
            mBuf[mDataLast++] = x;
            mDataLast %= mBufSize;
        }

        void push_front(const T &x) {
            before_push();
            mDataFirst = (mDataFirst + mBufSize - 1) % mBufSize;
            mBuf[mDataFirst] = x;
        }

        void pop_back() {
            mDataLast = (mDataLast + mBufSize - 1) % mBufSize;
            after_pop();
        }

        void pop_front() {
            mDataFirst = (mDataFirst + 1) % mBufSize;
            after_pop();
        }

        T &operator[](size_t i) {
            return mBuf[(mDataFirst + i) % mBufSize];
        }

        const T &operator[](size_t i) const {
            return mBuf[(mDataFirst + i) % mBufSize];
        }

        T &back() {
            return (*this)[size() - 1];
        }

        const T &back() const {
            return (*this)[size() - 1];
        }

        T &front() {
            return (*this)[0];
        }

        const T &front() const {
            return (*this)[0];
        }

        typedef DequeIterator<T, T*, T&> iterator;
        typedef DequeIterator<T, const T*, const T&> const_iterator;
        typedef std::reverse_iterator<iterator> reverse_iterator;
        typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

        const iterator begin() {
            return iterator(mBuf, mBufSize, mDataFirst, 0);
        }

        const iterator end() {
            return iterator(mBuf, mBufSize, mDataFirst, size());
        }

        const const_iterator cbegin() const {
            return const_iterator(mBuf, mBufSize, mDataFirst, 0);
        }

        const const_iterator cend() const {
            return const_iterator(mBuf, mBufSize, mDataFirst, size());
        }

        const const_iterator begin() const {
            return cbegin();
        }

        const const_iterator end() const {
            return cend();
        }

        const reverse_iterator rbegin() {
            return reverse_iterator(end());
        }

        const reverse_iterator rend() {
            return reverse_iterator(begin());
        }

        const const_reverse_iterator crbegin() const {
            return const_reverse_iterator(cend());
        }

        const const_reverse_iterator crend() const {
            return const_reverse_iterator(cbegin());
        }

        const const_reverse_iterator rbegin() const {
            return crbegin();
        }

        const const_reverse_iterator rend() const {
            return crend();
        }
    private:
        size_t mBufSize;
        T *mBuf;

        size_t mDataFirst, mDataLast;

        void init(size_t n, const T &basic) {
            if (n < MINIMAL_SIZE) {
                mBufSize = MINIMAL_SIZE;
            } else {
                mBufSize = n;
            }
            mBuf = new T[mBufSize];
            mDataFirst = 0;
            mDataLast = n;

            for (size_t i = 0; i < n; ++i) {
                mBuf[i] = basic;
            }
        }

        void realloc(size_t newSize) {
            if (newSize < MINIMAL_SIZE) {
                newSize = MINIMAL_SIZE;
            }

            T *newBuf = new T[newSize];
            for (size_t i = 0; i < size(); ++i) {
                newBuf[i] = mBuf[(mDataFirst + i) % mBufSize];
            }

            mDataLast = size();
            mBufSize = newSize;
            delete[] mBuf;
            mBuf = newBuf;
            mDataFirst = 0;
        }

        void before_push() {
            if (size() + 1 >= mBufSize) {
                realloc(size() * EXPANSION_COEFFICIENT);
            }
        }

        void after_pop() {
            if (size() * SHRINKAGE_COEFFICIENT < mBufSize) {
                realloc(size() * EXPANSION_COEFFICIENT);
            }
        }

        static const size_t EXPANSION_COEFFICIENT = 2;
        static const size_t SHRINKAGE_COEFFICIENT = 3;
        static const size_t MINIMAL_SIZE = 4;
    };
}

using nic11::Deque;
