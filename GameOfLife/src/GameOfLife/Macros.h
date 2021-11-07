#pragma once

#ifdef GOL_DEBUG
	#define GOL_LOG(X) std::cout << X << std::endl 
#else
	#define GOL_LOG(X) 
#endif // GOL_DEBUG
