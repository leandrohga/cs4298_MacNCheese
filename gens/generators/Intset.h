   /**********************************************************************
    *                             Intset.h                               *
    *                                                                    *
    *          Definition File for the Intset<N> Class Template.         *
    *                                                                    *
    *  Objects of the class Intset<N>, for some positive integer N, are  *
    *  sets of integers between 0 and N.                                 *
    *                                                                    *
    *  James L. Richards                                    Version 2.1  *
    *                                       Last Update: September 2007  *
    *  Marty J. Wolf                                                     *
    *                                       Last Update: February 2016   *
    *                                                                    *
    *                                                                    *
    **********************************************************************/

#include <vector>
using namespace std;

template <const int MAX_ELEMENT>
class Intset
{
public:

	Intset();
	// Construct an empty set. 

	bool IsEmpty() const;
	// Return true if this set is empty; otherwise, false is returned.

	bool Contains(int elem) const;
	// Return true if elem is in this; otherwise, false is returned.

	int Size() const;
	// Return the number of elements in this set.

	void Add(int elem);
	// Put elem into this set.

	void Remove(int elem);
	// If elem is in this set, it is removed from this set; otherwise, 
   // set is unchanged.

	const Intset& operator=(const Intset& set);
	// Assign a new set value to this set.

	//friend Intset operator+(const Intset& set1, const Intset& set2);
	Intset operator+(const Intset& set1 ) const;
	// Return the set union of set1 and set2; i.e., the returned set is
   // the set contains elements that are in one or both of set1 and
   // set2.

	Intset operator*(const Intset& set2) const;
	// Return the set intersection of set1 and set2; i..e., the returned
   // set contains elements that are in both set1 and set2.

private:

   vector<bool> el; // k is in this set if and only if el[k] == true

   int size;        // number of elements in this set
};

/***********************
 **  Implementations  **
 ***********************/

template <const int MAX_ELEMENT>
Intset<MAX_ELEMENT>::Intset()
{
	for (int i = 0; i <= MAX_ELEMENT; i++)
		el.push_back(false);
	size = 0;
}

template <const int MAX_ELEMENT>
bool Intset<MAX_ELEMENT>::IsEmpty() const
{
	return (size == 0);
}

template <const int MAX_ELEMENT>
bool Intset<MAX_ELEMENT>::Contains(int elem) const
{
	return el[elem];
}

template <const int MAX_ELEMENT>
int Intset<MAX_ELEMENT>::Size() const
{
	return size;
}

template <const int MAX_ELEMENT>
void Intset<MAX_ELEMENT>::Add(int elem)
{
	if (!el[elem])
	{
		el[elem] = true;
		size++;
	}
}

template <const int MAX_ELEMENT>
void Intset<MAX_ELEMENT>::Remove(int elem)
{
	if (el[elem])
	{
		el[elem] = false;
		size--;
	}
}

template <const int MAX_ELEMENT>
const Intset<MAX_ELEMENT>& 
   Intset<MAX_ELEMENT>::operator=(const Intset<MAX_ELEMENT>& set)
{
	if (this != &set)
		for (int k = 0; k <= MAX_ELEMENT; k++)
			el[k] = set.el[k];
	return *this;
}

template <const int MAX_ELEMENT>
Intset<MAX_ELEMENT> Intset<MAX_ELEMENT>::operator+(
                              const Intset<MAX_ELEMENT>& set2) const
{
	Intset<MAX_ELEMENT> set;
	for (int k = 0; k <= MAX_ELEMENT; k++)
		set.el[k] = el[k] || set2.el[k];
	return set;
}

template <const int MAX_ELEMENT>
Intset<MAX_ELEMENT> Intset<MAX_ELEMENT>::operator*(
                              const Intset<MAX_ELEMENT>& set2) const
{
	Intset<MAX_ELEMENT> set;
	for (int k = 0; k <= MAX_ELEMENT; k++)
		set.el[k] = el[k] && set2.el[k];
	return set;
}
