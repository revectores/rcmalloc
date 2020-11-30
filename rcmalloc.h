#ifndef __RCMALLOC_H_
#define __RCMALLOC_H_

#define E_NO_SPACE            1
#define E_CORRUPT_FREESPACE   2
#define E_PADDING_OVERWRITTEN 3
#define E_BAD_ARGS            4
#define E_BAD_POINTER         5

#define M_BESTFIT   0
#define M_WORSTFIT  1
#define M_FIRSTFIT  2

extern int m_error;

int	   rcinit(int size_of_region);
void * rcmalloc(int size, int style);
int    rcfree(void * ptr);
void   rcdump();

#endif

