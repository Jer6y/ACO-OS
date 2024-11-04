#include <mm/pageframe.h>
#include <aco/errno.h>

int pages_slfcheck(void)
{
        int bef_count = -233333;
        for(int i=0;i<PGFRAME_PAGE_NUMS;i++)
        {
                lock(&(PAGES[i].lk));
                if(PAGES[i].pgtype == PAGE_BUDDYALLOCATOR || PAGES[i].pgtype == SLAB_ALLOCATOR)
                {
                        if((PAGES[i].meta.buddy_flags & PG_BUDDY_FLAG_STATIC) != 0)
                        {
                                if(PAGES[i].meta.buddy_flags != PG_BUDDY_FLAG_STATIC)
                                {
                                        unlock(&(PAGES[i].lk));
                                        return -EFAULT;
                                }
                        }
                        if((PAGES[i].meta.buddy_flags & PG_BUDDY_FLAG_HEAD) != 0)
                        {
                                if((PAGES[i].meta.buddy_flags & PG_BUDDY_FLAG_BODY) !=0 || (PAGES[i].meta.buddy_flags & PG_BUDDY_FLAG_STATIC) !=0)
                                {
                                        unlock(&(PAGES[i].lk));
                                        return -EFAULT;
                                }
                                if(bef_count == -233333)
                                {
                                        bef_count = ((1<< PAGES[i].meta.buddy_order) -1);
                                }
                                else
                                {
                                        if(bef_count != 0)
                                        {
                                                unlock(&(PAGES[i].lk));
                                                return -EFAULT;
                                        }
                                        bef_count = ((1<< PAGES[i].meta.buddy_order)-1);
                                }
                        }
                        if((PAGES[i].meta.buddy_flags & PG_BUDDY_FLAG_BODY) != 0)
                        {
                                if(PAGES[i].meta.buddy_flags  != PG_BUDDY_FLAG_BODY)
                                {
                                        unlock(&(PAGES[i].lk));
                                        return -EFAULT;
                                }
                                if(PAGES[i].meta.buddy_order != CONFIG_BUDDYSYSTEM_MAX_ORDER)
                                {
                                        unlock(&(PAGES[i].lk));
                                        return -EFAULT;
                                }
                                bef_count--;
                        }
                }
		else if (PAGES[i].pgtype == VMA_AREA)
		{
			unlock(&PAGES[i].lk);
			return -EFAULT;
		}
                unlock(&PAGES[i].lk);
        }
        return 0;
}

