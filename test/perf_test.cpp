#include <stdio.h>
#include <sys/time.h>
#include <string>

#include "../include/rigid_list.h"

typedef struct timeval timevalT;

class Runner
{
public:
	Runner( const char *_name, int _count ) { name = _name; SetCount( _count ); };
	double Result() { return timevalToSeconds( endTime ) - timevalToSeconds( startTime ); };
	double Run() { init(); start(); action(); stop(); cleanup(); return Result(); };
	void SetCount( int _count ) { count = _count; };
	std::string name;
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
	PushBackRunner( const char *_name, listT &_list, int _count = 0 ) : Runner( _name, _count ) { list = &_list; };
protected:
	void action() {
		itemT	item;
		for( int i = count; i > 0; i-- ) {
			list->push_back( item );
		}
	}

protected:
	listT	*list;
};

class PushBackRigidRunner : public PushBackRunner<rigid_list<std::string>, std::string>
{
public:
	PushBackRigidRunner( const char *_name, rigid_list<std::string> &_list, int _count = 0 ) : PushBackRunner( _name, _list, _count ) {};
protected:
	void init() { list->reserve( count ); };
};

class Race
{
public:
	Race( const char *_name, Runner &a, Runner &b ) { name = _name; runnerA = &a; runnerB = &b; };
	void Go( int count ) { lastCount = count; runnerA->SetCount( count ); runnerB->SetCount( count ); runnerA->Run(); runnerB->Run(); PrintResults(); };
	void PrintResults() { printf("---\nRace: %s, count = %d\n1) %s: %f\n2) %s: %f\n", name.c_str(), lastCount, runnerA->name.c_str(), runnerA->Result(), runnerB->name.c_str(), runnerB->Result() );
								std::string *winnerName;
								double winnerFactor;
								if( runnerA->Result() < runnerB->Result() ) {
									winnerName = &runnerA->name;
									winnerFactor = runnerB->Result() / runnerA->Result();
								} else {
									winnerName = &runnerB->name;
									winnerFactor = runnerA->Result() / runnerB->Result();
								}
								printf("Winner: %s (%f x faster)\n", winnerName->c_str(), winnerFactor );
	};
	std::string name;
protected:
	Runner *runnerA;
	Runner *runnerB;
	int lastCount;
};

int main() {

	rigid_list<std::string> rl;
	std::list<std::string> sl;

	PushBackRigidRunner rrun( "rigid_list", rl );
	PushBackRunner<std::list<std::string>, std::string> srun( "std_list", sl );

	Race pushBackRace( "push_back()", rrun, srun );
	pushBackRace.Go( 10 );
	pushBackRace.Go( 100 );
	pushBackRace.Go( 1000 );
	pushBackRace.Go( 10000 );

	return 0;
}
