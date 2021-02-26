////////////////////////////////////////////////////////
//
//  TxqList.h:  TxqList Class
//
//  Copyright 1998  Tech-X Corporation
//
//  CVS: $Id: TxqList.h 900 1999-01-26 21:59:26Z kgl $
//
///////////////////////////////////////////////////////


#ifndef TXQLIST_H
#define TXQLIST_H

// unix includes
#include <iostream.h>

#define INCSIZE 10

/**
 *    --  Template class for a list of objects
 *
 *  Copyright 1994, 1995, 1996, 1997, 1998 by Tech-X Corporation
 *
 *  @author  John R. Cary and Sveta G. Shasharina, modified by Kelly G. Luetkemeyer
 *
 *  @version $Id: TxqList.h 900 1999-01-26 21:59:26Z kgl $
 */

template <class T> class TxqList {

  protected:

      ///
    T* members;
      ///
    int size;
      ///
    int	maxsize;
      ///
    int	incsize;

  public:

  // Constructors and destructor
      ///
    TxqList(int n = 0) : members(new T[n]), size(n), maxsize(n),
                      incsize(INCSIZE) {}
      ///
    TxqList(const TxqList<T>& list) : members(new T[list.Size()]), size(list.Size()),
                                maxsize(list.Size()), incsize(INCSIZE) {
      for(int i=0; i<size; i++) members[i] = list.members[i];
    }
      ///
    virtual ~TxqList() {
      delete [] members;
    }

      /// Assignment
    TxqList<T>& operator=(const TxqList<T>& list){
      if( this == &list ) return *this;
      size = 0; members = new T[0]; maxsize = 0; incsize = INCSIZE;
      Size(list.Size());
      for(int i=0; i<size; i++) members[i] = list.members[i];
      return *this;
    }

  private:

      /// Change size of list
    int Enlarge(int inc=-1){
      maxsize = ( inc > 0 ? maxsize + inc : maxsize + incsize );
      T* newmembers = new T[maxsize];
      if(!newmembers){
        cerr << "TxqList::Enlarge(int): out of memory!\n";
        return -1;
      }
      for(int i=0; i<Size(); i++) newmembers[i] = members[i];
      delete [] members;
      members = newmembers;
      return 0;
    }

      /// Change size of list
    int Shrink(){
      if( maxsize - Size() < incsize ) return 0;
      maxsize -= incsize;
      T* newmembers = new T[maxsize];
      if(!newmembers){
        cerr << "TxqList::Shrink(): out of memory!  Problems loom.\n";
        cerr << "    Could not allocate " << maxsize << " elements.\n";
        maxsize += incsize;
        cerr << "    Reverting to " << maxsize << " elements.\n";
        return -1;
      }
      for(int i=0; i<Size(); i++) newmembers[i] = members[i];
      delete [] members;
      members = newmembers;
      return 0;
    }

  public:

      /// Access
    virtual int Size() const {return size;}
      /// Access
    T& operator[](int indx) const {return members[indx];}

      ///
    virtual int RemoveAll(){ return Size(0);}
      /** Increase or decrease the list size, then return the new size;
       *  if the size is increased, then fill with default members.
       */
    virtual int Size(int new_sz){
      if( new_sz <= size ) {	        // Decrease size, eliminating end members.
	size = new_sz;
	Shrink();
	return size;
      }                                 // Increase size, eliminating end members.
      int inc = new_sz - maxsize;
      int enlrg = ( inc>0 ? Enlarge(inc) : 0 );	   // Increase capacity if needed.
      if(enlrg) return size;
      size = new_sz;
      return size;
    }

  // Adding, inserting, replacing, and removing members.
      /// Add new member to end
    int Append(const T& newmember) {
      int enlrg=0;
      if( size == maxsize ) enlrg = Enlarge();
      if(enlrg) return -1;
      members[size] = newmember;
      size++;
      return size-1;
    }
      /// Replace member
    virtual int Replace(const T& newmember, int n){
      if( n<0 || n>Size() ) return -1;
      members[n] = newmember;
      return n;
    }
      /// Insert new member
    int Insert(const T& newmember, int n){
        // Validate:  allowed to insert at end.
      if( n<0 || n>Size() ) return -1;
        // Increase size if needed
      int enlrg = ( size == maxsize ? Enlarge() : 0 );
      if(enlrg) return -1;
        // Copy up
      for(int k=Size(); k>n; k--) members[k] = members[k-1];
        // Insert new list
      members[n] = newmember;
      size++;
      return n;
    }
      /// Insert an entire list
    int Insert(const TxqList<T>& list, int n){
        // Validate
      if( n<0 || n>Size() ) return -1;
        // Increase size if needed
      int newsize = size + list.Size();
      int inc = newsize  - maxsize;
      int enlrg=0;
      if( inc > 0 ) enlrg = Enlarge(inc);
      if( enlrg ) return enlrg;
        // Copy up
      int il = n + list.Size();
      int k;
      for(k=newsize-1; k>=il; k--) members[k] = members[k-1];
        // Insert new list
      for (k=n; k<il; k++) members[k] = list.members[k-n];
      size = newsize;
      return n;
    }
      ///
    int Remove(int n) {
      if( (n<0) || (n>=size) ) return -1;                      // Verify
      size--;	                                               // Reduce size
      for( int i=n; i<Size(); i++) members[i] = members[i+1];  // Copy down
      Shrink();                                                // Reduce memory
      return 1;                                                // Successful
    }
      /// Remove members from il to iu, then return number removed.
    int Remove(int il, int iu) { 
      il = ( il > 0 ? il : 0 );
      iu = ( iu < Size() ? iu : Size() - 1 );
      int rmv = iu - il + 1;
      if( rmv < 0 ) return 0;
      size -= rmv;
      for( int i=il; i<Size(); i++) members[i] = members[i+rmv];  // Copy down
      Shrink();	                                                  // Reduce memory
      return rmv;                                                 // Successful
    }
};

#endif // TXQLIST_H
