#include <stdio.h>
#include <utility>
#include <string>

#include "rigid_list.h"
#include "string.h"

std::string _current_test_name;
#define TEST_NAME(_name) do { _current_test_name = _name; printf("Testing \"%s\"...\n", _current_test_name.c_str() ); } while(0);

#define MASSERT( condition, ... ) if( !(condition) ) { printf("*** ERROR!!! Test \"%s\" FAILED!\n", _current_test_name.c_str()); printf( __VA_ARGS__ ); printf("\n"); };

#define MASSERT_LIST_STATE( _list, _size, _capacity ) MASSERT( ((_list).size() == _size)&&((_list).free_size() == (_capacity-_size))&&((_list).capacity() == _capacity), \
"expected: size = %d, free_size = %d, capacity = %d\n\
  actual: size = %d, free_size = %d, capacity = %d", _size, _capacity-_size, _capacity, (int)_list.size(), (int)_list.free_size(), (int)_list.capacity() );

#define MASSERT_PUSHBACK_RESULT( _list, _size, _capacity, _pushBackActual, _pushBackExpected ) \
MASSERT_LIST_STATE( _list, _size, _capacity ) \
MASSERT( _pushBackActual == _pushBackExpected, "push_back() should return %s but didn't!", _pushBackExpected ? "TRUE":"FALSE" )

#define MASSERT_STRING_AT(_list, pos, expected) MASSERT( 0 == strcmp( itn(_list,pos)->c_str() ,expected) ,"The string at %d should be \"%s\" but it's \"%s\".", pos, expected, itn(_list,pos)->c_str() )

#define PRINT_LIST_STATE( _list ) printf("size = %d, capacity = %d, free_size = %d\n", (int)_list.size(), (int)_list.capacity(), (int)_list.free_size() );

typedef rigid_list<std::string> listT;

listT::iterator itn( listT &l, size_t n ) {
	listT::iterator it = l.begin();
	listT::iterator endit = l.end();
	for( int i = n; i--; i > 0 ) {
		if( it == endit ) { break; }
		it++;
	}
	return it;
}

#define PUSH_BACK_STR(_list, _str) do { s = _str; bRes = _list.push_back(s); } while(0);

void test_spliceOne() {
	listT l;
	bool bRes;
	std::string s;
	l.reserve(10);

	TEST_NAME("splice one element")

	s = "banana";
	l.insert( l.end(), 5, s );
	s = "pineapple";
	l.insert( l.end(), 5, s );

	MASSERT_STRING_AT(l, 5, "pineapple");
	MASSERT_STRING_AT(l, 2, "banana");

	l.splice( itn(l,2), l, itn(l,9) );
	MASSERT_STRING_AT(l, 2, "pineapple");
	MASSERT_STRING_AT(l, 5, "banana");
	MASSERT_LIST_STATE( l,10,10 )

	listT k;
	k.reserve(5);
	PUSH_BACK_STR( k, "carrot")
	PUSH_BACK_STR( k, "beetroot")
	PUSH_BACK_STR( k, "kohlrabi")

	MASSERT_LIST_STATE( k,3,5 )
	MASSERT_LIST_STATE( l,10,10 )

	k.splice( itn(k,1), l, itn(l,6) );

	MASSERT_LIST_STATE( k,4,5 )
	MASSERT_LIST_STATE( l,9,10 )
	MASSERT_STRING_AT(k, 1, "pineapple");
}

void test_spliceMany() {
	listT l;
	bool bRes;
	std::string s;
	l.reserve(10);

	TEST_NAME("splice several elements")

	s = "banana";
	l.insert( l.end(), 5, s );
	s = "pineapple";
	l.insert( l.end(), 5, s );

	listT k;
	k.reserve(5);
	PUSH_BACK_STR( k, "carrot")
	PUSH_BACK_STR( k, "beetroot")
	PUSH_BACK_STR( k, "kohlrabi")

	bRes = k.splice( itn(k,2), l, itn(l, 3), itn(l,7) );
	MASSERT( !bRes, "splicing more elements than the list can hold should fail!");
	
	k.reserve(7);
	bRes = k.splice( itn(k,2), l, itn(l, 3), itn(l,7) );
	MASSERT( bRes, "splicing to fill list up should work!");
	MASSERT_STRING_AT( k,0, "carrot")
	MASSERT_STRING_AT( k,1, "beetroot")
	MASSERT_STRING_AT( k,2, "banana")
	MASSERT_STRING_AT( k,3, "banana")
	MASSERT_STRING_AT( k,4, "pineapple")
	MASSERT_STRING_AT( k,5, "pineapple")
	MASSERT_STRING_AT( k,6, "kohlrabi")
}


int main() {
	listT l;
	bool bRes;
	std::string s;

	test_spliceOne();
	test_spliceMany();

	TEST_NAME("reserve and push back")
	
	l.reserve(3);
	MASSERT_LIST_STATE( l,0,3 )

	PUSH_BACK_STR( l, "ahoj")
	MASSERT_PUSHBACK_RESULT( l,1,3, bRes, true )
	PUSH_BACK_STR( l, "pepo")
	MASSERT_PUSHBACK_RESULT( l,2,3, bRes, true )
	PUSH_BACK_STR( l, "venco")
	MASSERT_PUSHBACK_RESULT( l,3,3, bRes, true )
	PUSH_BACK_STR( l, "franto")
	MASSERT_PUSHBACK_RESULT( l,3,3, bRes, false )
	l.reserve(5);
	MASSERT_LIST_STATE( l,3,5 )
	PUSH_BACK_STR( l, "joudo")
	MASSERT_PUSHBACK_RESULT( l,4,5, bRes, true )

	MASSERT_STRING_AT( l,0,"ahoj" )
	MASSERT_STRING_AT( l,1,"pepo" )
	MASSERT_STRING_AT( l,2,"venco" )
	MASSERT_STRING_AT( l,3,"joudo" )

	l.sort();

	MASSERT_STRING_AT( l,0,"ahoj" )
	MASSERT_STRING_AT( l,1,"joudo" )
	MASSERT_STRING_AT( l,2,"pepo" )
	MASSERT_STRING_AT( l,3,"venco" )

	MASSERT_LIST_STATE( l,4,5 )

	TEST_NAME("erase and push back")
	
	l.erase( itn(l,1), itn(l,3) );
	MASSERT_STRING_AT( l,0,"ahoj" )
	MASSERT_STRING_AT( l,1,"venco" )
	PUSH_BACK_STR( l, "kamile")
	MASSERT_PUSHBACK_RESULT( l,3,5, bRes, true )

	l.erase( itn(l,1) );
	MASSERT_STRING_AT( l,0,"ahoj" )
	MASSERT_STRING_AT( l,1,"kamile" )

	PUSH_BACK_STR( l, "rudo")
	PUSH_BACK_STR( l, "cendo")
	PUSH_BACK_STR( l, "ignaci")

	l.erase( itn(l,0) );
	MASSERT_STRING_AT( l,0,"kamile" )
	MASSERT_LIST_STATE( l,4,5 )

	TEST_NAME("pop back")

	l.pop_back();
	MASSERT_LIST_STATE( l,3,5 )
	MASSERT_STRING_AT( l,2,"cendo" )
	l.pop_back();
	PUSH_BACK_STR( l, "jardo")
	l.pop_back();
	l.pop_back();
	MASSERT_LIST_STATE( l,1,5 )
	l.pop_back();
	MASSERT_LIST_STATE( l,0,5 )
	l.pop_back();
	MASSERT_LIST_STATE( l,0,5 )

	TEST_NAME("insert and erase")
	s = "blazeji";
	l.push_front(s);
	s = "isztvane";
	l.push_front(s);
	s = "gadzo";
	l.push_front(s);
	s = "bedrichu";
	l.insert( itn(l,2),s );
	MASSERT_LIST_STATE( l,4,5 )
	MASSERT_STRING_AT( l,2,"bedrichu" )

	s = "Zdarec";
	l.insert( itn(l,0),s );
	MASSERT_LIST_STATE( l,5,5 )
	l.erase( itn(l,4) );
	l.insert( itn(l,0),s );
	MASSERT_LIST_STATE( l,5,5 )
	MASSERT_STRING_AT( l,0,"Zdarec" )

	TEST_NAME("insert multiple")

	s = "honzo";
	l.pop_back();
	bRes = l.insert( itn(l,4), 1, s );
	MASSERT_LIST_STATE( l,5,5 )
	MASSERT_STRING_AT( l,4,"honzo" )

	bRes = l.insert( itn(l,0), 3, s );
	MASSERT( !bRes, "Inserting stuff into a full list should fail!");

	l.erase( l.begin(), itn(l,3) );
	bRes = l.insert( itn(l,0), 3, s );
	MASSERT_LIST_STATE( l,5,5 )
	MASSERT_STRING_AT( l,0,"honzo" )
	MASSERT_STRING_AT( l,1,"honzo" )
	MASSERT_STRING_AT( l,2,"honzo" )

	TEST_NAME("clear")
	l.clear();
	MASSERT_LIST_STATE( l,0,5 )

	return 0;
}
