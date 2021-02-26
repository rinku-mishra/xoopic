#ifndef __List_h
#define __List_h

/*
====================================================================

oopicList.H

0.9	(PeterM, 08-93) Original List and Stack classes in G++.
0.961	(PeterM, JohnV, 08-24-93) Replace Borland list containers with
		vanilla C++ template lists in List.h
0.964	(JohnV, 09-28-93) Add ListIter::delete() and supporting data.

0.97	(JamesA 12-30-93) Add restart() to ListIter
0.971	(JamesA 02-05-94) Add putAt(), getAt(), find() to List, 
                itemName to ListItem.
0.975	(JamesA 03-12-94) Add removeItem() to List.
		(HadonN 06-16-94) Add removeAll(), fix(?) removeItem()
		(HadonN 09-19-94) fix(!) removeItem()
0.976	(Hadon 10-20-94) removed putAt(), getAt(), find() from List
0.977	(Hadon 10-28-94) fix operator=(), (call removeAll())
0.978	(JohnV 06-30-95) add deleteAll() to delete all data and 
               remove all items.
0.979 (JohnV 01-11-96) incorporate undocumented additions (from Acquah?)
1.001 (JohnV 02-29-96) renamed N_items() -> nItems().
      (mindgame 11-15-06) synchronize to oopiclist in oopd1 program

====================================================================
*/

#include <string.h>

#if defined(__BORLANDC__)
#include <alloc.h>
#endif

#if defined(_MSC_VER)
//#include <malloc/malloc.h>
#endif

#include <stdlib.h>

template <class Type> class oopicList;
template <class Type> class oopicListIter;
template <class Type> class oopicListItem
{
  // mindgame: encapsulate members by declaring them as private
  //                                  and friend classes;
  friend class oopicList <Type>;
  friend class oopicListIter <Type>;
  // mindgame: added new member, destructive
  //           now the destruction of data can be controlled seperately
  //          for each item.
  bool destructive;
  char* itemName; 		   
  oopicListItem<Type> *next;
public:
  Type *data;
public:
  // mindgame: added new argument, destr_flag
  oopicListItem(Type *newitem, oopicListItem<Type> *nxt, 
		bool destr_flag = true, const char* key = "")  
		{data=newitem; next=nxt; destructive = destr_flag;
		 itemName = strdup(key);}
  ~oopicListItem() {
    if (itemName) 
      free(itemName); 
    itemName = NULL; 
    next = NULL; 
    data = NULL;
  }
};

template <class Type> class oopicList
{
  // mindgame: encapsulate members by declaring them as private
  //                                  and friend class;
  friend class oopicListIter <Type>;
  // mindgame: changed members' property to private
  // mindgame: global destructive flag (applied for all items in the list)
  bool destructive;
  int nitems;
public:
  oopicListItem<Type> *head;
public:
  // mindgame: make global_destructive_flag controllable when it is initiated.
  oopicList(bool destr_flag=false) {head=0; nitems=0; destructive=destr_flag;}
  oopicList(oopicList<Type> &other) 
  {
    head=0; nitems=0; destructive=false;
    *this=other;
  }
  ~oopicList() 
  {
    if(destructive)
      { 
	deleteAll();
     }
    else 
      { 
	removeAll(); 
      }
  }
  
  void removeItem( Type *anItem);
  void removeAll();
  void deleteAll();
  void add(Type *newitem);
  void insert(Type *newitem, int posi);
  // mindgame: new add function capable of specifying whether the item is
  //          destructive or not. 
  void add(Type *newitem, bool destr_flag);
  void addToEnd(Type *newitem);  // expensive walk-the-list-to-end-and-add

  oopicList<Type>& operator=(oopicList<Type> &other)
  {
    //Not a terribly efficient routine.
    //first copy of list is reversed.
    //reversed_list goes out of scope and is deleted.
    oopicListIter<Type> walk(other);
    oopicList<Type> reversed_list;
    
    removeAll();
    while(!walk.Done())
      {
	reversed_list.add(walk.current());
	walk++;
      }
    oopicListIter<Type> walk2(reversed_list);
    while(!walk2.Done())
      {
	add(walk2.current());
	walk2++;
      }
    return *this;
  };

  oopicList<Type>& operator+=(oopicList<Type> &other)
  {
    // assumes that order is not important
    oopicListIter<Type> walk(other);
    while(!walk.Done())
      {
	// mindgame: since items are shallow-copied, data should
	//          not be deleted in this list.
	add(walk.current(), false);
	walk++;
      }
    return *this;
  }

  Type * pop(void) {
    if(head)
      {
	Type* retval=head->data;
	oopicListItem<Type> *to_delete=head;
	head = head->next;
	delete to_delete;
	nitems--;
	return retval;
      }
    else return 0;
  }

  int isEmpty() const {return (head==0);}
  void push(Type *newitem) { add(newitem); };
  inline int nItems() const {return nitems;};
  Type *headData() const {return head->data;}
};

template <class Type> void oopicList<Type>::add(Type *newitem)
{
   oopicListItem<Type> *New = new oopicListItem<Type>(newitem, head);

	nitems++;
	head = New;
}

template <class Type> void oopicList<Type>::add(Type *newitem, bool destr_flag)
{
   oopicListItem<Type> *New = new oopicListItem<Type>(newitem, head, destr_flag);

	nitems++;
	head = New;
}


template <class Type> void oopicList<Type>::insert(Type *newitem, int posi)
{
   oopicListItem<Type> *New = new oopicListItem<Type>(newitem,NULL);
   oopicListItem<Type> *walk;
   nitems++;
   if(head==NULL) {
      head = New;
      return;
   }

   // find the end of the list 
   for( walk=head; walk->next!=NULL; walk=walk->next );
   walk->next = New;

}

// Add an element to the end of the list.  Requires a list traversal.
template <class Type> void oopicList<Type>::addToEnd(Type *newitem)
{
   oopicListItem<Type> *New = new oopicListItem<Type>(newitem,NULL);
   oopicListItem<Type> *walk;
   nitems++;
   if(head==NULL) {
      head = New;
      return;
   }

   // find the end of the list 
   for( walk=head; walk->next!=NULL; walk=walk->next );
   walk->next = New;

}

template <class Type>
void oopicList<Type>::removeItem( Type *anItem )
// use when you need to find the item first, else use version in oopicListIter
{
	oopicListItem<Type>	*prevPtr, *currentPtr;

	if (head == NULL) {
		return;
	} else if (head->data == anItem) {
		currentPtr = head->next;
		delete head;
		head = currentPtr;
		nitems--;
	} else {
		prevPtr = head;
		currentPtr = prevPtr->next;
		while( !(currentPtr == NULL) ) {
			if ( currentPtr->data == anItem ) {
				prevPtr->next = currentPtr->next;
				delete currentPtr;
				currentPtr = prevPtr->next;
				nitems--;
			} else {
				prevPtr = currentPtr;
				currentPtr = currentPtr->next;
			}
		}
	}
}

// removeAll: removes all list elements, but NOT the data!

template <class Type>
void oopicList<Type>::removeAll()
{
	oopicListItem<Type> *next;
	while (head)
	{
		next = head->next;
		delete head;
		head = next;
	}
	nitems = 0;
}

// deleteAll: removes all list elements AND data (mindgame: only if destructive)

template <class Type>
void oopicList<Type>::deleteAll()
{
	oopicListItem<Type> *next;
	while ( head )
	{
		next = head->next;
		if (head->destructive) delete head->data; 
		delete head;
		head = next;
	}
	nitems = 0;
}

/*
	Acquah's version:

  template <class Type>
  void oopicList<Type>::deleteAllData()
  {
	ListItem<Type> *next, *current;
	current = head; 					// necc because deleteData() sometimes
	head = NULL;          				// ... (eventually) calls removeItem()
	while ( current )
	{
		next = (*current).next;
		current->deleteData();
		delete current;
		current = next;
	}
	nitems = 0;
  }

*/

////////////////////////////////////////////////////////////////////////////////

template <class Type> class oopicListIter
{
	oopicListItem<Type>*	currentPtr;
	oopicListItem<Type>*	previousPtr;
	oopicList<Type>*	theList;
public:
	oopicListIter(){}						// empty constructor, must be used with restart
	oopicListIter(oopicList<Type> & aList) {theList=&aList; restart();};
	void	restart() {previousPtr = currentPtr = theList->head;};
	void	restart(oopicList<Type>& aList) {theList=&aList; restart();};
	void	restart(Type& aGroup);
	Type*	data() {if (currentPtr) return currentPtr->data; else return NULL;};
	Type* current() {return data();};
	Type* operator () () {return data();};
	void	operator++(int) {if (currentPtr) {previousPtr = currentPtr;
		currentPtr = currentPtr->next;} else restart();};
	int	Done() {return currentPtr==NULL;};
	int	isEmpty() {return !(theList->head==0);};
	void	deleteCurrent();
};


//	This function removes an item from the list.  Might be cleaner
//	if it is inside oopicList<Type>, but do not want order n search
//	for element.
template <class Type>
void oopicListIter<Type>::deleteCurrent()
{
	if (currentPtr == NULL) return;		//	bail out if empty or at end
	//	if at head of list, point head to next item
	if (currentPtr == theList->head)
	{
		theList->head = currentPtr->next;
		previousPtr = NULL;
	}
	else previousPtr->next = currentPtr->next;
	delete	current();						//	delete the data
	delete	currentPtr;						//	delete the list item
	currentPtr = previousPtr;
	theList->nitems--;
}

//  Some useful macros
// mindgame: remove redundant titer.restart() within for loop 
#define ApplyToList(function,theList,listType) { oopicListIter<listType> titer(theList); for(;!titer.Done();titer++) titer.current()->function;}
#endif
