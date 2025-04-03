
#include "sd_card.h"
#include "ff.h"
#include <string>
#ifdef __cplusplus
extern "C" {
	#endif

	size_t sd_get_num();
	sd_card_t* sd_get_by_num(size_t num);

	size_t spi_get_num();
	spi_t* spi_get_by_num(size_t num);

	#ifdef __cplusplus
}
#endif
void sdtest();

namespace sd1{
	void setup();
	void open();
	void open(int i);
	void write1(std::string a);
	void finish();
}
