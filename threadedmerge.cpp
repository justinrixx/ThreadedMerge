/***************************************************************
* Program: A multi-threaded merge sort implementation
*
* Compile with: g++ -std=c++11 -pthread threadedmerge.cpp
****************************************************************/

#include <iostream>
#include <cstdlib>
#include <cassert>
#include <thread>
#include <time.h>

#define MAX_NUMBER 3000 // the largest random number generated
#define NUM_THREADS 4 // the number of threads to use

using namespace std;

/* Merge sort implementation */
template <class T>
void sortMerge(T * array, int numItems) throw (const char *);

/* Generate an array of random numbers */
int * randomInts(int quantity);

/* Split everything into threads for sorting */
void threadedSort(int numItems, int pieces, int * toSort);

/********************************************************
 * Main
 * Generate some random numbers and sort the list with threads
 *******************************************************/
int main(int argc, char const *argv[])
{
	int numItems;
	int numThreads;

	if (argc == 2)
	{
		numItems = atoi(argv[1]);
		numThreads = NUM_THREADS; // use the default number
	}
	else if (argc == 3)
	{
		numItems = atoi(argv[1]);
		numThreads = atoi(argv[2]);
	}
	else
	{
		cout << "Usage: " << argv[0] << " <number of items> <number of threads>" << endl;
		return 0;
	}

	int * toSort = randomInts(numItems);

	cout << "Before sorting:" << endl;
	for (int i = 0; i < numItems; i++)
		cout << toSort[i] << " ";
	cout << endl;

	threadedSort(numItems, 4, toSort);

	cout << "After sorting:" << endl;
	for (int i = 0; i < numItems; i++)
		cout << toSort[i] << " ";
	cout << endl;

	return 0;
}

/********************************************************
 * threadedSort
 * Split the array into pieces and sort each pieces on a
 * separate thread
 *******************************************************/
void threadedSort(int numItems, int pieces, int * toSort)
{
	int iCurrent = 0;
	int size = numItems;

	std::thread threads[pieces];

	// create a thread for each piece
	for (int i = pieces; i > 0; i--)
	{
		//cout << "iBegin: " << iCurrent << " iEnd: " << iCurrent + (size / i) << endl;

		threads[pieces - i] = std::thread(sortMerge<int>, toSort + iCurrent, size / i);

		iCurrent = iCurrent + (size / i);
		size -= (size / i);
	}

	for (int i = 0; i < pieces; i++)
		threads[i].join();

	cout << "Sublists sorted:" << endl;
	for (int i = 0; i < numItems; i++)
		cout << toSort[i] << " ";
	cout << endl;

	// now each sublist is sorted, so sort the whole thing
	sortMerge(toSort, numItems);
}

/********************************************************
 * randomInts
 * This function generates quantity ints and returns them
 * as an array (pointer)
 *******************************************************/
int * randomInts(int quantity)
{

	/* The list to return */
	int * list;

	try
	{
		list = new int[quantity];
	}
	catch (bad_alloc)
	{
		cerr << "ERROR: Could not allocate memory for random numbers" << endl;
	}

	/* Fill it with randomness */
	srand(time(NULL));

	for (int i = 0; i < quantity; i++)
		list[i] = rand() % MAX_NUMBER;

	return list;
}

/********************************************************
 * sortMerge
 * This function implements the merge sort.
 *******************************************************/
template <class T>
void sortMerge(T * array, int numItems) throw (const char *)
{
   // Arrays to copy between
   T * source = array;
   T * dest   = NULL;
   
   // indices used in the sort
   int iFirstBegin  = 0;
   int iFirstMax    = 0;
   int iSecondBegin = 0;
   int iSecondMax   = 0;
   int iDest        = 0;

   // allocate a twin array of the same size to merge into
   try
   {
      dest = new T[numItems];
   }
   catch (std::bad_alloc)
   {
      throw "ERROR: Could not allocate a new buffer for the merge sort";
   }

   /*************************************************
    * INFINITE LOOP! -- needs explaination --
    * Sounds crazy, but the only way out is if there
    * is only one subarry in the array. This condition
    * is checked inside the loop within this infinite
    * one.
    ************************************************/
   while (true)
   {
      // loop until we've gone through the entire array
      while (iFirstBegin < numItems)
      {
         /************** FIND SUBARRAYS **************/
         
         // get the proper iFirstMax
         iFirstMax = iFirstBegin;
         while (!(source[iFirstMax] > source[iFirstMax + 1]) && iFirstMax < numItems)
            ++iFirstMax;
         ++iFirstMax; // max is one past the end

         // same with iSecondMax
         iSecondBegin = iSecondMax = iFirstMax;
         while (!(source[iSecondMax] > source[iSecondMax + 1])
                && iSecondMax < numItems)
            ++iSecondMax;
         ++iSecondMax;

         /************** EXIT CONDITION ************/
         if (iFirstBegin == 0 && iFirstMax == numItems)
         {
            if (dest == array)
            {
               // swap dest and source
               T * temp = source;
               source = dest;
               dest = temp;

               // copy all the data over
               for (int i = 0; i < numItems; ++i)
                  source[i] = dest[i];
            }

            // clean up
            delete [] dest;

            // get out of dodge
            return;
         }


         /*************** MERGE ****************/
         iDest = iFirstBegin;

         while (iFirstBegin < iFirstMax && iSecondBegin < iSecondMax
                && iFirstBegin < numItems && iSecondBegin < numItems)
         {
            assert(iFirstMax < numItems && iSecondBegin < numItems);

            // put in the smaller of the two
            if (source[iFirstBegin] > source[iSecondBegin])
            {
               dest[iDest] = source[iSecondBegin];
               ++iSecondBegin;
            }
            else
            {
               dest[iDest] = source[iFirstBegin];
               ++iFirstBegin;
            }
         
            ++iDest;
         }

         // make sure to get all the items from both subarrays
         while (iFirstBegin < iFirstMax && iFirstBegin < numItems)
         {
            assert(iFirstBegin < numItems);
            assert(iDest < numItems);
         
            dest[iDest] = source[iFirstBegin];
            ++iFirstBegin;
            ++iDest;
         }
         while (iSecondBegin < iSecondMax && iSecondBegin < numItems)
         {
            assert(iSecondBegin < numItems);
            assert(iDest < numItems);

            dest[iDest] = source[iSecondBegin];
            ++iSecondBegin;
            ++iDest;
         }

         // set the new iFirstBegin to be where we left off
         iFirstBegin = iSecondMax;

      }


      // swap source and dest
      T * temp = source;
      source = dest;
      dest = temp;

      // re-initialize indices
      iFirstBegin = 0;
   }
}
