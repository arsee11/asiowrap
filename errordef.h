//errordef.h
//

#ifndef ERROR_DEF_H
#define ERROR_DEF_H

namespace asiow{

enum NetError{
	ERR_NOERROR,
	ERR_CLOSED,
	ERR_ON_BIND,
	ERR_ON_OPEN,
	ERR_ON_SEND,
	ERR_ON_RECEIVE,
	ERR_UNKNOWN
};

}

#endif /*ERROR_DEF_H*/
