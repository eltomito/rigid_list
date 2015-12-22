#ifndef _rigid_list_h_
#define _rigid_list_h_ 1

#include <list>
#include <stddef.h>
#include <cstdlib>

/**
 * \mainpage
 * A version of std::list that only allocates memory when you tell it to.
 *
 * EXAMPLE:
 * @code
 * rigid_list<int> list(10);
 * printf("capacity = %d, size = %d, free size = %d.\n", (int)list.capacity(), (int)list.size(), (int)list.free_size() );
 * //prints out: capacity = 10, size = 0, free size = 10.
 * int i = 0;
 * while( list.push_back(i) ) { i++; }
 * printf("The last element that fit in the list was %d.\n", *(--list.end()) );
 * //prints out: The last element that fit in the list was 9.
 * @endcode
 *
 * This is the same as std::list except that it only uses preallocated memory and fails to grow when it runs out of it.
 * Use rigid_list::reserve() to preallocate memory for a given number of list elements.
 * Then you can rigid_list::push_back() up to the reserved number of elements and they will be stored in the list
 * without the list allocating any new memory.
 * If you push_back() more elements than you've reserved() space for, they will not be added and push_back() will return false.
 *
 * You can increase the capacity of the list by calling reserve( m ) where m > n and continue push_back()-ing more elements into it.
 *
 * BEWARE!
 * Not all methods of std::list and their various versions are implemented.
 * If a method or a version of it is not documented here, it isn't implemented, so don't use it!
 * It may still be inherited from std::list but it will probably not play nice with this container.
 * With that said, these inherited methods ARE safe to use:
 *
 * - begin(), end(), cbegin(), cend(), rbegin(), rend(), crbegin(), crend()
 * - front(), back()
 * - empty()
 * - size()
 * - sort()
 *
 * PERFORMANCE NOTE:
 * In gcc use at least -O2 to make rigid_list outperform std::list. With -O1, rigid_list is terribly slow. (tested with gcc 4.8.4).
 */
template <class T> class rigid_list : public std::list<T>
{
private:
	typedef	std::list<T> free_listT;
public:
	/** Constructor - n is the number of elements to preallocate.
	*/
	rigid_list( size_t n = 0 ) : std::list<T>() { reserve( n ); };

	/** Preallocate memory for n list elements.
	 * @param n - The number of elements memory should be allocated for.
	 *
	 * If n is smaller than the current size of the list,
    * elements at the end that don't fit in the new capacity will be dropped.
	 */
	void reserve( size_t n )
	{
		size_t c = capacity();
		if( n > c ) {
			T dummyItem;
			free_list.insert( free_list.end(), (n-c), dummyItem );
		} else if( n < c ) {
			size_t shrinkBy = c - n;
			if(free_size() >= shrinkBy) {
				typename free_listT::iterator itn = free_list.begin();
				std::advance( itn, shrinkBy );
				free_list.erase( free_list.begin(), itn );
			} else {
				shrinkBy -= free_size();
				free_list.clear();
				typename rigid_list<T>::iterator itn = this->end();
				for( int i = 0; i < shrinkBy; i++ ) { itn--; };
				((std::list<T> *)this)->erase( itn, this->end() );
			}
		}
	};

	/** Add an element to the end of the list.
 	 * @returns true if there was enough space to add the element, false otherwise.
	 */
	bool push_back( const T &item )
	{
		if( free_size() > 0 ) {
			(*free_list.begin()) = item;
			((std::list<T> *)this)->splice( this->end(), free_list, free_list.begin() );
			return true;
		}
		return false;
	};

	/** Add an element to the beginning of the list.
 	 * @returns true if there was enough space to add the element, false otherwise.
	 */
	bool push_front( const T &item )
	{
		if( free_size() > 0 ) {
			(*free_list.begin()) = item;
			((std::list<T> *)this)->splice( this->begin(), free_list, free_list.begin() );
			return true;
		}
		return false;
	};

	/** Insert one element at a given position.
	* @returns iterator pointing to the new element or this->end() if there was no space to add it.
	*/
	typename rigid_list<T>::iterator insert( typename rigid_list<T>::iterator pos, const T &item )
	{
		if( this->free_size() == 0 ) { return this->end(); }	
		(*free_list.begin()) = item;
		((std::list<T> *)this)->splice( pos, free_list, free_list.begin() );
		pos--;
		return pos;
	};

	/** Insert a number of copies of an element at a given position.
	* @returns true if there was enough free space to do it, false otherwise.
 	*/
	bool insert( typename rigid_list<T>::iterator pos, size_t count, const T& item )
	{
		if( this->free_size() < count ) { return false; }	
		typename std::list<T>::iterator it = free_list.begin();
		for( int i = count; i > 0; i-- ) {
			(*it) = item;
			it++;
		}
		((std::list<T> *)this)->splice( pos, free_list, free_list.begin(), it );
		return true;
	};

	/** Moves one element from a position in a different (or same) list to a position in this one.
	*
	* When splicing elements from the same list, it works exactly the same as in a std::list.
	* If the source list is different, then the operation is only carried out
	* if capacities of both lists can be preserved, i.e., if the destination list
	* doesn't have enough free space to exchange for the new element, the operation fails.
	*
	* @returns true if splicing from the same list or if there was enough free elements to exchange for the new ones.
	*          False if splicing from another list and not enough free space was available.
   */
	bool splice( typename rigid_list<T>::iterator pos, rigid_list<T> &other, typename rigid_list<T>::iterator it )
	{
		if( ( other != (*this) )&&( this->free_size() == 0 )) { return false; }
		typename std::list<T>::iterator stdpos = (typename std::list<T>::iterator)pos;
		typename std::list<T>::iterator stdit = (typename std::list<T>::iterator)it;
		((std::list<T> *)this)->splice( stdpos, (std::list<T> &)other, stdit );

		if( other != (*this) ) {
			other.free_list.splice( other.free_list.begin(), *this, free_list.begin() );
		}
		return true;
	};

	bool splice( typename rigid_list<T>::iterator pos, rigid_list<T> &other, typename rigid_list<T>::iterator first, typename rigid_list<T>::iterator last )
	{
		size_t count = abs(std::distance( first, last));
		if( ( other != (*this) )&&( this->free_size() < count )) { return false; }
		typename std::list<T>::iterator stdpos = (typename std::list<T>::iterator)pos;
		typename std::list<T>::iterator stdfirst = (typename std::list<T>::iterator)first;
		typename std::list<T>::iterator stdlast = (typename std::list<T>::iterator)last;
		((std::list<T> *)this)->splice( stdpos, (std::list<T> &)other, stdfirst, stdlast );

		if( other != (*this) ) {
			typename std::list<T>::iterator lastFree = free_list.begin();
			std::advance( lastFree, count );
			other.free_list.splice( other.free_list.begin(), *this, free_list.begin(), lastFree );
		}
		return true;
	};

	/** Deletes the last element from the list.
	*/
	void pop_back()
	{
		if( this->empty() ) { return; }
		typename rigid_list<T>::iterator it = this->end();
		it--;
		free_list.splice( free_list.end(), (*this), it );
	};

	/** Deletes the first element from the list.
	*/
	void pop_front()
	{
		if( this->empty() ) { return; }
		free_list.splice( free_list.end(), (*this), this->begin() );
	};

	/** Erase one element from the list.
	* The capacity of the list is unaffected.
	*/
	void erase( typename rigid_list<T>::iterator it ) {
		free_list.splice( free_list.end(), *this, it );
	};

	/** Erase several elements from the list.
	* The capacity of the list is unaffected.
	*/
	void erase( typename rigid_list<T>::iterator its, typename rigid_list<T>::iterator ite ) {
		free_list.splice( free_list.end(), *this, its, ite );
	};

	/** Erase all elements from the list.
	* The capacity of the list is unaffected.
	*/
	void clear() {
		this->erase( this->begin(), this->end() );
	};

	/** Report the maximum number of elements this list can currently hold.
	* The capacity can be changed by calling reserve().
	*/
	size_t capacity() { return this->size() + free_list.size(); };

	/** Report the number of elements that can still be added to this list before its capacity is filled up.
	*/
	size_t free_size() { return free_list.size(); };

protected:
	free_listT	free_list;
};

#endif //_rigid_list_h_
