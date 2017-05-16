#ifndef __HASHMAP_H__
#define __HASHMAP_H__

#include "Common.h"
#include <map>
#include <stdlib.h>

namespace hiveFluidSimulation 
{
	template<class CKeyType>
	inline unsigned int hashFunction(const CKeyType &vKey)
	{
		return 0u;
	}

	template <class CKeyType, class CValueType>
	class CHashmap
	{
	public:
		typedef typename std::map<unsigned int, CValueType> KeyValueMap;

	private:
		KeyValueMap **m_HashMap;
		unsigned int m_BucketCount;
		unsigned int m_ModuloValue;

	protected:
		FORCE_INLINE void init()
		{
			m_HashMap = new KeyValueMap*[m_BucketCount];
			for (unsigned int i=0; i < m_BucketCount; i++)
			{
				m_HashMap[i] = NULL;
			}
		}

		FORCE_INLINE void cleanup()
		{
			if (m_HashMap)
			{
				for (unsigned int i=0; i < m_BucketCount; i++)
				{
					if (m_HashMap[i] != NULL)
					{
						m_HashMap[i]->clear();
						delete m_HashMap[i];
					}
				}
				delete [] m_HashMap;
				m_HashMap = NULL;
			}
		}

	public:
		FORCE_INLINE CHashmap(const unsigned int vBucketCount)
		{
			// Use a bucket count of 2^n => faster modulo
			unsigned int PowerOfTwo = 1u;
			while(PowerOfTwo < vBucketCount) 
				PowerOfTwo <<= 1;
			m_BucketCount = PowerOfTwo;
			m_ModuloValue = m_BucketCount-1u;
			init();
		}

		~CHashmap()
		{
			cleanup();
		}

		FORCE_INLINE void clear()
		{
			cleanup();
			init();
		}

		FORCE_INLINE KeyValueMap* getKeyValueMap(const unsigned int vIndex)
		{
			return m_HashMap[vIndex];
		}

		FORCE_INLINE void reset()
		{
			for (unsigned int i=0; i < m_BucketCount; i++)
			{
				if (m_HashMap[i] != NULL)
				{
					m_HashMap[i]->clear();
				}
			}
		}


		/** Return the bucket count.
		 */
		FORCE_INLINE unsigned int bucket_count() const
		{	
			return m_BucketCount;
		}

		/** Find element. 
		 */
		FORCE_INLINE CValueType* find(const CKeyType &vKey)
		{
			const unsigned int HashValue = hashFunction<CKeyType>(vKey);
			const unsigned int MapIndex = HashValue & m_ModuloValue;
			if (m_HashMap[MapIndex] != NULL)
			{
				typename KeyValueMap::iterator &Iter = (*m_HashMap[MapIndex]).find(HashValue);
				if (Iter != (*m_HashMap[MapIndex]).end())
					return &Iter->second;
			}
			return NULL;
		}

		/** Insert element. 
		 */
		FORCE_INLINE void insert(const CKeyType &vKey, const CValueType& vValue)
		{
			const unsigned int HashValue = hashFunction<CKeyType>(vKey);
			const unsigned int MapIndex = HashValue & m_ModuloValue;
			if (m_HashMap[MapIndex] == NULL)
			{
				m_HashMap[MapIndex] = new KeyValueMap();
			}
			(*m_HashMap[MapIndex])[HashValue] = vValue;
		}

		/** Remove the given element and return true, if the element was found. 
		 */
		FORCE_INLINE void remove(const CKeyType &vKey)
		{
			const unsigned int HashValue = hashFunction<CKeyType>(vKey);
			const unsigned int MapIndex = HashValue & m_ModuloValue;
			if (m_HashMap[MapIndex] != NULL)
			{
				m_HashMap[MapIndex]->erase(HashValue);
				if (m_HashMap[MapIndex]->size() == 0)
				{
					delete m_HashMap[MapIndex];
					m_HashMap[MapIndex] = NULL;
				}
			}
		}

		FORCE_INLINE CValueType& operator[](const CKeyType &vKey)
		{
			const int HashValue = hashFunction<CKeyType>(vKey);
			const unsigned int MapIndex = HashValue & m_ModuloValue;
			if (m_HashMap[MapIndex] == NULL)
			{
				m_HashMap[MapIndex] = new KeyValueMap();
			}
			return (*m_HashMap[MapIndex])[HashValue];
		}

		FORCE_INLINE const CValueType& operator[](const CKeyType &vKey) const
		{
			const unsigned int HashValue = hashFunction<CKeyType>(vKey, m_BucketCount);
			const unsigned int MapIndex = HashValue & m_ModuloValue;
			if (m_HashMap[MapIndex] == NULL)
			{
				m_HashMap[MapIndex] = new KeyValueMap();
			}
			return (*m_HashMap[MapIndex])[HashValue];
		}

		FORCE_INLINE const CValueType* query(const CKeyType &vKey) const
		{
			const unsigned int HashValue = hashFunction<CKeyType>(vKey);
			const unsigned int MapIndex = HashValue & m_ModuloValue;
			if (m_HashMap[MapIndex] == NULL)
			{
				return NULL;
			}
 			typename KeyValueMap::iterator &Iter = m_HashMap[MapIndex]->find(HashValue);
			if (Iter != m_HashMap[MapIndex]->end())
				return &Iter->second;
			return NULL;
		}

		FORCE_INLINE CValueType* query(const CKeyType &vKey) 
		{
			const unsigned int HashValue = hashFunction<CKeyType>(vKey);
			const unsigned int MapIndex = HashValue & m_ModuloValue;
			if (m_HashMap[MapIndex] == NULL)
			{
				return NULL;
			}
			const typename KeyValueMap::iterator &Iter = m_HashMap[MapIndex]->find(HashValue);
			if (Iter != m_HashMap[MapIndex]->end())
				return &Iter->second;
			return NULL;
		}
		
	};
}

#endif
