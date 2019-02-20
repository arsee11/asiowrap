///utils.h
//

#ifndef UTILS_H
#define UTILS_H

#include <memory>
#include "endpoints.h"

namespace asiow{

struct Item{
	
	Item(const void* d, size_t s)
		:size(s)
	{
		data = new uint8_t[s];
		const uint8_t* dt = (const uint8_t*)d;
		std::copy(dt, dt+s, data);
	}

	virtual ~Item(){
		if(data != nullptr)
			delete[] data;
	}

	uint8_t* advancePtr(size_t n){ return data+nsent+n; }

	uint8_t* data=nullptr;
	size_t size=0;
	size_t nsent=0;
};

using item_ptr=std::shared_ptr<Item>;


struct UdpItem : public Item{

	UdpItem(const void* d, size_t s, const UdpEndpoint& in_remote)
		:Item(d, s)
		,remote(in_remote)
	{
	}

	UdpEndpoint remote;
};

using udpitem_ptr = std::shared_ptr<UdpItem>;

}//asiow

#endif/*UTILS_H*/
