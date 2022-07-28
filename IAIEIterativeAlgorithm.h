//---------------------------------------------------------------------------
#ifndef IAIEIterativeAlgorithmH
#define IAIEIterativeAlgorithmH
//---------------------------------------------------------------------------
class IAIEBinaryFile;
class IAIEComputingTimer;
//---------------------------------------------------------------------------
class IAIEIterativeAlgorithm
{
 public:
	 virtual bool __stdcall GetIsPrepared(void) = 0;
	 virtual bool __stdcall GetIsExecuting(void) = 0;
	 virtual bool __stdcall GetIsWaiting(void) = 0;
	 virtual bool __stdcall GetIsFinished(void) = 0;
	 virtual int __stdcall GetIterationIndex(void) = 0;
	 virtual IAIEComputingTimer* __stdcall GetComputingTimer(void) = 0;

	 virtual int __stdcall GetTag(void) = 0;
	 virtual void __stdcall SetTag(int ATag) = 0;

	 virtual void __stdcall Prepare(void) = 0;
	 virtual bool __stdcall ExecuteNextIteration(void) = 0;
	 virtual void __stdcall Finish(void) = 0;
	 virtual void __stdcall Clear(void) = 0;
	 virtual void __stdcall Free(void) = 0;
	 virtual void __stdcall LoadFromFile(IAIEBinaryFile* File) = 0;
	 virtual void __stdcall SaveToFile(IAIEBinaryFile* File) = 0;

 #ifdef __BORLANDC__
	 __property bool IsPrepared = {read=GetIsPrepared};
	 __property bool IsExecuting = {read=GetIsExecuting};
	 __property bool IsWaiting = {read=GetIsWaiting};
	 __property bool IsFinished = {read=GetIsFinished};
	 __property int IterationIndex = {read=GetIterationIndex};
	 __property IAIEComputingTimer* ComputingTimer = {read=GetComputingTimer};

	 __property int Tag = {read=GetTag, write=SetTag};
 #endif
};
//---------------------------------------------------------------------------
#endif