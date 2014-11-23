//this is to be extended by all classes that use it...
#ifndef __RESOURCE_MASTER_
#define __RESOURCE_MASTER_

template<typename T>
class ResourceMaster
{
public:
	T				rm_Reference;
	int				rm_InUse;
	char*			rm_Filename;
	bool			rm_KeepLoaded;
};

#endif __RESOURCE_MASTER_