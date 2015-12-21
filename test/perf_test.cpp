#include <stdio.h>
#include <sys/time.h>
#include <string>

#include "../include/rigid_list.h"

typedef struct timeval timevalT;

class Runner
{
public:
	Runner( int _count ) { SetCount( _count ); };
	double Result() { return timevalToSeconds( endTime ) - timevalToSeconds( startTime ); };
	double Run() { init(); start(); action(); stop(); cleanup(); return Result(); };
	void SetCount( int _count ) { count = _count; };
protected:
	virtual void action() {};
	virtual void init() {};
	virtual void cleanup() {};
	int count;
private:	
	inline void start() { gettimeofday(&startTime, NULL); };
	inline void stop() { gettimeofday(&endTime, NULL); };
	inline double timevalToSeconds( timevalT &t ) {  return (double)t.tv_sec + ((double)t.tv_usec/(double)1000000.0); };

	timevalT	startTime;
	timevalT endTime;
};

template <class listT, class itemT> class PushBackRunner : public Runner
{
public:
	PushBackRunner( listT &_list, int _count = 1000 ) : Runner( _count ) { list = &_list; };
protected:
	void action() {
		itemT	item;
		for( int i = count; i > 0; i-- ) {
			list->push_back( item );
		}
	}

private:
	listT	*list;
};

void race( Runner &a, Runner &b, int count )
{
	a.SetCount( count );
	b.SetCount( count );
	a.Run();
	b.Run();
	printf("count: %d\nrigid: %f\n  std: %f\n", count, a.Result(), b.Result() );
	
}

int main() {

	rigid_list<std::string> rl;
	std::list<std::string> sl;

	PushBackRunner<rigid_list<std::string>, std::string> rrun( rl );
	PushBackRunner<std::list<std::string>, std::string> srun( sl );

	rl.reserve( 10 );
	race( rrun, srun, 10 );

	rl.reserve( 100 );
	race( rrun, srun, 100 );

	rl.reserve( 1000 );
	race( rrun, srun, 1000 );

	return 0;
}
