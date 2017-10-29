#include <e3test.h>
#include <leaf/include/leaf-label-fib.h>
#include <leaf/include/leaf-e-service.h>
#include <rte_malloc.h>
DECLARE_TEST_CASE(tc_leaf_fib);

START_TEST(leaf_fib_general){
	int idx=0;
	struct leaf_label_entry * base=allocate_leaf_label_base(-1);
	ck_assert_msg(!!base,"OUT OF MEMORY,please reserve enough memory for test leaf fib");
	ck_assert(!!leaf_label_entry_at(base,0));
	ck_assert(!!leaf_label_entry_at(base,NR_LEAF_LABEL_ENTRY-1));
	ck_assert(!leaf_label_entry_at(base,NR_LEAF_LABEL_ENTRY));
	/*environmrntal presetup*/
	for(idx=0;idx<MAX_E_LINE_SERVICES;idx++)
		e_line_base[idx].is_valid=0;
	for(idx=0;idx<MAX_COMMON_NEIGHBORS;idx++)
		neighbor_base[idx].is_valid=0;
	for(idx=0;idx<MAX_COMMON_NEXTHOPS;idx++)
		nexthop_base[idx].is_valid=0;
	struct common_neighbor neighbor;
	struct common_nexthop  nexthop={
		.common_neighbor_index=0,
	};
	ck_assert(register_common_neighbor(&neighbor)==0);
	ck_assert(register_common_nexthop(&nexthop)==0);
	
	struct ether_e_line eline;
	eline.e3iface=0;
	eline.vlan_tci=12;
	eline.label_to_push=0x322;
	eline.NHLFE=0;
	ck_assert(register_e_line_service(&eline)==0);
	eline.vlan_tci++;
	eline.label_to_push++;
	ck_assert(register_e_line_service(&eline)==1);
	ck_assert(find_e_line_service(0)->ref_cnt==0);
	ck_assert(find_e_line_service(1)->ref_cnt==0);
	
	struct leaf_label_entry entry;
	entry.e3_service=e_line_service+3;
	entry.service_index=0;

	ck_assert(set_leaf_label_entry(base,0,&entry)<0);
	entry.e3_service=e_line_service;
	entry.service_index=2;
	ck_assert(set_leaf_label_entry(base,0,&entry)<0);
	entry.e3_service=e_line_service;
	entry.service_index=0;
	ck_assert(!set_leaf_label_entry(base,0,&entry));
	ck_assert(find_e_line_service(0)->ref_cnt==1);
	ck_assert(find_e_line_service(1)->ref_cnt==0);
	ck_assert(!set_leaf_label_entry(base,NR_LEAF_LABEL_ENTRY-1,&entry));
	ck_assert(set_leaf_label_entry(base,NR_LEAF_LABEL_ENTRY,&entry)<0);
	ck_assert(find_e_line_service(0)->ref_cnt==1);
	ck_assert(find_e_line_service(1)->ref_cnt==0);
	entry.e3_service=e_line_service;
	entry.service_index=1;
	ck_assert(!set_leaf_label_entry(base,NR_LEAF_LABEL_ENTRY-1,&entry));
	ck_assert(find_e_line_service(0)->ref_cnt==1);
	ck_assert(find_e_line_service(1)->ref_cnt==1);
	entry.e3_service=e_line_service;
	entry.service_index=1;
	ck_assert(!set_leaf_label_entry(base,0,&entry));
	ck_assert(find_e_line_service(0)->ref_cnt==0);
	ck_assert(find_e_line_service(1)->ref_cnt==1);
	reset_leaf_label_entry(base,0);
	ck_assert(find_e_line_service(0)->ref_cnt==0);
	ck_assert(find_e_line_service(1)->ref_cnt==1);
	reset_leaf_label_entry(base,NR_LEAF_LABEL_ENTRY-1);
	ck_assert(find_e_line_service(0)->ref_cnt==0);
	ck_assert(find_e_line_service(1)->ref_cnt==0);
	
	/*environmrntal presetup*/
	for(idx=0;idx<MAX_E_LINE_SERVICES;idx++)
		e_line_base[idx].is_valid=0;
	for(idx=0;idx<MAX_COMMON_NEIGHBORS;idx++)
		neighbor_base[idx].is_valid=0;
	for(idx=0;idx<MAX_COMMON_NEXTHOPS;idx++)
		nexthop_base[idx].is_valid=0;
	rte_free(base);
}
END_TEST
ADD_TEST(leaf_fib_general);