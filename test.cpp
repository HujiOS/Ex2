#include "uthreads.h"

#include <iostream>

void f (void)
{
	int tid = uthread_get_tid();
	int i = 1;
	while(1)
	{
		if(i == uthread_get_quantums(tid))
		{
			std::cout << "f" << tid << " Quanta:" <<  i << std::endl;
			if (i == 10)
			{
				std::cout << "f END" << std::endl;
				uthread_terminate(tid);
			}
			i++;
		}

	}
}

void dummyf(void)
{
	int tid = uthread_get_tid();
	int i = 1;
	while(1)
	{
		if(i == uthread_get_quantums(tid))
		{
			std::cout << "dummyf" << tid << " Quanta:" <<  i << std::endl;
			if (i == 100)
			{
				std::cout << "dummyf END" << std::endl;
				uthread_terminate(tid);
			}
			i++;
		}

	}
}

void g (void)
{
	int tid = uthread_get_tid();
	int i = 1;
	while(1)
	{
		if(i == uthread_get_quantums(tid))
		{
			std::cout << "g" << tid << " Quanta:" <<  i << std::endl;
			if (i == 10)
			{
				std::cout << "g END" << std::endl;
				uthread_terminate(tid);
			}
			i++;
		}

	}
}
void d (void)
{
	int tid = uthread_get_tid();
	int i = 1;
	while(1)
	{
		if(i == uthread_get_quantums(tid))
		{
			std::cout << "d" << tid << " Quanta:" <<  i << std::endl;
			if(i==5){
				std::cout << "blocking dblock!" << std::endl;
				uthread_block(2);
			}
			if(i == 9){
				uthread_resume(2);
			}
			if (i == 10)
			{
				std::cout << "d END" << std::endl;
				uthread_terminate(tid);
			}
			i++;
		}

	}
}
void dblock (void)
{
	int tid = uthread_get_tid();
	int i = 1;
	while(1)
	{
		if(i == uthread_get_quantums(tid))
		{
			std::cout << "dblock" << tid << " Quanta:" <<  i << std::endl;
			if (i == 10)
			{
				std::cout << "d END" << std::endl;
				uthread_terminate(tid);
			}
			i++;
		}

	}
}


void a (void)
{
	int tid = uthread_get_tid();
	int i = 1;
	while(1)
	{
		if(i == uthread_get_quantums(tid))
		{
			std::cout << "a" << tid << " Quanta:" <<  i << std::endl;
			if(i==5){
				cout << "syncing a to aSync" << endl;
				uthread_sync(1);
			}
			if (i == 10)
			{
				std::cout << "a END" << std::endl;
				uthread_terminate(tid);
			}
			i++;
		}

	}
}

void aSync (void)
{
	int tid = uthread_get_tid();
	int i = 1;
	while(1)
	{
		if(i == uthread_get_quantums(tid))
		{
			std::cout << "aSync" << tid << " Quanta:" <<  i << std::endl;
			if (i == 10)
			{
				std::cout << "aSync END" << std::endl;
				uthread_terminate(tid);
			}
			i++;
		}

	}
}


void b (void)
{
	int tid = uthread_get_tid();
	int i = 1;
	while(1)
	{
		if(i == uthread_get_quantums(tid))
		{
			std::cout << "b" << tid << " Quanta:" <<  i << std::endl;
			if(i==5){
				cout << "syncing b to bSync" << endl;
				uthread_sync(1);
			}
			if (i == 10)
			{
				std::cout << "b END" << std::endl;
				uthread_terminate(tid);
			}
			i++;
		}

	}
}

void bSync (void)
{
	int tid = uthread_get_tid();
	int i = 1;
	while(1)
	{
		if(i == uthread_get_quantums(tid))
		{
			std::cout << "bSync" << tid << " Quanta:" <<  i << std::endl;
			if (i == 10)
			{
				std::cout << "bSync END" << std::endl;
				uthread_terminate(tid);
			}
			i++;
		}

	}
}

void bblock(void){
	int tid = uthread_get_tid();
	int i = 1;
	while(1)
	{
		if(i == uthread_get_quantums(tid))
		{
			std::cout << "bblock" << tid << " Quanta:" <<  i << std::endl;
			if(i==5){
				cout << "blocking threads 1 2"  << endl;
				uthread_block(2);
				uthread_block(1);
			}
			if(i==7){
				cout << "resuming thread 2 "<< endl;
				uthread_resume(2);
			}
			if(i==12){
				cout << "resuming thread 1 "<< endl;
				uthread_resume(1);
			}
			if (i == 15)
			{
				std::cout << "bblock END" << std::endl;
				uthread_terminate(tid);
			}
			i++;
		}

	}
}

int main(void)
{
	try
	{
		cout<<"init status is "<< uthread_init(100) << endl;
		int tid = uthread_get_tid();
		int i = 1;
		std::cout << "Thread:m Number:(0) " << tid << std::endl;
		std::cout << "Init Quantum num is:" << uthread_get_total_quantums() << std::endl;
		while(1)
		{
			if(i == uthread_get_quantums(tid))
			{
				std::cout << "m" << tid << " Quanta:" <<  i << std::endl;
				if (i == 3)
				{
					std::cout << "m spawns f at (1) " << uthread_spawn(f) << std::endl;
					std::cout << "m spawns g at (2) " << uthread_spawn(g) << std::endl;
				}
				if (i == 10)
				{
					std::cout << "m spawns d at (1) " << uthread_spawn(d) << std::endl;
					std::cout << "m spawns dblock at (2) " << uthread_spawn(dblock) << std::endl;
				}
				if(i==26){
					std::cout << "m spawns aSync at (1) " << uthread_spawn(aSync) << std::endl;
					std::cout << "m spawns a at (2) " << uthread_spawn(a) << std::endl;
					std::cout << "m spawns f at (3) " << uthread_spawn(f) << std::endl;
					std::cout << "m spawns g at (4) " << uthread_spawn(g) << std::endl;
				}
				if(i==46){
					std::cout << "m spawns bSync at (1) " << uthread_spawn(bSync) << std::endl;
					std::cout << "m spawns b at (2) " << uthread_spawn(b) << std::endl;
					std::cout << "m spawns bblock at (3) " << uthread_spawn(bblock) << std::endl;
				}
				if(i > 64 && i < 165){
//					uthread_spawn(dummyf);
				}
				if (i == 64)
				{
					std::cout << "Total Quantums: " << uthread_get_total_quantums() << std::endl;
					uthread_terminate(tid);
				}
				i++;
			}
		}
	}
	catch(const std::exception& e)
	{
		std::cout << "Caught The Following Excpetion: \n" << e.what() << std::endl;
	}

}
