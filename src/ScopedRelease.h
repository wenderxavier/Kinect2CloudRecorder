#pragma once

template <typename T>
class ScopedRelease
{
public:
	ScopedRelease();
	void set(T* newptr);
	T*& get() { return ptr; }
	~ScopedRelease();

	T* operator->();
	T& operator*();

private:
	T* ptr;
	
	// copy constructor and operator shouldn't be called for this class
	ScopedRelease(const ScopedRelease<T>&);
    ScopedRelease<T>& operator=(const ScopedRelease<T>&);

};

template <typename T>
ScopedRelease<T>::ScopedRelease() : ptr(NULL) {}

template <typename T>
void ScopedRelease<T>::set(T* newptr) { ptr = newptr; }

template <typename T>
ScopedRelease<T>::~ScopedRelease()
{
	if (ptr != NULL)
	{
		ptr->Release();
	}
}

template <typename T>
T* ScopedRelease<T>::operator->()
{
	if (ptr == NULL)
		abort();

	return ptr;
}

template <typename T>
T& ScopedRelease<T>::operator*()
{
	if (ptr == NULL)
		abort();
		
	return *ptr;
}

template <typename T>
void SafeRelease(T*& ptr)
{
	if (ptr != NULL)
	{
		ptr->Release();
		ptr = NULL;
	}
}