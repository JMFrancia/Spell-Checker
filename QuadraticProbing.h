#ifndef QUADRATIC_PROBING_H
#define QUADRATIC_PROBING_H

#include <vector>
#include <string>
#include <iostream>
using namespace std;

int nextPrime( int n );

// QuadraticProbing Hash table class
//
// CONSTRUCTION: an approximate initial size or default of 101
//
// ******************PUBLIC OPERATIONS*********************
// bool insert( x )       --> Insert x
// bool remove( x )       --> Remove x
// bool contains( x )     --> Return true if x is present
// void makeEmpty( )      --> Remove all items
// int hash( string str ) --> Global method to hash strings
// int * searchGetProbes(x) --> searches for x, returns position (result[0]) and # probes required (result[1])

//    int hash( const string & key );
//    int hash( int key );

template <typename HashedObj>
class HashTable
{
  public:

        /**
     * A hash routine for string objects.
     */
    int hash( const string & key ) const
    {
        int hashVal = 0;
    
        for( int i = 0; i < key.length( ); i++ )
            hashVal = 37 * hashVal + key[ i ];
    
        return hashVal;
    }
    
    /**
     * A hash routine for ints.
     */
    int hash( int key ) const
    {
        return key;
    }
    
    explicit HashTable( int size = 101, string pType = "QUADRATIC", bool rehashing = true ) : array( nextPrime( size ) ) , rehashingON(rehashing) //set default size 101 - otherwise first prime after size
      { 
            if(pType != "QUADRATIC" && pType != "LINEAR"){
                cout << "\nWARNING: Hash Table generated with unrecognized probe type \"" << pType << "\".";
                cout <<" Defaulting to QUADRATIC.\n";
                probe = "QUADRATIC";
            } else
                probe = pType;
            makeEmpty( ); 
      }

    bool contains( const HashedObj & x ) const  //returns if x is present
    {
    	bool result;
        if(probe == "LINEAR")
    		result = isActive(findLinearPos(x)[0]);
    	else
    		result = isActive(findPos(x));        
        return result;
    }

    void makeEmpty( )           //empties hash table (lazy deletion)
    {
        currentSize = 0;
        for( int i = 0; i < array.size( ); i++ )
            array[ i ].info = EMPTY;
    }

    int getSearchProbes(const HashedObj & x ) const {
	int result;
	if(probe == "LINEAR")
		result = findLinearPos(x)[1];
	else
		result = 0;
	return result;
    }

    int insert( const HashedObj & x ) //inserts x into table
    {
            // Insert x as active
        int currentPos;
        int * result;
        
        if(probe == "LINEAR"){
            result = findLinearPos( x ); 
            currentPos = result[0];
        } else
            currentPos = findPos ( x );
            
        if( isActive( currentPos ) )       //if already exists, return false
            return -1;

        array[ currentPos ] = HashEntry( x, ACTIVE );   //otherwise, hash entry 

            // Rehash; see Section 5.5
        if(rehashingON && ++currentSize > array.size( ) / 2 )   //if table is more than half full, rehash
            rehash( );
    
        if(probe == "LINEAR")
            return result[1];
        else
            return currentPos;
    }

    bool remove( const HashedObj & x )      //remove obj x
    {
        int currentPos = findPos( x );      
        if( !isActive( currentPos ) )       //if object doesn't exist, return false
            return false;

        array[ currentPos ].info = DELETED;       //otherwise, mark spot as deleted
        return true;
    }

    enum EntryType { ACTIVE, EMPTY, DELETED };

  private:
    struct HashEntry        //hash element struct, holding element and entry type (deleted/empty/active)
    {
        HashedObj element;
        EntryType info;

        HashEntry( const HashedObj & e = HashedObj( ), EntryType i = EMPTY )  //constructor for hash entry HashEntry(e,i)
          : element( e ), info( i ) { }
    };
    
    string probe;
    vector<HashEntry> array;
    int currentSize;
    bool rehashingON;

    bool isActive( int currentPos ) const               //returns if position is active (full)
      { return array[ currentPos ].info == ACTIVE; }

    int * findLinearPos(const HashedObj & x ) const {
        //returns pointer to result. Result[0] holds final index. Result[1] holds number of probes required.
        int * result = new int[2];
        int currentPos = myhash( x );                        
        result[1] = 0;
 

        while( array[ currentPos ].info != EMPTY &&         
                array[ currentPos ].element != x )
        {
            currentPos++;  // Compute ith probe
            result[1]++;
            if( currentPos >= array.size( ) )
                currentPos -= array.size( );
        } 
	
	result[0] = currentPos; 
        return result;    
    }
     
    int findPos( const HashedObj & x ) const             //find pos by object based on it's hash
    {
        int offset = 1;
        int currentPos = myhash( x );                        //find pos

          // Assuming table is half-empty, and table length is prime,
          // this loop terminates
        while( array[ currentPos ].info != EMPTY &&         //quadratic probe if spot is full
                array[ currentPos ].element != x )
        {
            currentPos += offset;  // Compute ith probe
            offset += 2;
            if( currentPos >= array.size( ) )
                currentPos -= array.size( );       
        }
        

        return currentPos;
    }

    void rehash( )              
    {
        vector<HashEntry> oldArray = array;

            // Create new double-sized, empty table
        array.resize( nextPrime( 2 * oldArray.size( ) ) );
        for( int j = 0; j < array.size( ); j++ )
            array[ j ].info = EMPTY;

            // Copy table over
        currentSize = 0;
        for( int i = 0; i < oldArray.size( ); i++ )
            if( oldArray[ i ].info == ACTIVE )
                insert( oldArray[ i ].element );
    }
    
    int myhash( const HashedObj & x ) const   //finds initial hash val of object x (hash % table size)
    {
        int hashVal = hash( x );

        hashVal %= array.size( );
        if( hashVal < 0 )
            hashVal += array.size( );

        return hashVal;
    }

    
		/**
	 * Internal method to test if a positive number is prime.
	 * Not an efficient algorithm.
	 */
	bool isPrime( int n )
	{
	    if( n == 2 || n == 3 )
		return true;

	    if( n == 1 || n % 2 == 0 )
		return false;

	    for( int i = 3; i * i <= n; i += 2 )
		if( n % i == 0 )
		    return false;

	    return true;
	}

	/**
	 * Internal method to return a prime number at least as large as n.
	 * Assumes n > 0.
	 */
	int nextPrime( int n )
	{
	    if( n <= 0 )
		n == 3;

	    if( n % 2 == 0 )
		n++;

	    for( ; !isPrime( n ); n += 2 )
		;

	    return n;
	}

};


#endif
