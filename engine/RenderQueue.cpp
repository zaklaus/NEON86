#include "StdAfx.h"
#include "RenderQueue.h"
#include "engine.h"

CRenderQueue::CRenderQueue(void)
{
	mCapacity = 4;
	mCount = 0;
	mCommands = (CRenderCommand*)malloc(mCapacity*sizeof(CRenderCommand));
}

BOOL CRenderQueue::PushCommand(UINT kind, const RENDERDATA& data)
{
	if (mCount >= mCapacity)
	{
		mCapacity += 4;
		mCommands = (CRenderCommand*)realloc(mCommands, mCapacity*sizeof(CRenderCommand));

		if (!mCommands)
		{
			MessageBoxA(NULL, "Can't push render command!", "Out of memory error", MB_OK);
			ENGINE->Shutdown();
			return FALSE;
		}
	}

	mCommands[mCount++] = CRenderCommand(kind, data);

	return TRUE;
}

VOID CRenderQueue::Release(void)
{
	free(mCommands);
	mCapacity = mCount = 0;
}


VOID CRenderQueue::ExecutePreDraw(void)
{
	for (UINT i=0; i < mCount; ++i)
	{
		mCommands[i].ExecutePreDraw();
	}
}

VOID CRenderQueue::ExecuteDraw(void)
{
	for (UINT i=0; i < mCount; ++i)
	{
		mCommands[i].ExecuteDraw();
	}
}

VOID CRenderQueue::ExecutePostDraw(void)
{
	for (UINT i=0; i < mCount; ++i)
	{
		mCommands[i].ExecutePostDraw();
	}
}

VOID CRenderQueue::Clear(void)
{
	mCount = 0;
}