#include "rttest.h"
#include <aco/errno.h>
#include <aco/avlmini.h>
#include <aco/oop.h>

#define CHECK(condition)        do                              \
                                {                               \
                                        if(condition)           \
                                                (*success)++;   \
                                        else                    \
                                                (*error)++;     \
                                } while(0)

struct avl_root test_tree;

struct  at_node {
	int 		value;
	struct avl_node	_node;
} testcases[20];

static int compare_func(struct avl_node* new_node, struct avl_node* parent)
{
	struct at_node* new_atnode = container_of(new_node, struct at_node, _node);
	struct at_node* parent_atnode = container_of(parent, struct at_node, _node);
	return (new_atnode->value - parent_atnode->value);
}

static int find_func(int key, struct avl_node* parent)
{
        struct at_node* parent_atnode = container_of(parent, struct at_node, _node);
        return (key - parent_atnode->value);
}


int rt_avlmini(int* success, int* error)
{
	for(int i=0;i<20;i++)
	{
		testcases[i].value = i;
		avl_node_init(&(testcases[i]._node));
	}
	avl_root_init(&test_tree);
	CHECK(avl_root_empty(&test_tree) == 1);
	for(int i=0;i<20;i++)
	{
		struct avl_node* duplicated = NULL;
		CHECK(avl_node_empty(&(testcases[i]._node)) == 1);
		avl_node_add(&test_tree, &(testcases[i]._node), compare_func, duplicated);
		CHECK(duplicated == NULL);
		CHECK(avl_node_empty(&(testcases[i]._node)) == 0);
		avl_node_add(&test_tree, &(testcases[i]._node), compare_func, duplicated);
		CHECK(duplicated != NULL);
		CHECK(duplicated == &(testcases[i]._node));
		CHECK(avl_root_empty(&test_tree) == 0);
	}
	for(int i=0;i<20;i++)
	{
		struct avl_node* res_node = NULL;
		avl_node_find(&test_tree, i, find_func, res_node);
		CHECK(res_node == &(testcases[i]._node));
	}
	for(int i=20;i<30;i++)
        {
                struct avl_node* res_node = (struct avl_node*)0x1234;
                avl_node_find(&test_tree, i, find_func, res_node);
                CHECK(res_node == NULL);
        }
	CHECK(avl_node_first(&test_tree) == &(testcases[0]._node));
	CHECK(avl_node_last(&test_tree) == &(testcases[19]._node));

	struct avl_node* iterator = avl_node_first(&test_tree);
	int test_ptr = 0;
	while(iterator != NULL)
	{
		struct at_node* it_atnode = container_of(iterator, struct at_node, _node);
		CHECK(it_atnode->value == test_ptr);
		test_ptr++;
		iterator = avl_node_next(iterator);
	}
	CHECK(test_ptr == 20);
	iterator = avl_node_last(&test_tree);
	test_ptr = 19;
	while(iterator != NULL)
	{
		struct at_node* it_atnode = container_of(iterator, struct at_node, _node);
		CHECK(it_atnode->value == test_ptr);
		test_ptr--;
		iterator = avl_node_prev(iterator);
	}
	CHECK(test_ptr == -1);
	for(int i=0;i<20;i++)
	{
		struct avl_node* res_node = NULL;
		CHECK(avl_node_first(&test_tree) == &(testcases[i]._node));
		avl_node_find(&test_tree, i, find_func, res_node);
		CHECK(res_node == &(testcases[i]._node));
		avl_node_erase(&(testcases[i]._node), &test_tree);
		CHECK(avl_node_first(&test_tree) != &(testcases[i]._node));
		avl_node_find(&test_tree, i, find_func, res_node);
		CHECK(res_node == NULL);
	}
	return 0;
}
