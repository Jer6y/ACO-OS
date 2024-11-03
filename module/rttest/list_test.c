#include "rttest.h"
#include <aco/errno.h>
#include <aco/list.h>

struct test_list {
	int 			val;
	struct list_head 	_node;
} tList[20];

#define CHECK_RESULT(condition)	do				\
				{				\
					if(condition)		\
						(*success)++;	\
					else			\
						(*error)++;	\
				} while(0)

int rt_list_api(int* success, int* error)
{
	struct list_head ret;
	INIT_LIST_HEAD(&ret);
	CHECK_RESULT(ret.prev == ret.next && ret.prev != NULL);
	
	for(int i=0;i<20;i++)
	{
		tList[i].val = i;
	}
	struct test_list* tmp = list_entry(&(tList[14]._node), struct test_list, _node);
	CHECK_RESULT(tmp->val == 14);
	
	for(int i=0;i<20;i++)
	{
		list_add(&(tList[i]._node), &ret);
	}
	struct list_head* iterator;
	int j = 19;
	list_for_each(iterator, &ret)
	{
		struct test_list* tmp = list_entry(iterator, struct test_list,_node);
		CHECK_RESULT(tmp->val == j);
		j--;
	}
	
	for(int i =0;i<20;i+=2)
	{
		list_del(&(tList[i]._node));
	}

	j = 19;
	list_for_each(iterator, &ret)
        {
		struct test_list* tmp = list_entry(iterator, struct test_list,_node);
                CHECK_RESULT(tmp->val == j);
                j-=2;
        }

	CHECK_RESULT((list_empty(&ret)) == 0);

	struct list_head* tmp_2;
        list_for_each_safe(iterator,tmp_2,&ret)
        {
                struct test_list* tmp = list_entry(iterator, struct test_list,_node);
                list_del(iterator);
        }
	CHECK_RESULT(list_is_singular(&ret) == 0);

	CHECK_RESULT((list_empty(&ret)) == 1);
	
	for(int i=0;i<20;i++)
        {
                list_add_tail(&(tList[i]._node), &ret);
        }
	CHECK_RESULT(list_is_singular(&ret) == 0);

	j=0;
	list_for_each(iterator, &ret)
        {
		struct test_list* tmp = list_entry(iterator, struct test_list,_node);
                CHECK_RESULT(tmp->val == j);
                j++;
        }
	CHECK_RESULT(list_is_singular(&ret) == 0);
	
	list_for_each_safe(iterator,tmp_2,&ret)
        {
                struct test_list* tmp = list_entry(iterator, struct test_list,_node);
                list_del(iterator);
        }
	CHECK_RESULT(list_empty(&ret) == 1);

	for(int i=0;i<10;i++)
        {
                list_add_tail(&(tList[i]._node), &ret);
        }
	
	struct list_head ret_2;
	INIT_LIST_HEAD(&ret_2);
	for(int i=10;i<20;i++)
	{
		list_add_tail(&(tList[i]._node), &ret_2);
	}

	list_splice_tail_init(&ret_2,&ret);
	j=0;
        list_for_each(iterator, &ret)
        {
                struct test_list* tmp = list_entry(iterator, struct test_list,_node);
                CHECK_RESULT(tmp->val == j);
                j++;
        }
	
	list_for_each_safe(iterator,tmp_2,&ret)
        {
                struct test_list* tmp = list_entry(iterator, struct test_list,_node);
                list_del(iterator);
        }

	CHECK_RESULT(list_empty(&ret_2) == 1);
	CHECK_RESULT(list_empty(&ret) == 1);
	CHECK_RESULT(list_is_singular(&ret) == 0);
	
	for(int i=0;i<20;i++)
	{
		if(i ==0)
			list_add_tail(&(tList[i]._node), &ret);
		else
			node_add_bef(&(tList[i-1]._node),&(tList[i]._node));
	}
	CHECK_RESULT(list_empty(&ret) == 0);
	j = 19;
	list_for_each_safe(iterator,tmp_2, &ret)
        {
                struct test_list* tmp = list_entry(iterator, struct test_list,_node);
                CHECK_RESULT(tmp->val == j);
		j--;
		list_del(iterator);
        }
	for(int i=0;i<20;i++)
        {
                if(i ==0)
                        list_add_tail(&(tList[i]._node), &ret);
                else
                        node_add_aft(&(tList[i-1]._node),&(tList[i]._node));
        }
	j=0;
	list_for_each_safe(iterator,tmp_2, &ret)
        {
                struct test_list* tmp = list_entry(iterator, struct test_list,_node);
                CHECK_RESULT(tmp->val == j);
                j++;
                list_del(iterator);
        }
	return 0;
}
