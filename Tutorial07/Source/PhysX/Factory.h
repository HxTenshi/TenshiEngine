#ifndef _FACTORY_H_
#define _FACTORY_H_

#include <map>

template<
	typename AbstractProduct,
	typename ProductID,
	typename ProductCreatorFunc
>
class Factory{
public:
	void registerProductCreator(const ProductID& id, const ProductCreatorFunc& creator){
		mProductContainer[id] = creator;
	}
	void unregisterProductCreator(const ProductID& id){
		mProductContaine.rease(id);
	}

	AbstractProduct* create(const ProductID& id){
		return mProductContainer[id]();
	}

private:
	typedef std::map<ProductID, ProductCreatorFunc> ProductContainer;
	ProductContainer mProductContainer;
};

#endif