# rigid_list
A version of std::list that only allocates memory when you tell it to.

##EXAMPLE

```
rigid_list<int> list(10);
printf("capacity = %d, size = %d, free size = %d.\n",
       (int)list.capacity(), (int)list.size(), (int)list.free_size() );
//prints out: capacity = 10, size = 0, free size = 10.
int i = 0;
while( list.push_back(i) ) { i++; }
printf("The last element that fit in the list was %d.\n", *(--list.end()) );
//prints out: The last element that fit in the list was 9.
```

rigid_list is the same as std::list except that it only uses preallocated memory and fails to grow when it runs out of it.
Use rigid_list::reserve() to preallocate memory for a given number of list elements.
hen you can rigid_list::push_back() up to the reserved number of elements and they will be stored in the list
without the list allocating any new memory.
If you push_back() more elements than you've reserved() space for, they will not be added and push_back() will return false.

You can increase the capacity of the list by calling reserve( m ) where m > n and continue push_back()-ing more elements into it.

##BEWARE! WORK IN PROGRESS!

Not all methods of std::list and their various versions are implemented.
If a method or a version of it is not documented, it isn't implemented, so don't use it!
It may still be inherited from std::list but it will probably not play nice with rigid_list.
With that said, these inherited methods ARE safe to use:

- begin(), end(), cbegin(), cend(), rbegin(), rend(), crbegin(), crend()
- front(), back()
- empty()
- size()
- sort()

##PERFORMANCE NOTE:

In gcc, use at least -O2 to make rigid_list outperform std::list. With -O1, rigid_list is terribly slow. (tested with gcc 4.8.4).
