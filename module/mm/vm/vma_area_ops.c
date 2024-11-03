#include <vma_area_ops.h>
#include <aco/string.h>
#include <aco/errno.h>

void vma_area_val_init(vma_area_t* vma_area, vmar_type_t vmar_tp, viraddr_t va, phyaddr_t pa, uintptr_t size, pageframe_t* pgfm, vm_prot prot)
{
        avl_node_init(&(vma_area->a_node));
        INIT_NODE(&(vma_area->l_node));
        vma_area->vmar_typ = vmar_tp;
        vma_area->va = va;
        vma_area->pa = pa;
        vma_area->size = size;
        vma_area->pgfm = pgfm;
        vma_area->prot = prot;
}

void vma_area_val_deinit(vma_area_t* vma_area)
{
        memset((void*)vma_area, 0, sizeof(vma_area_t));
}

FUNC_BUILTIN int insert_compare_vmarea(struct avl_node* want, struct avl_node* have)
{
        vma_area_t* want_node = container_of(want, vma_area_t, a_node);
        vma_area_t* have_node = container_of(have, vma_area_t, a_node);
        int ret = 0;
        if( !((want_node->va + want_node->size <= have_node->va)\
                        || (want_node->va >= have_node->va + have_node->size)))
                ret = 0;
        if( want_node->va + want_node->size <= have_node->va)
                ret = -1;
        if( want_node->va >= have_node->va + have_node->size)
                ret = 1;
        return ret;
}

int vma_area_hang(vma_t* vma, vma_area_t* vma_area)
{
        struct avl_node* duplicated = NULL;
        avl_node_add(&(vma->a_root), &(vma_area->a_node), insert_compare_vmarea, duplicated);
        if(duplicated != NULL)
                return -EEXIST;
        duplicated = avl_node_prev(&(vma_area->a_node));
        if(duplicated == NULL)
                list_add(&(vma_area->l_node),&(vma->l_list));
        else
        {
                vma_area_t* prev_node = container_of(duplicated, vma_area_t, a_node);
                node_add_aft(&(prev_node->l_node), &(vma_area->l_node));
        }
        return 0;
}

void vma_area_unhang(vma_t* vma, vma_area_t* vma_area)
{
        list_del(&(vma_area->l_node));
        avl_node_erase(&(vma_area->a_node), &(vma->a_root));
        return;
}
