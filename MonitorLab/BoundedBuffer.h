#ifndef BOUNDED_BUFFER_H
#define BOUNDED_BUFFER_H

#include <array>
#include <boost/interprocess/sync/interprocess_semaphore.hpp>

#if !defined BUFFER_SIZE
#define BUFFER_SIZE 10
#endif

//Simple Bounded buffer implementation
template <typename T>
class BoundedBuffer {
public:

	BoundedBuffer<T>(size_t maxBufferSize);

	void deposit(T item);

	T fetch();

	bool empty() const;

private:

	int front_ = 0;
	int rear_ = 0;
	int count_ = 0;

	std::array<T, BUFFER_SIZE> buffer_;

	boost::interprocess::interprocess_semaphore not_full_;
	boost::interprocess::interprocess_semaphore not_empty_;
};

#include "BounderBuffer.inl"
#endif
