#include <stdlib.h>
#include <stdio.h>
#include "tinterp.h"

int main(void)
{
    char buf[TINTERP_BUF_SIZE];

    sscanf("Помощь", "%[^\n]", buf);
    tinterp_parse(buf);

    sscanf("Установить время 16:25:30", "%[^\n]", buf);
    tinterp_parse(buf);

    return 0;
}
