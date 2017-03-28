/////////////////////////////////////////////////////////////////////////////
// CNewMap<KEY, ARG_KEY, VALUE, ARG_VALUE>

#pragma warning(disable:4995) 

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
class CNewMap : public CObject
{
protected:
	// Association
typedef	struct CAssoc
	{
		CAssoc* pNext;
		UINT nHashValue;  // needed for efficient iteration
		KEY key;
		VALUE value;
	} ASSOC,*PASSOC;;
	public:
		// Construction
		CNewMap(int nBlockSize = 10);
		
		// Attributes
		// number of elements
		int GetCount() const;
		BOOL IsEmpty() const;
		
		// Lookup
		BOOL Lookup(ARG_KEY key, VALUE& rValue) const;
		
		// Operations
		// Lookup and add if not there
		VALUE& operator[](ARG_KEY key);
		
		// add a new (key, value) pair
		void SetAt(ARG_KEY key, ARG_VALUE newValue);
		
		// removing existing (key, ?) pair
		BOOL RemoveKey(ARG_KEY key);
		void RemoveAll();
		
		// iterating all (key, value) pairs
		POSITION GetStartPosition() const;
		void GetNextAssoc(POSITION& rNextPosition, KEY& rKey, VALUE& rValue) const;
		
		// advanced features for derived classes
		UINT GetHashTableSize() const;
		void InitHashTable(UINT hashSize, BOOL bAllocNow = TRUE);
		
		// Implementation
	protected:
		CAssoc** m_pHashTable;
		UINT m_nHashTableSize;
		int m_nCount;
		CAssoc* m_pFreeList;
		struct CPlex* m_pBlocks;
		int m_nBlockSize;
		
		CAssoc* NewAssoc();
		void FreeAssoc(CAssoc*);
		CAssoc* GetAssocAt(ARG_KEY, UINT&) const;
		
		public:
			~CNewMap();
			void Serialize(CArchive&);
			
};

/////////////////////////////////////////////////////////////////////////////
// CNewMap<KEY, ARG_KEY, VALUE, ARG_VALUE> inline functions

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
AFX_INLINE int CNewMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::GetCount() const
{ return m_nCount; }
template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
AFX_INLINE BOOL CNewMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::IsEmpty() const
{ return m_nCount == 0; }
template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
AFX_INLINE void CNewMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::SetAt(ARG_KEY key, ARG_VALUE newValue)
{ (*this)[key] = newValue; }
template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
AFX_INLINE POSITION CNewMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::GetStartPosition() const
{ return (m_nCount == 0) ? NULL : BEFORE_START_POSITION; }
template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
AFX_INLINE UINT CNewMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::GetHashTableSize() const
{ return m_nHashTableSize; }

/////////////////////////////////////////////////////////////////////////////
// CNewMap<KEY, ARG_KEY, VALUE, ARG_VALUE> out-of-line functions

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
CNewMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::CNewMap(int nBlockSize)
{
	ASSERT(nBlockSize > 0);
	
	m_pHashTable = NULL;
	m_nHashTableSize = 17;  // default size
	m_nCount = 0;
	m_pFreeList = NULL;
	m_pBlocks = NULL;
	m_nBlockSize = nBlockSize;
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
void CNewMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::InitHashTable(
															UINT nHashSize, BOOL bAllocNow)
															//
															// Used to force allocation of a hash table or to override the default
															//   hash table size of (which is fairly small)
{
	ASSERT_VALID(this);
	ASSERT(m_nCount == 0);
	ASSERT(nHashSize > 0);
	
	if (m_pHashTable != NULL)
	{
		// free hash table
		delete[] m_pHashTable;
		m_pHashTable = NULL;
	}
	
	if (bAllocNow)
	{
		m_pHashTable = new CAssoc* [nHashSize];
		memset(m_pHashTable, 0, sizeof(CAssoc*) * nHashSize);
	}
	m_nHashTableSize = nHashSize;
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
void CNewMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::RemoveAll()
{
	ASSERT_VALID(this);
	
	if (m_pHashTable != NULL)
	{
		// destroy elements (values and keys)
		for (UINT nHash = 0; nHash < m_nHashTableSize; nHash++)
		{
			CAssoc* pAssoc;
			for (pAssoc = m_pHashTable[nHash]; pAssoc != NULL;
			pAssoc = pAssoc->pNext)
			{
				DestructElements<VALUE>(&pAssoc->value, 1);
				DestructElements<KEY>(&pAssoc->key, 1);
			}
		}
	}
	
	// free hash table
	delete[] m_pHashTable;
	m_pHashTable = NULL;
	
	m_nCount = 0;
	m_pFreeList = NULL;
	m_pBlocks->FreeDataChain();
	m_pBlocks = NULL;
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
CNewMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::~CNewMap()
{
	RemoveAll();
	ASSERT(m_nCount == 0);
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
typename CNewMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::CAssoc*
CNewMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::NewAssoc()
{
	if (m_pFreeList == NULL)
	{
		// add another block
		CPlex* newBlock = CPlex::Create(m_pBlocks, m_nBlockSize, sizeof(CNewMap::CAssoc));
		// chain them into free list
		CNewMap::CAssoc* pAssoc = (CNewMap::CAssoc*) newBlock->data();
		// free in reverse order to make it easier to debug
		pAssoc += m_nBlockSize - 1;
		for (int i = m_nBlockSize-1; i >= 0; i--, pAssoc--)
		{
			pAssoc->pNext = m_pFreeList;
			m_pFreeList = pAssoc;
		}
	}
	ASSERT(m_pFreeList != NULL);  // we must have something
	
	CNewMap::CAssoc* pAssoc = m_pFreeList;
	m_pFreeList = m_pFreeList->pNext;
	m_nCount++;
	ASSERT(m_nCount > 0);  // make sure we don't overflow
	ConstructElements<KEY>(&pAssoc->key, 1);
	ConstructElements<VALUE>(&pAssoc->value, 1);   // special construct values
	return pAssoc;
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
void CNewMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::FreeAssoc(typename CNewMap::CAssoc* pAssoc)
{
	DestructElements<VALUE>(&pAssoc->value, 1);
	DestructElements<KEY>(&pAssoc->key, 1);
	pAssoc->pNext = m_pFreeList;
	m_pFreeList = pAssoc;
	m_nCount--;
	ASSERT(m_nCount >= 0);  // make sure we don't underflow
	
	// if no more elements, cleanup completely
	//if (m_nCount == 0)
	//	RemoveAll();
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
typename CNewMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::CAssoc*
CNewMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::GetAssocAt(ARG_KEY key, UINT& nHash) const
// find association (or return NULL)
{
	nHash = HashKey<ARG_KEY>(key) % m_nHashTableSize;
	
	if (m_pHashTable == NULL)
		return NULL;
	
	// see if it exists
	CAssoc* pAssoc;
	for (pAssoc = m_pHashTable[nHash]; pAssoc != NULL; pAssoc = pAssoc->pNext)
	{
		if (CompareElements(&pAssoc->key, &key))
			return pAssoc;
	}
	return NULL;
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
BOOL CNewMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::Lookup(ARG_KEY key, VALUE& rValue) const
{
	ASSERT_VALID(this);
	
	UINT nHash;
	CAssoc* pAssoc = GetAssocAt(key, nHash);
	if (pAssoc == NULL)
		return FALSE;  // not in map
	
	rValue = pAssoc->value;
	return TRUE;
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
VALUE& CNewMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::operator[](ARG_KEY key)
{
	ASSERT_VALID(this);
	
	UINT nHash;
	CAssoc* pAssoc;
	if ((pAssoc = GetAssocAt(key, nHash)) == NULL)
	{
		if (m_pHashTable == NULL)
			InitHashTable(m_nHashTableSize);
		
		// it doesn't exist, add a new Association
		pAssoc = NewAssoc();
		pAssoc->nHashValue = nHash;
		pAssoc->key = key;
		// 'pAssoc->value' is a constructed object, nothing more
		
		// put into hash table
		pAssoc->pNext = m_pHashTable[nHash];
		m_pHashTable[nHash] = pAssoc;
	}
	return pAssoc->value;  // return new reference
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
BOOL CNewMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::RemoveKey(ARG_KEY key)
// remove key - return TRUE if removed
{
	ASSERT_VALID(this);
	
	if (m_pHashTable == NULL)
		return FALSE;  // nothing in the table
	
	CAssoc** ppAssocPrev;
	ppAssocPrev = &m_pHashTable[HashKey<ARG_KEY>(key) % m_nHashTableSize];
	
	CAssoc* pAssoc;
	for (pAssoc = *ppAssocPrev; pAssoc != NULL; pAssoc = pAssoc->pNext)
	{
		if (CompareElements(&pAssoc->key, &key))
		{
			// remove it
			*ppAssocPrev = pAssoc->pNext;  // remove from list
			FreeAssoc(pAssoc);
			return TRUE;
		}
		ppAssocPrev = &pAssoc->pNext;
	}
	return FALSE;  // not found
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
void CNewMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::GetNextAssoc(POSITION& rNextPosition,
														   KEY& rKey, VALUE& rValue) const
{
	ASSERT_VALID(this);
	ASSERT(m_pHashTable != NULL);  // never call on empty map
	
	CAssoc* pAssocRet = (CAssoc*)rNextPosition;
	ASSERT(pAssocRet != NULL);
	
	if (pAssocRet == (CAssoc*) BEFORE_START_POSITION)
	{
		// find the first association
		for (UINT nBucket = 0; nBucket < m_nHashTableSize; nBucket++)
			if ((pAssocRet = m_pHashTable[nBucket]) != NULL)
				break;
			ASSERT(pAssocRet != NULL);  // must find something
	}
	
	// find next association
	ASSERT(AfxIsValidAddress(pAssocRet, sizeof(CAssoc)));
	CAssoc* pAssocNext;
	if ((pAssocNext = pAssocRet->pNext) == NULL)
	{
		// go to next bucket
		for (UINT nBucket = pAssocRet->nHashValue + 1;
		nBucket < m_nHashTableSize; nBucket++)
			if ((pAssocNext = m_pHashTable[nBucket]) != NULL)
				break;
	}
	
	rNextPosition = (POSITION) pAssocNext;
	
	// fill in return data
	rKey = pAssocRet->key;
	rValue = pAssocRet->value;
}

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
void CNewMap<KEY, ARG_KEY, VALUE, ARG_VALUE>::Serialize(CArchive& ar)
{
	ASSERT_VALID(this);
	
	CObject::Serialize(ar);
	
	if (ar.IsStoring())
	{
		ar.WriteCount(m_nCount);
		if (m_nCount == 0)
			return;  // nothing more to do
		
		ASSERT(m_pHashTable != NULL);
		for (UINT nHash = 0; nHash < m_nHashTableSize; nHash++)
		{
			CAssoc* pAssoc;
			for (pAssoc = m_pHashTable[nHash]; pAssoc != NULL;
			pAssoc = pAssoc->pNext)
			{
				SerializeElements<KEY>(ar, &pAssoc->key, 1);
				SerializeElements<VALUE>(ar, &pAssoc->value, 1);
			}
		}
	}
	else
	{
		DWORD nNewCount = ar.ReadCount();
		while (nNewCount--)
		{
			KEY newKey;
			VALUE newValue;
			SerializeElements<KEY>(ar, &newKey, 1);
			SerializeElements<VALUE>(ar, &newValue, 1);
			SetAt(newKey, newValue);
		}
	}
}

