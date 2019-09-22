#ifndef _ElementDef_20080902pm0418_
#define _ElementDef_20080902pm0418_
using namespace FDO;
template <class AOBJ, uint BLOCK_OBJ_COUNT = 100>
class TMemPool
{
	struct MEMORY_NODE
	{
		AOBJ			mAOBJ;
		MEMORY_NODE*	mpNext;
	};

	struct MEMORY_NODE_BLOCK
	{
		MEMORY_NODE_BLOCK*	mpNext;
		MEMORY_NODE			mvElement[BLOCK_OBJ_COUNT];
	};

protected:
	MEMORY_NODE_BLOCK*	mpHeadBlock;
	MEMORY_NODE*		mpHeadNode;
	uint				mnTotalMemorySize;
	uint				mnMaxCount;
	uint				mnValidCount;

public:
	TMemPool() : 
	  mpHeadBlock(0),
		  mpHeadNode(0),
		  mnTotalMemorySize(0),
		  mnMaxCount(0),
		  mnValidCount(0)
	  {
	  }

	  ~TMemPool()
	  {
		  assert(mnMaxCount == mnValidCount);

		  while(mpHeadBlock)
		  {
			  MEMORY_NODE_BLOCK *pCurBlock = mpHeadBlock;
			  mpHeadBlock = mpHeadBlock->mpNext;
			  ::delete[] (unsigned char*)pCurBlock;
		  }
	  }

	  void Release()
	  {
		  //assert(m_nMaxListCount == m_nValidListCount);

		  while(mpHeadBlock)
		  {
			  MEMORY_NODE_BLOCK *pCurBlock = mpHeadBlock;
			  mpHeadBlock = mpHeadBlock->mpNext;
			  ::delete[] (unsigned char*)pCurBlock;
		  }

		  mpHeadNode = 0;
		  mnMaxCount = 0;
		  mnValidCount = 0;
		  mnTotalMemorySize = 0;
	  }

	  void NewBlock()
	  {
		  //new個新的block加入memory list中
		  MEMORY_NODE_BLOCK *pNewBlock = (MEMORY_NODE_BLOCK*)::new unsigned char[sizeof(MEMORY_NODE_BLOCK)];
		  if(!pNewBlock)
			  return;

		  pNewBlock->mpNext = mpHeadBlock;
		  mpHeadBlock = pNewBlock;

		  //串連其中的object
		  MEMORY_NODE* pvCurList = pNewBlock->mvElement;
		  for(uint i = 1; i < BLOCK_OBJ_COUNT; ++i)
			  pvCurList[i - 1].mpNext = &pvCurList[i];

		  //將整個串連串到可供使用的串列中
		  pvCurList[BLOCK_OBJ_COUNT - 1].mpNext = mpHeadNode;
		  mpHeadNode = pvCurList;

		  mnMaxCount += BLOCK_OBJ_COUNT;
		  mnValidCount += BLOCK_OBJ_COUNT;

		  mnTotalMemorySize += sizeof(MEMORY_NODE_BLOCK);
	  }

	  MEMORY_NODE* QueryNode()
	  {
		  if(!mpHeadNode)
			  NewBlock();

		  MEMORY_NODE* pCurNode = mpHeadNode;
		  mpHeadNode = mpHeadNode->mpNext;
		  pCurNode->mpNext = 0;
		  --mnValidCount;

		  return pCurNode;
	  }

	  void ReleaseNode(MEMORY_NODE *pNode)
	  {
		  if(pNode)
		  {	
			  pNode->mpNext = mpHeadNode;
			  mpHeadNode = pNode;
			  ++mnValidCount;
		  }
	  }

	  void ReleaseList(MEMORY_NODE* pHead, MEMORY_NODE** ppEnd, uint nCount)
	  {
		  *ppEnd = mpHeadNode;
		  mpHeadNode = pHead;
		  mnValidCount += nCount;
	  }

	  AOBJ* fnQueryObj()
	  {
		  return (AOBJ*)QueryNode();
	  }

	  void fnReleaseObj(void* pObj)
	  {
		  assert(((MEMORY_NODE*)pObj)->mpNext == 0);
		  ReleaseNode((MEMORY_NODE*)pObj);
	  }

	  bool IsFullValid() const
	  {
		  return (mnValidCount == mnMaxCount);
	  }

	  uint GetMaxCount() const
	  {
		  return mnMaxCount;
	  }

	  uint GetMemoryAmount() const
	  {
		  return mnTotalMemorySize;
	  }

	  uint GetUsageCount() const
	  {
		  assert(mnMaxCount >= mnValidCount);
		  return (mnMaxCount - mnValidCount);
	  }

	  static int __cdecl fnPointerCompare(const void* e1, const void* e2)
	  {
		  // 從大排到小
		  return *(int*)e2 - *(int*)e1;
	  }

	  // 整理並釋放多餘的記憶體 (以區塊為單位)
	  void Cleanup()
	  {
		  // 未使用的空間至少需有一個MEMORY_NODE_BLOCK包含的數量
		  if(mnValidCount >= BLOCK_OBJ_COUNT)
		  {
			  uint nBlocks = mnTotalMemorySize / sizeof(MEMORY_NODE_BLOCK);
			  assert(mnTotalMemorySize % sizeof(MEMORY_NODE_BLOCK) == 0);
			  assert(nBlocks * BLOCK_OBJ_COUNT == mnMaxCount);
			  assert(sizeof(void*) == sizeof(uint));

			  MEMORY_NODE_BLOCK** pvpBlocks = (MEMORY_NODE_BLOCK**)
				  ::new unsigned char[sizeof(MEMORY_NODE_BLOCK*) * nBlocks +
				  sizeof(MEMORY_NODE*) * mnValidCount + sizeof(uint) * (nBlocks + mnValidCount)];
			  uint* pvCounts = (uint*)(pvpBlocks + nBlocks);

			  MEMORY_NODE** pvpNodes = (MEMORY_NODE**)(pvCounts + nBlocks);
			  uint* pvParents = (uint*)(pvpNodes + mnValidCount);

			  if(pvpBlocks)
			  {
				  uint i = 0;
				  for(MEMORY_NODE_BLOCK* pmnb = mpHeadBlock; pmnb; pmnb = pmnb->mpNext)
				  {
					  pvpBlocks[i] = pmnb;
					  pvCounts[i] = 0;
					  ++i;
				  }
				  assert(i == nBlocks);
				  qsort(pvpBlocks, i, sizeof(MEMORY_NODE_BLOCK*), fnPointerCompare);

				  i = 0;
				  for(MEMORY_NODE* pmn = mpHeadNode; pmn; pmn = pmn->mpNext)
				  {
					  pvpNodes[i] = pmn;
					  ++i;
				  }
				  assert(i == mnValidCount);
				  qsort(pvpNodes, i, sizeof(MEMORY_NODE*), fnPointerCompare);

				  uint b = 0;
				  for(i = 0; i < mnValidCount; ++i)
				  {
					  while(pvpNodes[i] < pvpBlocks[b]->mvElement)
					  {
						  ++b;
						  assert(b < nBlocks);
					  }

					  assert(pvpNodes[i] < (pvpBlocks[b]->mvElement + BLOCK_OBJ_COUNT));
					  pvParents[i] = b;
					  ++pvCounts[b];
				  }

				  b = 0;
				  mpHeadBlock = 0;
				  for(i = 0; i < nBlocks; ++i)
				  {
					  if(pvCounts[i] >= BLOCK_OBJ_COUNT)
					  {
						  ::delete[] (unsigned char*)(pvpBlocks[i]);
						  pvpBlocks[i] = 0;
						  ++b;	// free block count
						  continue;
					  }

					  // re-link reserved blocks
					  pvpBlocks[i]->mpNext = mpHeadBlock;
					  mpHeadBlock = pvpBlocks[i];
				  }

				  mpHeadNode = 0;
				  for(i = 0; i < mnValidCount; ++i)
				  {
					  if(!pvpBlocks[pvParents[i]])
						  continue;

					  // re-link reserved nodes
					  pvpNodes[i]->mpNext = mpHeadNode;
					  mpHeadNode = pvpNodes[i];
				  }

				  mnValidCount -= BLOCK_OBJ_COUNT * b;
				  mnMaxCount -= BLOCK_OBJ_COUNT * b;
				  mnTotalMemorySize -= sizeof(MEMORY_NODE_BLOCK) * b;

				  delete[] (unsigned char*)(pvpBlocks);
			  }
		  }
	  }

	  static inline AOBJ* fnCastObjectPtr(MEMORY_NODE* node) { return (AOBJ*)node; }
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#include "singleton.h"

template <class T,uint BLOCK_OBJ_COUNT = 100>
class ObjFactory 
{
	friend T;
private:
	TMemPool<T,BLOCK_OBJ_COUNT> mPool;
	CRITICAL_SECTION critical_sec_query;
public:
	ObjFactory() { InitializeCriticalSection(&critical_sec_query); }
	virtual ~ObjFactory() { DeleteCriticalSection(&critical_sec_query); }

	// - for debug
 	inline uint GetObjCount		() {return mPool.GetUsageCount();}
 	inline uint GetMemoryAmount () {return mPool.GetMemoryAmount();}


	inline T* Create()
	{ 
		::EnterCriticalSection(&critical_sec_query);
		T* pt = mPool.fnQueryObj();
		::LeaveCriticalSection(&critical_sec_query);				
		return pt;
	}

	inline void Destroy(void* p)
	{ 
		::EnterCriticalSection(&critical_sec_query);

		mPool.fnReleaseObj(p); 
		::LeaveCriticalSection(&critical_sec_query);
	}
};

#endif