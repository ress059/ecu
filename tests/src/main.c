#include <ecu/circular_sll.h>

int main()
{
    struct circular_sll my_list;
    struct circular_sll_node my_node;

    circular_sll_ctor(&my_list);
    circular_sll_push_back(&my_list, &my_node);
}