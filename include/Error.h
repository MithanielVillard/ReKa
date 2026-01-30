#ifndef REKAERROR__H_
#define REKAERROR__H_

#include <string>

namespace reka 
{

enum ErrorStatus
{
	REKA_OK,
	REKA_FAILED
};

struct Error
{
	ErrorStatus status;
	std::string message;
};

}
#endif // !REKAERROR__H_
