#include "ProductBuilderFactory.h"

#ifdef Product_1
#include "Product1Builder.h"
#endif

#ifdef Product_2
#include "Product2Builder.h"
#endif

#ifdef Product_3
#include "Product3Builder.h"
#endif

#ifdef Product_4
#include "Product4Builder.h"
#endif

#ifdef Product_5
#include "Product5Builder.h"
#endif

IProductBuilder* ProductBuilderFactory::create()
{
#if defined(Product_1)
    return new Product1Builder();
#elif defined(Product_2)
    return new Product2Builder();
#elif defined(Product_3)
    return new Product3Builder();
#elif defined(Product_4)
    return new Product4Builder();
#elif defined(Product_5)
    return new Product5Builder();
#else
    return nullptr;
#endif
}
