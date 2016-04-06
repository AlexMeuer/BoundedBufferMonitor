template <typename T>
BoundedBuffer<T>::BoundedBuffer(size_t maxBufferSize) :
not_full_(BUFFER_SIZE),
not_empty_(0)
{
}

template <typename T>
void BoundedBuffer<T>::deposit(T item) {
	while (count_ == BUFFER_SIZE - 1) {
		not_full_.wait();
	}

	buffer_[rear_] = item;
	if (rear_ < BUFFER_SIZE) {
		rear_ = (rear_ + 1) % BUFFER_SIZE;
	}

	++count_;

	not_empty_.post();
}

template <typename T>
T BoundedBuffer<T>::fetch() {
	
	while (count_ == 0) {
		not_empty_.wait();
	}

	T result = buffer_[front_];

	if (front_ < BUFFER_SIZE) {
		front_ = (front_ + 1) % BUFFER_SIZE;
	}

	--count_;

	not_full_.post();

	return result;
}

template <typename T>
bool BoundedBuffer<T>::empty() const {
	return count_;
}