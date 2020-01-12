#include "my.h"
void print_buf(struct ftpmsg buf)
{
    struct commands *pt;
    for (pt = cmdtbl; pt->desc; pt++) {
        if  (pt->typenum == buf.type) 
            if  (pt->codenum == buf.code || pt->codenum == -1) 
                break;
    }
    printf("type 0x%x, " ,buf.type);
    printf("code 0x%x, (%s), datalen %d\n", buf.code, pt->desc, buf.datalen);
    printf("data:%s\n", buf.data);


    fprintf(stderr, "*****************************\n\n\n");
}
