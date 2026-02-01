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
	Error(ErrorStatus status, std::string message) : status(status), message(message) {};

	ErrorStatus status;
	std::string message;
};

}
#endif // !REKAERROR__H_
